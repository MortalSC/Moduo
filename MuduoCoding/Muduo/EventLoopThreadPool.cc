#include "EventLoopThreadPool.h"
#include "EventLoopThread.h"

namespace myMuduo
{
    EventLoopThreadPool::EventLoopThreadPool(EventLoop *baseLoop,
                                             const std::string &nameArg)
        : baseLoop_(baseLoop),
          name_(nameArg),
          started_(false),
          numThreads_(0),
          next_(0)
    {
    }
    EventLoopThreadPool::~EventLoopThreadPool()
    {
    }

    // 启动
    void EventLoopThreadPool::start(const ThreadInitCallback &cb )
    {
        started_ = true; // 设置启动状态

        // 如果没有调用过 setThreadNum 就不会进入 for 循环
        // 即：没有启动 subLoop 去工作
        for (int i = 0; i < numThreads_; ++i)
        {
            // 名字设置
            char buf[name_.size() + 32];
            snprintf(buf, sizeof(buf), "%s %d", name_.c_str(), i);
            // 创建事件循环线程与线程对象
            EventLoopThread *t = new EventLoopThread(cb, buf);
            // 存入容器，并使用智能指针管理（不用手动释放资源）
            threads_.push_back(std::unique_ptr<EventLoopThread>(t));
            // 放入容器管理线程，并启动
            loops_.push_back(t->startLoop());
        }

        // 整个服务端只有一个 mainLoop 在运行
        if (numThreads_ == 0 && cb)
        {
            cb(baseLoop_);
        }
    }
    // 如果在工作在多线程中
    // baseLoop以轮询的方式把Channel分配给subLoop
    EventLoop *EventLoopThreadPool::getNextLoop()
    {
        // 如果没有创建过其他的线程与EventLoop，则返回主线程/baseLoop
        EventLoop* loop = baseLoop_;

        // 如果创建了，才用轮询的方式，分配线程执行任务
        if(!loops_.empty()){
            // 使用顺序遍历容器中存在的线程与EventLoop去处理任务
            loop = loops_[next_];
            ++next_;
            if(next_ >= loops_.size()) next_ = 0;
        }

        return loop;
    }

    std::vector<EventLoop *> EventLoopThreadPool::getAllLoops()
    {
        if (loops_.empty())
        {
            // 说明线程池中没有线程
            // 即：整体只有一个 主线程（mainLoop）
            return std::vector<EventLoop *>(1, baseLoop_);
        }
        else
        {
            return loops_;
        }
    }
}