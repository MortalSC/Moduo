#pragma once
#include "noncopyable.h"

#include <functional>
#include <string>
#include <vector>
#include <memory>

namespace myMuduo
{
    class EventLoop;
    class EventLoopThread;
    class EventLoopThreadPool : noncopyable
    {
    public:
        using ThreadInitCallback = std::function<void(EventLoop *)>;

        EventLoopThreadPool(EventLoop* baseLoop, const std::string &nameArg);
        ~EventLoopThreadPool();

        // 设置底层线程数量
        void setThreadNum(int numThreads){numThreads_ = numThreads;}

        // 启动
        void start(cosnt ThreadInitCallback& cb = ThreadInitCallback());

        // 如果在工作在多线程中
        // baseLoop以轮询的方式把Channel分配给subLoop
        EventLoop* getNextLoop();

        // 获取池中所有的 loop
        std::vector<EventLoop*> getAllLoops();
    
        bool started()const{return started_;}

        const std::string name() const {return name_;}
    
    private:
        EventLoop *baseLoop_; // 基础的用户创建的loop，用于接收新用户链接的
        std::string name_;
        bool started_;
        int numThreads_; // 线程数量
        int next_;
        // 是否需要手动释放资源？
        std::vector<std::unique_ptr<EventLoopThread>> threads_; // 管理所有线程
        std::vector<EventLoop *> loops_;                        // 管理所有事件的指针
    };
}