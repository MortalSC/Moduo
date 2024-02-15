#pragma once

/**
 * 封装 socket fd 操作
 */

#include "noncopyable.h"

namespace myMuduo
{
    class InetAddress;
    class Socket : noncopyable
    {
    public:
        explicit Socket(int sockfd)
            : sockfd_(sockfd) {}
        ~Socket();

        int fd() const { return sockfd_; }
        // 绑定本地 ip/port
        void bindAddress(cosnt InetAddress& localaddr);
        // 启动监听
        void listen();
        // 绑定接收
        int accept(InetAddress* peeraddr);
        // 关闭写入功能
        void shutdownWrite();
        // 设置 Tcp 无延时（不进行 Tcp 数据缓冲）
        void setTcpNoDelay(bool on);
        // 
        void setReuseAddr(bool on);
        // 
        void setReusePort(bool on);
        // 设置保持连接
        void setKeepAlive(bool on);

    private:
        const int sockfd_;
    };
}