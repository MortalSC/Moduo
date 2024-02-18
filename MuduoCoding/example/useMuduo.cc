/* 基于本项目源码开发使用实例 */
/* 前置要求：参考 README.md 文档关于配置使用说明，完成相关操作 */

/* 引入头文件 */
#include <myMuduo/TcpServer.h> // 网络库头文件【以包含基本需要的全部头文件】
#include <myMuduo/Log.h>       // 日志

#include <string>
using namespace myMuduo;
class Server
{
public:
    Server(EventLoop *loop,
           const InetAddress &addr,
           const std::string &name)
        : server_(loop, addr, name),
          loop_(loop)
    {
        // 注册回调函数
        server_.setConnectionCallback(
            std::bind(&Server::onConnection, this, std::placeholders::_1));

        server_.setMessageCallback(
            std::bind(&Server::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

        // 设置合适的 loop 线程数量
        server_.setThreadNum(3); // 设置 3 个 subLoop 执行任务处理【总共就是4个（+1个mainLoop）】
    }


~Server(){

}
    void start()
    {
        server_.start();
    }

private:
    // 连接建立或者断开的回调
    void onConnection(const TcpConnectionPtr &conn)
    {
        if (conn->connected())
        {
            Log_INFO("Connection up: %s", conn->peerAddress().toIpPort().c_str());
        }
        else
        {
            Log_INFO("Connection down: %s", conn->peerAddress().toIpPort().c_str());
        }
    }

    // 可读写事件回调
    void onMessage(const TcpConnectionPtr &conn,
                   Buffer *buf,
                   Timestamp time)
    {
        
        std::string msg = buf->retrieveAllAsString(); // 数据转成 string 类型返回
        conn->send(msg);
        conn->shutdown(); // 关闭写端
    }

private:
    EventLoop *loop_;
    TcpServer server_;
};

int main()
{
    std::cout << "1" << std::endl;
        std::cout << "测试此处错误！" << std::endl;

    EventLoop loop;
    std::cout << "2" << std::endl;

    // 绑定 ip/端口【ip默认为本地环回】
    InetAddress addr(8000);
    std::cout << "3" << std::endl;

    Server server(&loop, addr, "测试使用本项目Muduo库"); // 启动Acceptor（创建了一个nonblocking fd）创建绑定
    std::cout << "4" << std::endl;

    server.start(); // 启用监听
    std::cout << "5" << std::endl;

    loop.loop(); // 启动mainLoop，就是底层的Poller[epoll]

    return 0;
}