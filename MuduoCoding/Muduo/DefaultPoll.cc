#include "Poller.h"
#include "EpollPoller.h"

#include <stdlib.h>

/**
 * 实现 Poller 中，返回具体 IO 复用的实现形式对象
 * 在该文件中实现 static Poller *newDefualtPoller(EventLoop *loop); 的意义：
 * => 防止出现在基类中去引用派生类造成的头文件相互包含的不好设计方案
 */

namespace myMuduo
{
    // namespace NetWork
    // {
        Poller *Poller::newDefualtPoller(EventLoop *loop){
            // 默认的 IO复用的实例设定
            // 通过一个环境变量来选择
            if(::getenv("MUDUO_USE_POLL")){
                // 如果环境变量中存在：MUDUO_USE_POLL
                // 就使用 poll
                return nullptr; // 生成 poll 的实例（模拟项目中仅实现：epoll）
            }else{
                // 否则使用 epoll
                return new EpollPoller(loop); // 生成 epoll 的实例
            }
        }
    // }
}