#include "Acceptor.h"
#include "Log.h"
#include "InetAddress.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>

namespace myMuduo
{
    /*创建非阻塞的sockfd*/
    static int createNonblocking(){
        /**
         * SOCK_STREAM：表明数据是字节流
         * SOCK_NONBLOCK：表明sockfd是非阻塞的
         * SOCK_CLOEXEC：表明从进程继承而来的fd在该进程中都设置为默认关闭
        */
        int sockfd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
        if(sockfd<0){
            Log_FATAL("%s:%s:%d listen socket create errno : %d \n", __FILE__, __FUNCTION__, __LINE__, errno);
        }
    }
    Acceptor::Acceptor(EventLoop *loop, const InetAddress &listenAddr, bool reuseport)
    :loop_(loop),
    acceptSocket_(createNonblocking()),     // 完成创建套接字
    acceptChannel_(loop, acceptSocket_.fd()),
    listening_(false)
    {
        acceptSocket_.setReuseAddr(true);
        acceptSocket_.setReusePort(true);
        acceptSocket_.bindAddress(listenAddr);  // 绑定套接字

        // 调用 TcpServer start() 启动 Acceptor.listen
        // 如果有新用户的连接，执行回调
        /**
         * 1. 把 fd 打包成 Channel
         * 2. 把 Channel 分配给 subLoop 去处理
        */
       // 绑定回调
       acceptChannel_.setReadOnlyCallback(std::bind(&Acceptor::headleRead, this));
    }
    
    
    Acceptor::~Acceptor(){
        // 释放Channel中关注的所有内容
        acceptChannel_.disableAll();
        // 释放本身
        acceptChannel_.remove();
    }

    void Acceptor::listen(){
        listening_ = true;  // 设置监听状态
        // 启动监听
        acceptSocket_.listen();
        // Channel对象启用可读关注是否有事件发生
        acceptChannel_.enableReading();
    }

    // listenfd有事件发生了，就是有新用户连接了【由baseLoop中Poller反馈而知】
    void Acceptor::headleRead(){
        // 获取客户端连接信息
        InetAddress peerAddr;
        // 获取连接的fd和填充客户端ip/port信息
        int connfd = acceptSocket_.accept(&peerAddr);
        if(connfd >= 0){
            // 执行连接回调
            if(newConnectionCallback_){
                // 轮询找到可用的subLoop，分发当前的Channel待处理对象，去处理
                newConnectionCallback_(connfd, peerAddr);
            }else{
                // 有链接，无回调 => 关闭连接
                ::close(connfd);
            }
        }else{
            // 出错
            Log_ERROR("%s:%s:%d accept create errno : %d \n", __FILE__, __FUNCTION__, __LINE__, errno);
            // 分析情形
            // 文件描述符资源上限
            if(errno == EMFILE){
            Log_ERROR("%s:%s:%d sockfd reached limit\n", __FILE__, __FUNCTION__, __LINE__);
            }
        }
    }
}