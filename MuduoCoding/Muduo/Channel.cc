#include "Channel.h"
#include "EventLoop.h"
#include "Log.h"

#include <sys/epoll.h>

namespace myMuduo
{
    // 标识 sockfd 的关注状态
    const int Channel::kNoneEvnet = 0;                  // 什么都不关心
    const int Channel::kReadEvent = EPOLLIN | EPOLLPRI; // 关心读事件
    const int Channel::kWriteEvent = EPOLLOUT;          // 关心写事件

    Channel::Channel(EventLoop *loop, int fd)
        : loop_(loop), fd_(fd), events_(0), revents_(0), index_(-1), tied_(false)
    {
    }

    Channel::~Channel() {}

    // 什么时候调用
    void Channel::tie(const std::shared_ptr<void> &ptr)
    {
        tie_ = ptr;
        tied_ = true;
    }
    /**
     * 一旦Channel中的fd的事件（events）改变后
     * update负责在Poller里修改fd相应的事件epoll_ctl
     */
    void Channel::update()
    {
        // 调用 Poller 中的方法
        // loop_->updateChannel(this);
    }

    // 在Channel所属的EventLoop中删除，当前的Channel对象
    void Channel::remove()
    {
        // loop_->removeChannel(this);
    }

    //
    void Channel::handleEvnet(Timestamp receiveTime)
    {
        if (tied_)
        {
            // 提升智能指针 => 作用检验 Channel 对象是否存在！
            std::shared_ptr<void> guard = tie_.lock();
            if (guard)
            {
                headleEventWithGuard(receiveTime);
            }
        }
        else
        {
            /* */
            headleEventWithGuard(receiveTime);
        }
    }

    // 根据Poller监听Channel发生了具体事件，由channel进行调用回调函数
    void Channel::headleEventWithGuard(Timestamp receiveTime)
    {
        Log_INFO("Channel headleEvent revents : %d", revents_);
        // 执行特定的方法
        // 发生异常，执行关闭
        if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN))
        {
            if (closeCallback_)
            {
                closeCallback_();
            }
        }

        // 是否发生错误，是：执行指定行为
        if (revents_ & EPOLLERR)
        {
            if (errorCallback_)
                errorCallback_();
        }

        // 是否发生可读事件
        if (revents_ & (EPOLLIN | EPOLLPRI))
        {
            if (readOnlyCallback_)
                readOnlyCallback_(receiveTime);
        }

        // 是否发生可写事件
        if (revents_ & EPOLLOUT)
        {
            if (writeCallback_)
                writeCallback_();
        }
    }
}