#pragma once

/**
 * EventLoop 事件循环的类
 * => 主要模块
 *  => Channel【实现对需要管理的事件封装（sockfd 与 操作行为{EPOLL_IN等}）】
 *  => Poller【epoll的抽象，我们就实现epoll（原版包含poll）】
 */

#include "noncopyable.h"
#include "Timestamp.h"
#include "CurrentThread.h"

#include <functional>
#include <vector>
#include <atomic>
#include <memory>
#include <mutex>

namespace myMuduo
{

    class Channel;
    class Poller;
    class EventLoop : noncopyable
    {
    public:
        // 定义回调类型
        using Functor = std::function<void()>;

        EventLoop();
        ~EventLoop();

        // 开启事件循环函数
        void loop();
        // 退出事件循环
        void quit();

        Timestamp pollReturnTime() const { return pollReturnTime_; }

        // 在当前【loop】执行
        void runInLoop(Functor cb);
        // 把 cd（回调）放入队列中，唤醒 loop 所在线程执行 cb
        void queueInLoop(Functor cb);

        // 用于唤醒loop所在线程
        void wakeup();

        // EventLoop 方法 => Poller 方法
        void updateChannel(Channel *channel);
        void removeChannel(Channel *channel);
        bool hasChannel(Channel *channel);

        // 判断是否 EventLoop 是否在创建它的线程中，从而确定执行回调方法时的选择问题【与runInLoop / queueInLoop 相关】
        bool isInLoopThread() const { return threadId_ == CurrentThread::tid(); }

    private: // 私有接口
        // wake up
        void headleRead();
        // 执行回调
        void doPendingFunctors();

    private: // 私有成员
        using ChannelList = std::vector<Channel *>;
        // 引用 C++11 原子操作，底层实现方式CAS
        // 控制是否循环相关！
        std::atomic_bool looping_;
        std::atomic_bool quit_; // 标志 loop 退出循环

        // 标识创建EventLoop的线程id
        const pid_t threadId_; // 记录当前lop所在线程的 id，可以用于判断是否在创建EventLoop时所在的线程（自己所处的线程）

        // 与 Poller 相关：记录时间点，与 Poller 管理
        Timestamp pollReturnTime_;       // 记录 Poller 返回发生事件Channels的时间点
        std::unique_ptr<Poller> poller_; // EventLoop 管理的 Poller

        // 作为主事件循环EventLoop分发任务给工作EventLoop的唤醒操作
        // 作用：当获取到一个新用户的Channel
        // 通过轮询算法选择一个subReactor[工作EventLoop]
        // 由于subReactor可能处于线程阻塞态，
        // wakeupFd_ 该成员变量就是用于唤醒/通知线程进行工作处理的！
        // 是通过调用系统调用 eventfd 函数获取，比socketpair高效
        int wakeupFd_;
        std::unique_ptr<Channel> wakeupChannel_;

        // 与 Channel 相关的
        ChannelList activeChannels_; // 存储就绪的 Channel

        std::atomic_bool callingPendingFunctors_; // 标识当前 loop 是否有需要执行的回调操作
        std::vector<Functor> pendingFunctors_;    // 存储 loop 所需执行的所有回调操作
        std::mutex mutex_;                        // 互斥锁：用于保护上面 vector 容器的线程安全操作
    };
}