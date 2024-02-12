#pragma once

/**
 * Channel：理解为通道
 * => 用于封装 sockfd 和 与之对应的操作（EPOLL_IN、EPOLL_OUT 等事件）
 * => 以及所绑定的 Poller（本项目中特指Epoll） 返回的具体事件
 */
#include "noncopyable.h"
#include "Timestamp.h"

#include <functional>
#include <memory>

namespace myMuduo
{
    class EventLoop; // 前置声明（只用到指针）
    // class Timestamp;    // 前置声明（需要实例类型了，不能只声明不含头文件）
    class Channel : noncopyable
    {
    public:
        // typedef std::function<void()> EventCallback;                     // 事件回调方法
        // typedef std::function<void(Timestamp)> ReadOnlyEventCallback;    // 只读事件回调方法
        using EventCallback = std::function<void()>;                  // C++11 写法
        using ReadOnlyEventCallback = std::function<void(Timestamp)>; // C++11 写法

        Channel(EventLoop *loop, int fd);
        ~Channel();

        // fd得到Poller通知以后，处理事件的回调
        void handleEvnet(Timestamp receiveTime);

        // 设置回调函数对象
        // 使用 move 避免拷贝，将左值转成右值引用
        void setReadOnlyCallback(ReadOnlyEventCallback cb) { readOnlyCallback_ = std::move(cb); }
        void setReadCallback(EventCallback cb) { writeCallback_ = std::move(cb); }
        void setCloseCallback(EventCallback cb) { closeCallback_ = std::move(cb); }
        void setErrorCallback(EventCallback cb) { errorCallback_ = std::move(cb); }

        // 防止channel被释放（remove）后，以及执行回调方法
        void tie(const std::shared_ptr<void> &);

        int fd() const { return fd_; }
        int events() const { return events_; }
        // Channel 自身不能监听fd事件发生
        // Poller可以，故提供接口用于Poller告知Channel
        int set_revents(int revt) { revents_ = revt; }
        // 不关注事件
        bool isNoneEvent() const { return events_ == kNoneEvnet; }

        // 设置 fd 上的事件状态
        // update() : 即调用 epoll_ctl
        void enableReading()
        {
            events_ |= kReadEvent;
            update();
        }
        void disableReading()
        {
            events_ &= ~kReadEvent;
            update();
        }
        void enableWriting()
        {
            events_ |= kWriteEvent;
            update();
        }
        void disableWriting()
        {
            events_ &= ~kWriteEvent;
            update();
        }
        void disableAll()
        {
            events_ = kNoneEvnet;
            update();
        }

        // 返回 fd 的事件状态
        bool isWriting() const { return events_ & kWriteEvent; }
        bool isReading() const { return events_ & kReadEvent; }
        //bool isNoneEvent() const { return events_ == kNoneEvnet; }

        int index() { return index_; }
        void set_index(int idx) { index_ = idx; }

        // one loop per thread
        EventLoop *onwerLoop() { return loop_; }
        // 删除 Channel
        void remove();

    private:
        // 调用 Poller（epoll_ctl）
        void update();
        // 处理受保护的event事件
        void headleEventWithGuard(Timestamp receiveTime);

    private:
        // 标识 sockfd 的关注状态
        static const int kNoneEvnet;  // 什么都不关心
        static const int kReadEvent;  // 关心读事件
        static const int kWriteEvent; // 关心写事件

        EventLoop *loop_; // 事件循环
        const int fd_;    // fd，Poller监听的对象
        int events_;      // 注册监听感兴趣的事件
        int revents_;     // Poller返回具体发生的事件
        int index_;       //

        std::weak_ptr<void> tie_; // 绑定自己：用于跨线程检测对象是否存在
        bool tied_;
        /**
         * weak_ptr的两个作用：
         * => 解决 shared_ptr 循环引用问题
         * => 多线程中跨线程的对象生存状态监听（Channel是否存在）
         *  => 通过类型提升实现！
         *  => 如果提升成功，则说明对象存在，否则说明对象不存在
         */

        // 回调类型（函数对象）
        // Channel 通道里能够获取fd最终发生的具体事件（revents_）
        // 因此可以调用具体事件的回调方法
        ReadOnlyEventCallback readOnlyCallback_;
        EventCallback writeCallback_;
        EventCallback closeCallback_;
        EventCallback errorCallback_;
    };
}
