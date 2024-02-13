#pragma once

/**
 * Muduo库中，多路复用事件分发器的核心IO复用模块
 * => 原版中：会被 Epoll 和 Poll 继承，即支持两种多路复用方式
 * => 模拟实现版本中，目前仅实现 Epoll 的支持
 */

#include "noncopyable.h"
#include "Timestamp.h"

#include <vector>
#include <unordered_map>

namespace myMuduo
{
    class Channel;

    namespace NetWork
    {
        class EventLoop;
        class Poller : noncopyable
        {
        public:
            using ChannelList = std::vector<Channel *>;

            // 构造与析构
            Poller(EventLoop *loop);
            virtual ~Poller() = default;

            // 纯虚函数 => 统一接口
            // activeChannels：处于激活态的 Channel（需要被关注的）
            // 相等于 epoll_wait
            virtual Timestamp poll(int timeoutMs, ChannelList *activeChannels) = 0; // 相等于 epoll_wait
            // 相等于 epoll_ctl：ADD / MOD
            virtual void updateChannel(Channel *channel) = 0;
            // 相等于：epoll_ctl：DEL
            virtual void removeChannel(Channel *channel) = 0;
            // 判断channel是否在当前Poller中
            bool hasChannel(Channel *channel) const;
            // 提供给EventLoop获取Poller默认的IO复用方式（模拟的项目中就只有epoll）
            static Poller *newDefualtPoller(EventLoop *loop);

        protected:
            /**
             * int：表示的就是 sockfd
             * channel：表示 sockfd 对应的通道实体
             * 实现：通过 fd 快速查找 对应的 channel
             */
            using ChannelMap = std::unordered_map<int, Channel *>;
            ChannelMap channels_;

        private:
            EventLoop *ownerLoop_;
        };
    }
}