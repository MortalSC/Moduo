#include "Poller.h"
#include "Channel.h"



namespace myMuduo
{
    namespace NetWork
    {
        // class Channel;
        Poller::Poller(EventLoop *loop)
            : ownerLoop_(loop) {}

        bool Poller::hasChannel(Channel *channel) const
        {
            // 迭代器
            ChannelMap::const_iterator iter = channels_.find(channel->fd());
            // auto iter = channels_.find(channel->fd());
            // 存在性判断
            return iter != channels_.end() && iter->second == channel;
        }

        // 为什么不在 Poller.cc 中，实现 static Poller *newDefualtPoller(EventLoop *loop); 的定义
        // 原因在于：Poller 作为一个基类，该方法是返回 基类的指针 指向实际的 IO复用 操作的实现方式（epoll / poll）
        // 如果在此处进行定义实现，在必须包含对应的 派生类对象的头文件！
        // 这样就造成了基类引用派生类，这种设计方式不好！
    }   
}