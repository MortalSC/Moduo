#pragma once
#include "noncopyable.h"
#include "Thread.h"

#include <functional>
#include <mutex>
#include <condition_variable>

/**
 * EventLoopThread类：用于关联一个EventLoop和一个线程
 */

namespace myMuduo
{
    class EventLoop;
    class EventLoopThread : noncopyable
    {
    public:
        using ThreadInitCallback = std::function<void(EventLoop *)>;

        EventLoopThread(const ThreadInitCallback &cb = ThreadInitCallback(), const std::&name = std::string());
        ~EventLoopThread();

        // 启动事件循环
        EventLoop *startLoop();

    private:
        // 线程函数：创建 EventLoop 对象
        void threadFunc();

        EventLoop *loop_;
        bool exiting_; // 是否退出循环
        Thread thread_;
        std::mutex mutex_;
        std::condition_variable cond_;
        ThreadInitCallback callback_; // 线程初始化回调
    };
}