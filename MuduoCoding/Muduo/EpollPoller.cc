#include "EpollPoller.h"
#include "Log.h"
#include "Channel.h"

#include <errno.h>

namespace myMuduo
{
    namespace NetWork
    {
        // 标识 Channel 在 epoll 的状态
        const int kNew = -1;    // 新的
        const int kAdded = 1;   // 已经存在的
        const int kDeleted = 2; // 已经删除的
        // 注：channel中的index_与此处的kXXX对应！
        // index_：在Channel类中标识，channel在epoll中的状态（新的/已添加/已删除）
        EpollPoller::EpollPoller(EventLoop *loop)
            : Poller(loop), epollfd_(::epoll_create1(EPOLL_CLOEXEC)) // 创建 epoll
              ,
              events_(kInitEventListSize) // 设置初始大小：存储发生事件的fd
        {
            if (epollfd_ < 0)
            {
                // epoll 创建失败
                Log_FATAL("create epoll fail : %d\n", errno);
            }
        }
        EpollPoller::~EpollPoller() override; // 要求编译器检查，是否为虚函数（要构成重写）

        // 填写活跃的链接
        void EpollPoller::fillActiveChannels(int numEvents, ChannelList *activeChannels) const
        {
        }
        // 更新 channel 通道：即：channel 在 epoll 的具体操作（add/mod/del）
        void EpollPoller::update(int operation, Channel *channel)
        {
            // 管理的结构声明
            struct epoll_event event;
            // 初始化空间
            memset(&event, sizeof(event));
            // 获取channel对象关注的fd
            int fd = channel->fd();
            // 结构内成员赋值，由于我们使用的是Channel*，故对应 epoll_data_t 中的指针类型
            event.data.ptr = channel;
            event.events = channel->events();
            event.data.fd = fd;

            if (::epoll_ctl(epollfd_, operation, fd, &event) < 0)
            {
                // 注：ctl 操作有三种，其中 del 失败关系不大
                if (operation == EPOLL_CTL_DEL)
                {
                    LOG_ERROR("EPOLL_CTL_DEL ERROR:%d\n", errno);
                }
                else
                {
                    // add/mod 失败就是崩溃性问题
                    Log_FATAL("epoll_ctl add/mod errno:%d\n", errno);
                }
            }
        }

        // updateChannel => epoll_ctl (ADD/MOD)
        // 调用逻辑
        // Channel（update/remove） => EventLoop（updateChannel/removeChannel） => EpollPoller（updateChannel/removeChannel）
        void EpollPoller::updateChannel(Channel *channel)
        {
            // 获取 channel 对象的存在状态
            const int index = channel->index();
            // 输出信息验证
            Log_INFO("fd = %d event = %d index = %d\n", channel->fd(), channel->events(), index);

            // 分类讨论：
            // channel对象是新的
            if (index == kNew || index == kDeleted)
            {
                int fd = channel->fd();
                if (index == kNew)
                {
                    // 如果是新的，应该加入到 Poller channelMap（管理channel对象） 中
                    // 获取文件描述符（作为channelMap的key）
                    channels_[fd] = channel;
                }
                // 更新 channel index 的值（标记状态）
                channel->set_index(kAdded);
                // 给指定 channel 关注的文件描述符注册到epoll中
                update(EPOLL_CTL_ADD, channel);
            }
            else
            {
                // 说明 channel 曾经在 Poller 上注册过了，在 channelMap（管理channel对象）
                // 检查是否有需要关注的行为（...）
                if (channel->isNoneEvent())
                {
                    // 无关注的行为，没有就从 epoll 中删除
                    update(EPOLL_CTL_DEL, channel);
                    channel->set_index(kDeleted);
                }
                else
                {
                    // 关注的事件行为发生改变，更新
                    update(EPOLL_CTL_MOD, channel);
                    channel->set_index(kAdded);
                }
            }
        }
        // removeChannel => epoll_ctl (DEL)
        // 从 Poller 中删除 Channel
        // 注：该操作是在 EventLoop Channel channelMap中删除【在EventLoop中的channelList仍然存在】
        // 如果 该channel 对象在 epoll 关注的红黑树中，则也要删除
        void EpollPoller::removeChannel(Channel *channel)
        {
            // 获取关注的文件描述符
            int fd = channel->fd();
            // 从 channelMap 中删除
            channels_->erase(fd);

            // 获取 Channel 中记录的 index 状态
            int index = channel->index();
            // 如果在 epoll 中存在（kAdded）
            if(index == kAdded){
                // 从 epoll 中取出
                update(EPOLL_CTL_DEL,channel);

            }
            channel->set_index(kDeleted);
        }

    }
}