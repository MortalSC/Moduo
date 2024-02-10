#include "InetAddress.h"

#include <strings.h>
#include <string.h>

namespace NetWork
{

    // 构造函数
    InetAddress::InetAddress(uint16_t port, std::string ip)
    {
        // 内存初始化（清零）
        bzero(&addr_, sizeof(addr_));
        // 常规操作：设置协议信息
        addr_.sin_family = AF_INET;// IPv4
        addr_.sin_port = htons(port);// 端口号
        addr_.sin_addr.s_addr = inet_addr(ip.c_str());// 设置ip
    }

    // 其他接口：获取 ip / port
    // 获取 ip
    std::string InetAddress::toIp() const{
        // 注意：ip 此时是网络字节序！
        char buf[64] = {0};
        ::inet_ntop(AF_INET, &addr_.sin_addr, buf, sizeof(buf));
        return buf;
    }
    // 获取 ip 和 端口号
    std::string InetAddress::toIpPort() const{
        // 格式：ip:port
                char buf[64] = {0};
        ::inet_ntop(AF_INET, &addr_.sin_addr, buf, sizeof(buf));
        size_t end = strlen(buf);
        uint16_t port = ntohs(addr_.sin_port);
        sprintf(buf+end, ":%u", port);
        return buf;
    }
    // 获取端口号
    uint16_t InetAddress::toPort() const{
        return ntohs(addr_.sin_port);
    }
};
