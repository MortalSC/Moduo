#include "EventLoop.h"
#include "Log.h"
#include "Poller.h"
#include "Channel.h"

#include <sys/eventfd.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <memory>

namespace myMuduo
{
    // 我们遵循：one loop per thread is usually a good model
    // 防止一个线程创建多个 EventLoop
    // 如果某个线程没有创建过 EventLoop，则 t_loopInThisThread 指针指向 nullptr
    // 否则就是执行线程中存在的 EventLoop
    // __thread <=> thread_local
    __thread EventLoop *t_loopInThisThread = nullptr;

    // 定义默认的 Poller IO 复用的超时时间
    const int kPollTimeMs = 10000;

    // 用于创建 wakeup fd：用来唤醒/通知subReactor处理新来的Channel
    int createEventFd()
    {
        int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
        if (evtfd < 0)
        {
            // 创建出错：无法执行工作任务
            Log_FATAL("eventfd() error: %d\n", errno);
        }
        return evtfd;
    }

    EventLoop::EventLoop()
        : looping_(false), quit_(false), callingPendingFunctors_(false), // 一开始没有需要处理的回调
          threadId_(CurrentThread::tid()),                               // 记录当前线程id
          poller_(Poller::newDefualtPoller(this)),
          wakeupFd_(createEventFd()),
          wakeupChannel_(new Channel(this, wakeupFd_))
    {
        Log_DEBUG("EventLoop create %p in thread %d \n", this, threadId_);
        if (t_loopInThisThread)
        {
            // 已存在 EventLoop
            Log_FATAL("Another EvnetLoop %p exists in this thread %d \n", t_loopInThisThread, threadId_);
        }
        else
        {
            // 不存在则创建
            t_loopInThisThread = this;
        }

        // 设置wake up的事件类型以及发生事件后的回调操作
        wakeupChannel_->setReadOnlyCallback(std::bind(&EventLoop::headleRead, this));
        // 每个EventLoop都具备监听wakeupChannel的EPOLLIN读事件
        // 即：赋予subReactor可以读取MainReactor设置的线程唤醒操作
        wakeupChannel_->enableReading();
    }

    EventLoop::~EventLoop()
    {
        wakeupChannel_->disableAll(); // 关闭所有的关心事件
        wakeupChannel_->remove();     // 从 Poller 中删除
        ::close(wakeupFd_);
        t_loopInThisThread = nullptr;
    }

    // wake up
    void EventLoop::headleRead()
    {
        uint64_t one = 1;
        ssize_t n = read(wakeupFd_, &one, sizeof(one));
        if (n != sizeof(one))
        {
            Log_ERROR("EventLoop::headleRead() read %d bytes instead of 8 \n", n);
        }
    }

    // 开启事件循环函数
    void EventLoop::loop()
    {
        // 开启事件循环
        // 初始化循环状态和退出状态
        looping_ = true;
        quit_ = false;
        Log_INFO("EventLoop %p start looping \n", this);
        // 循环体
        while (!quit_)
        {
            // 清理就绪数组内容
            activeChannels_.clear();
            // 调用 Poller::poll 方法[即：epoll_wait]获取就绪的资源
            // 此处监听 clientfd / wakeupfd
            // clientfd：就是与客户端通信的
            // wakeupfd：mainLoop 唤醒 subLoop
            pollReturnTime_ = poller_->poll(kPollTimeMs, &activeChannels_);
            for (Channel *channel : activeChannels_)
            {
                // 处理每一个就绪的资源
                // 循环内就是 Poller 向 EventLoop 上报了发生了的事件
                // EventLoop 可以取通知 Channel 进行相应的处理【调用回调】
                channel->handleEvnet(pollReturnTime_);
            }
            // 执行当前 EventLoop 事件循环处理的回调操作
            /**
             * IO 线程 mainReactor/mainLoop accept（新用户）链接，需要使用一个channel打包fd及其回调
             * 已连接用户的 Channel 需要分发给 workReactor/subReactor/subLoop 去执行任务
             * mainLoop 事先注册一个回调cb（需要subLoop执行）
             * subLoop被wakeup唤醒后，就执行以下方法，即：执行mainLoop注册的回调
             */
            doPendingFunctors();
        }
        Log_INFO("EventLoop %p stop looping \n", this);
        looping_ = false;
    }

    // 退出事件循环
    /**
     * 如果某个 loop/线程中 自己调用了自己的 quit，则说明它没有被阻塞，因为quit方法也是一个回调响应
     * 如果某个 loop/线程中 调用其他的 loop/线程中的 quit，
     *  => 首先，该loop/线程应该处于被唤醒状态
     *  => 其次，调用了 quit，则 poll方法就会返回告知事件发生，quit被设置为true，不在进行事件循环
     */
    void EventLoop::quit()
    {
        // 处理自身的
        quit_ = true;
        // 处理可能操作其他的
        if (!isInLoopThread())
        {
            // 此处就是可能存在 subLoop 中调用了 mainLoop 的 quit
            // 需要进行对其他线程的唤醒
            wakeup();
        }
    }

    // 在当前【loop】执行
    void EventLoop::runInLoop(Functor cb)
    {
        if (isInLoopThread())
        {
            // 在当前线程中执行 cb
            cb();
        }
        else
        {
            // 在其他线程执行，需要唤醒loop所在的线程执行cb
            queueInLoop(cb);
        }
    }
    // 把 cd（回调）放入队列中，唤醒 loop 所在线程执行 cb
    void EventLoop::queueInLoop(Functor cb)
    {
        {
            // queueInLoop可能被多个线程访问
            // 其中需要访问 pendingFunctors_，必须保证线程安全
            std::unique_lock<std::mutex> lock(mutex_);
            pendingFunctors_.emplace_back(cb);
            // emplace_back 和 push_back 的区别在于：直接构造和拷贝构造
        }
        // 唤醒与刚刚加入的回调关联的 loop 去执行
        if (!isInLoopThread() || callingPendingFunctors_)
        {
            /**
             * !isInLoopThread()：
             *  => 对应的是需要执行的回调非当前loop/线程需要执行的
             *  => 而是其他loop/线程需要执行的回调
             *  => 此时我们需要去唤醒那个loop/线程
             * callingPendingFunctors_：
             *  => 对应的是需要执行的回调刚好是当前loop/线程需要执行的
             *  => 此时，可能当前loop/线程正在执行 doPendingFunctors()
             *  => 如果 doPendingFunctors() 执行完了，就可能因为没有事件而在 poll 处阻塞！
             *  => 我们需要把他唤醒
            */
            wakeup();
        }
    }

    // 用于唤醒loop所在线程
    // 向wakeupfd写一个数据，就可以实现事件发生
    void EventLoop::wakeup(){
        uint64_t one = 1;
        ssize_t n = write(wakeupFd_, &one, sizeof(one));
        if(n!=sizeof(one)){
            Log_ERROR("EventLoop::wakeup() write %lu bytes instead of 8\n",n);
        }
    }

    // EventLoop 方法 => Poller 方法
    void EventLoop::updateChannel(Channel *channel){
        poller_->updateChannel(channel);
    }
    void EventLoop::removeChannel(Channel *channel){
        poller_->removeChannel(channel);
    }
    bool EventLoop::hasChannel(Channel *channel){
        return poller_->hasChannel(channel);
    }

    void EventLoop::doPendingFunctors(){
        std::vector<Functor> functors;
        callingPendingFunctors_ = true;
        {
            std::unique_lock<std::mutex> lock(mutex_);
            functors.swap(pendingFunctors_);
        }
        //=====================================
        /**
         * 此时为什么创建一个局部的functor？
         *  => 如果基于原容器操作！我们是有所得约束的！
         *  => 每执行完一个回调，就要从容器中删除。
         *  => 此时持有锁，不能被写入！
         *  => 如果这个时候，恰好又有新的资源就绪
         *  => 那么就有由于正在执行回调而导致无法写入的阻塞
         *  =>=================================
         *  => 使用局部对象与原容器直接交换！
         *  => 就实现了一次性拿出回调，原容器可以立即解放不再被锁定
         *  => doPendingFunctors() 在临时容器中，取回调执行
         *  => 同时，如果有新的回调加入，由于原容器没有锁得限制，可以写入
         *  => 一定程度上实现了并发。
        */
        //=====================================
        for(const Functor& functor : functors)
        {
            functor();  // 执行当前loop需要执行的回调方法
        }
        callingPendingFunctors_ = false;
    }

}