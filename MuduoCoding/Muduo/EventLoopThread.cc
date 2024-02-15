#include "EventLoopThread.h"
#include "EventLoop.h"
namespace myMuduo
{
    EventLoopThread::EventLoopThread(const ThreadInitCallback &cb = ThreadInitCallback(), const std::&name = std::string())
        : loop_(nullptr), // 实际的 EventLoop 需要在threadFunc中创建
          exiting_(false),
          thread_(std::bind(&EventLoopThread::threadFunc, this), name), // 创建 Thread 对象（子线程需要调用start才进行创建）
          mutex_(),                                                     // 调用默认构造
          cond_(),                                                      // 调用默认构造
          callback_(cb)
    {
    }
    EventLoopThread::~EventLoopThread()
    {
        exiting_ = true; // 修改退出状态
        if (!loop_)
        {
            // 线程退出时，也需要关闭事件循环
            loop_->quit();
        }
    }

    // 启动事件循环
    EventLoop *EventLoopThread::startLoop()
    {
        // 启动线程 => 执行 func => EventLoopThread & threadFunc
        thread_->start();

        EventLoop *loop = nullptr;

        {
            std::unique_lock<std::mutex> lock(mutex_);
            while (loop_ == nullptr)
            {
                cond_.wait(lock); // 等待绑定成功【看threadFunc()函数执行情况】
            }
            loop = loop_;
        }
        return loop;
    }

    // 线程函数：创建 EventLoop 对象
    // 在新的线程中进行运行
    void EventLoopThread::threadFunc()
    {
        // 在线程中创建一个 EventLoop 对象
        EventLoop loop; // 设计核心：one loop per thread is usually a good model

        // 如果存在回调
        if (callback_)
        {
            callback_(&loop);
        }

        {
            // 加锁控制
            std::unique_lock<std::mutex> lock(mutex_);
            loop_ = &loop;
            // 使用条件变量：确保绑定成功！
            cond_.notify_one(); // 成功就进行通知
        }

        // 启动事件循环，处理工作
        loop.loop();    // 调用 EvnetLoop 的 loop 方法，开启了 Poller 的 epoll
    
        // 到此则说明事件循环关闭 => 服务器程序要关闭了
        std::unique_lock<std::mutex> lock(mutex_);
        loop_ = nullptr;
    }
}