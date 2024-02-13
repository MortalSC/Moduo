#pragma once

/**
 *  继承自 Poller，实现具体的 epoll 多路复用
 */

#include "Poller.h"

#include <vector>
#include <sys/epoll.h>

namespace myMuduo
{
    namespace NetWork
    {
        class Channel;
        class EpollPoller : public Poller
        {
        public:
            EpollPoller(EventLoop *loop);
            ~EpollPoller() override; // 要求编译器检查，是否为虚函数（要构成重写）

            // 重写 基类 方法
            // poll => epoll_wait
            Timestamp poll(int timeoutMs, ChannelList *activeChannels) override;
            // updateChannel => epoll_ctl (ADD/MOD)
            void updateChannel(Channel *channel) override;
            // removeChannel => epoll_ctl (DEL)
            void removeChannel(Channel *channel) override;

        private:
            // 设置 EventList 的默认初始长度
            static const int kInitEventListSize = 16;
            // 填写活跃的链接
            void fillActiveChannels(int numEvents, ChannelList *activeChannels) const;
            // 更新 channel 通道
            void update(int operation, Channel *channel);
            // epoll_event：记录事件和文件描述符（关注的）
            using EventList = std::vector<epoll_event>;

            int epollfd_; // epoll_create 返回的句柄
            EventList events_;
        };
    }
}