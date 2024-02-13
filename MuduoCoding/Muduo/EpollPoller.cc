#include "EpollPoller.h"
#include "Log.h"

#include <errno.h>

namespace myMuduo
{
    namespace NetWork
    {   
        // 标识 Channel 在 epoll 的状态
        const int kNew = -1;    // 新的
        const int kAdded = 1;   // 已经存在的
        const int kDeleted = 2; // 已经删除的
        EpollPoller(EventLoop *loop)
        :Poller(loop)
        ,epollfd_(::epoll_create1(EPOLL_CLOEXEC))   // 创建 epoll
        ,events_(kInitEventListSize)                // 设置初始大小：存储发生事件的fd
        {
            if(epollfd_ < 0){
                // epoll 创建失败
                Log_FATAL("create epoll fail : %d\n", errno);
            }
        }
        ~EpollPoller() override; // 要求编译器检查，是否为虚函数（要构成重写）

    }
}