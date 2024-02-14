#include "EpollPoller.h"
#include "Log.h"
#include "Channel.h"
#include "Poller.h"

#include <errno.h>
#include <unistd.h>
#include <strings.h>

namespace myMuduo
{
    // namespace NetWork
    // {
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

        EpollPoller::~EpollPoller() {
            ::close(epollfd_);
        } 

        // updateChannel => epoll_ctl (ADD/MOD)
        // 调用逻辑
        // Channel（update/remove） => EventLoop（updateChannel/removeChannel） => EpollPoller（updateChannel/removeChannel）
        void EpollPoller::updateChannel(Channel *channel)
        {
            // 获取 channel 对象的存在状态
            const int index = channel->index();
            // 输出信息验证
            Log_INFO("func => %s : fd = %d event = %d index = %d\n", __FUNCTION__, channel->fd(), channel->events(), index);

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
            channels_.erase(fd);

            Log_INFO("func => %s : fd = %d\n", __FUNCTION__, channel->fd());

            // 获取 Channel 中记录的 index 状态
            int index = channel->index();
            // 如果在 epoll 中存在（kAdded）
            if (index == kAdded)
            {
                // 从 epoll 中取出
                update(EPOLL_CTL_DEL, channel);
            }
            channel->set_index(kDeleted);
        }

        // 更新 channel 通道：即：channel 在 epoll 的具体操作（add/mod/del）
        void EpollPoller::update(int operation, Channel *channel)
        {
            // 管理的结构声明
            struct epoll_event event;
            // 初始化空间
            bzero(&event, sizeof(event));
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
                    Log_ERROR("EPOLL_CTL_DEL ERROR:%d\n", errno);
                }
                else
                {
                    // add/mod 失败就是崩溃性问题
                    Log_FATAL("epoll_ctl add/mod errno:%d\n", errno);
                }
            }
        }

        Timestamp EpollPoller::poll(int timeoutMs, ChannelList *activeChannels)
        {
            Log_DEBUG("func => %s :  \n", __FUNCTION__);
            // 我们底层使用的 vector，而函数参数需要的是int数组的起始地址
            // 我们选择使用 起始迭代器解引用获取值，再进行取地址，就可以获取数组的起始地址
            int numEvents = ::epoll_wait(epollfd_, &*(events_.begin()), static_cast<int>(events_.size()), timeoutMs);
            int saveErrno = errno; // 记录下当前 errno（全局），在多线程情形下可能遇到多个同时访问或者涉及修改情形

            Timestamp now(Timestamp::now());
            if (numEvents > 0)
            {
                Log_DEBUG("%d events happened\n", numEvents);
                fillActiveChannels(numEvents, activeChannels);
                if (numEvents == events_.size())
                {
                    // 如果出现就绪事件个数等于容器容量，则需要扩容
                    events_.resize(events_.size() * 2);
                }
            }
            else if (numEvents == 0)
            {
                Log_DEBUG("%s timeout\n", __FUNCTION__);
            }
            else
            {
                // 到此就是：可能发生了外部中断
                if (saveErrno != EINTR)
                {
                    // 发生外部中断
                    errno = saveErrno;
                    Log_ERROR("EpollPoller::poll() error!\n");
                }
            }
            return now;
        }

        // 填写活跃的链接
        void EpollPoller::fillActiveChannels(int numEvents, ChannelList *activeChannels) const
        {
            for(int i = 0; i < numEvents; ++i){
                // events_[i].data.ptr -> void* 
                Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
                // 设置事件发生回应【即：Channel中revent_：关心的是 Poller 的返回】
                channel->set_revents(events_[i].events);
                activeChannels->push_back(channel);
                // Poller向EventLoop反馈，所有发生类的事件列表
                // 【相等于epoll的就绪队列（双端队列）】
           }
        }
    // }
}