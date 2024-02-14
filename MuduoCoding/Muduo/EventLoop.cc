#include "EventLoop.h"
#include "Log.h"
#include "Poller.h"
#include "Channel.h"

#include <sys/eventfd.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

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
        wakeupChannel_(new Channel(this, wakeupFd_)), 
        currentActiveChannel_(nullptr)
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
        // 每个EventLoop都将监听wakeupChannel的EPOLLIN读事件
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

}