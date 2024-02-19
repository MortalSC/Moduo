#include "Socket.h"
#include "Log.h"
#include "InetAddress.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <netinet/tcp.h>


namespace myMuduo
{
    Socket::~Socket()
    {
        close(sockfd_);
    }

    // 绑定本地 ip/port
    void Socket::bindAddress(const InetAddress &localaddr)
    {
        // 绑定
        if (0 != ::bind(sockfd_, (sockaddr *)localaddr.getSockAddr(), sizeof(sockaddr_in)))
        {
            Log_FATAL("bind sockfd : %d fail\n", sockfd_);
        }
    }

    // 启动监听
    void Socket::listen()
    {
        if (0 != ::listen(sockfd_, 1024))
        {
            Log_FATAL("listen sockfd : %d fail\n", sockfd_);
        }
    }

    // 绑定接收
    int Socket::accept(InetAddress *peeraddr)
    {
        sockaddr_in addr;
        socklen_t len = sizeof(addr);   // 必须初始化
        bzero(&addr, sizeof(addr));
        // 设置文件描述符为非阻塞：SOCK_CLOEXEC | SOCK_NONBLOCK
        int connfd = ::accept4(sockfd_, (sockaddr *)&addr, &len, SOCK_CLOEXEC | SOCK_NONBLOCK);
        if (connfd >= 0)
        {
            peeraddr->setSockAddr(addr);
        }
        return connfd;
    }

    // 关闭写入功能
    void Socket::shutdownWrite()
    {
        if (shutdown(sockfd_, SHUT_WR) < 0)
        {
            Log_ERROR("shutdownWrite sockfd %d fail\n", sockfd_);
        }
    }

    // 设置 Tcp 无延时（不进行 Tcp 数据缓冲）
    void Socket::setTcpNoDelay(bool on)
    {
        int optval = on ? 1 : 0;
        ::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval));
    }

    //
    void Socket::setReuseAddr(bool on){
int optval = on ? 1 : 0;
        ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    }

    //
    void Socket::setReusePort(bool on){
int optval = on ? 1 : 0;
        ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    }
    // 设置保持连接
    void Socket::setKeepAlive(bool on){
int optval = on ? 1 : 0;
        ::setsockopt(sockfd_, IPPROTO_TCP, SO_KEEPALIVE, &optval, sizeof(optval));
    }
}