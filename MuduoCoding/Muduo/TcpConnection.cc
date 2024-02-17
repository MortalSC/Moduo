#include "TcpConnection.h"
#include "Log.h"
#include "Socket.h"
#include "Channel.h"
// #include "EventLoop.h"

#include <functional>

namespace myMuduo
{

    static EventLoop *CheckLoopNotNull(EventLoop *loop)
    {
        if (loop == nullptr)
        {
            Log_FATAL("%s:%s:%d TcpConnection loop is Null!\n", __FILE__, __FUNCTION__, __LINE__);
        }
        return loop;
    }
    TcpConnection::TcpConnection(EventLoop *loop,
                                 const std::string &name,
                                 int sockfd, // 来自TcpServer
                                 const InetAddress &localAddr,
                                 const InetAddress &peerAddr)
        : loop_(CheckLoopNotNull(loop)),
          name_(name),
          state_(kConnecting),
          reading_(true),
          socket_(new Socket(sockfd)),
          channel_(new Channel(loop, sockfd)),
          localAddr_(localAddr),
          peerAddr_(peerAddr),
          highWaterMark_(64 * 1024 * 1024) // 64Mb
    {
        // 给设置Channel相应的回调函数
        // 如果Poller给channel通知感兴趣的事件发生了，会调用相应的回调函数
        channel_->setReadOnlyCallback(
            std::bind(&TcpConnection::headleRead, this, std::placeholders::_1));
        channel_->setWriteCallback(
            std::bind(&TcpConnection::headleWrite, this));
        channel_->setCloseCallback(
            std::bind(&TcpConnection::headleClose, this));
        channel_->setErrorCallback(
            std::bind(&TcpConnection::headleError, this));
        Log_INFO("TcpConnection::ctor[%s] at fd=%d\n", name_.c_str(), sockfd);
        // 设置保活
        socket_->setKeepAlive(true);
    }
    TcpConnection::~TcpConnection()
    {
        Log_INFO("TcpConnection::dtor[%s] at fd=%d state=%d\n", name_.c_str(), channel_->fd(), (int)state_);
    }
}