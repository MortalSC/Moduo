#pragma once

/**
 * 线程类：线程对象记录了：
 *  => 线程是否启动
 *  => 线程是否等待
 *  => 线程 tid
 *  => 线程 名称
 *  => 线程 方法
 */
#include "noncopyable.h"

#include <functional>
#include <thread>
#include <memory>
#include <unistd.h>
#include <string>
#include <atomic>

namespace myMuduo
{
    class Thread : noncopyable
    {
    public:
        using ThreadFunc = std::function<void()>;

        explicit Thread(ThreadFunc func, const std::string &name = std::string());
        ~Thread();
        // 线程启动
        void start();
        // 线程等待
        void join();
        // 是否启动
        bool started() const { return started_; }
        // 获取 tid【相当于top指令获取的】
        pid_t tid() const { return tid_; }
        // 获取线程方法名
        const std::string name() const { return funcName_; }

        // 获取线程个数
        static int numCreated() { return numCreated_; }

    private:
        // 设置默认线程名称
        void setDefaultName();

    private:
        bool started_; // 标识线程启动
        bool joined_;  // 标识线程等待
        // 不要使用thread直接声明定义对象
        // 与Linux下原生的相同，定义时，就启动了线程
        // std::thread thread_;
        // 借助指针指针来管理
        std::shared_ptr<std::thread> thread_;
        pid_t tid_;                         // 线程 id
        ThreadFunc func_;                   // 存储线程方法
        std::string funcName_;              // 线程名
        static std::atomic_int numCreated_; // 线程个数计数
    };
}