#pragma once

/**
 * 封装 socket 所需的 ip / port
 */

#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
namespace myMuduo
{
    // namespace NetWork
    // {
    class InetAddress
    {
    public:
        // 构造函数
        explicit InetAddress(uint16_t port, std::string ip = "127.0.0.1");
        explicit InetAddress(const sockaddr_in &addr) : addr_(addr) {}

        // 其他接口：获取 ip / port
        // 获取 ip
        std::string toIp() const;
        // 获取 ip 和 端口号
        std::string toIpPort() const;
        // 获取端口号
        uint16_t toPort() const;
        // 获取对象
        const sockaddr_in *getSockAddr() const
        {
            return &addr_;
        }

        // 外部设置 addr 内容
        void setSockAddr(cosnt sockaddr_in& addr){addr_ = addr;}
    private:
        struct sockaddr_in addr_;
    };
    // }
}