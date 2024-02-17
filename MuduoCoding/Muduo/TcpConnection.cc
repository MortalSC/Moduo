#include "TcpConnection.h"
#include "Log.h"
#include "Socket.h"
#include "Channel.h"
#include "EventLoop.h"

#include <functional>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <netinet/tcp.h>

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

    void TcpConnection::headleRead(Timestamp receiveTime)
    {
        int savedError = 0;
        ssize_t n = inputBuffer_.readFd(channel_->fd(), &savedError);
        if (n > 0)
        {
            // 有数据，读到了缓冲区
            // 已建立连接的用户，有可读事件发生了
            // 调用用户传入的onMessage
            // std::shared_from_this()：获取当前对象的智能指针
            messageCallback_(shared_from_this(), &inputBuffer_, receiveTime);
        }
        else if (n == 0)
        {
            headleClose();
        }
        else
        {
            errno = savedError;
            Log_ERROR("TcpConnection::headleRead.\n");
            headleError();
        }
    }
    void TcpConnection::headleWrite()
    {
        // 优先判断是否可写
        if (channel_->isWriting())
        {
            int savedError = 0;
            ssize_t n = outputBuffer_.writeFd(channel_->fd(), &savedError);
            if (n > 0)
            {
                // 写入成功
                outputBuffer_.retrieve(n);
                if (outputBuffer_.readableBytes() == 0)
                {
                    // 数据写完，关闭可写
                    channel_->disableWriting();
                    if (writeComleteCallback_)
                    {
                        //  唤醒loop_对应的线程执行回调
                        loop_->queueInLoop(
                            std::bind(writeComleteCallback_, shared_from_this()));
                    }
                    if (state_ == kDisconnecting)
                    {
                        shutdownInLoop();
                    }
                }
            }
            else
            {
                Log_ERROR("TcpConnection::headleWrite.\n");
            }
        }
        else
        {
            // 不可写情形
            Log_ERROR("TcpConnection fd=%d is down, no more writing.\n", channel_->fd());
        }
    }
    // 处于已连接 / 正在断开的才有close
    void TcpConnection::headleClose()
    {
        Log_INFO("TcpConnection::headleClose fd=%d state=%d \n", channel_->fd(), (int)state_);
        setState(kDisconnected);
        channel_->disableAll();
        TcpConnectionPtr connPtr(shared_from_this());
        connectionCallback_(connPtr); // 执行连接关闭的回调
        closeCallback_(connPtr);      // 关闭连接回调
    }
    void TcpConnection::headleError()
    {
        int optval;
        socklen_t optlen = sizeof(optval);
        int err = 0;
        if (::getsockopt(channel_->fd(), SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0)
        {
            err = errno;
        }
        else
        {
            err = optval;
        }
        Log_ERROR("TcpConnection::headleError name:%s - SO_ERROR:%d\n", name_.c_str(), err);
    }

    // 发送数据
    void TcpConnection::send(const void *message, int len)
    {
    }
    void TcpConnection::send(const std::string &buf)
    {
        // 连接状态判断
        if (state_ == kConnected)
        {
            if (loop_->isInLoopThread())
            {
                sendInLoop(buf.c_str(), buf.size());
            }
            else
            {
                loop_->runInLoop(
                    std::bind(
                        &TcpConnection::sendInLoop,
                        this,
                        buf.c_str(),
                        buf.size()));
            }
        }
    }

    // 发送数据
    /**
     * 发送数据时，应用程序写的快
     * 而内核发送数据慢，此时需要把数据写入发送缓冲区中，并且设置水位回调
     */
    void TcpConnection::sendInLoop(const void *data, size_t len)
    {
        ssize_t nwrote = 0;      // 用于记录一次写入（发送）的数据量
        size_t remaining = len;  // 记录待发送数据的全部数据量大小
        bool faultError = false; //

        // 判断连接状态【之前可能调用了shutdown】
        if (state_ == kDisconnected)
        {
            Log_ERROR("disconnected, give up writing.\n");
            return;
        }

        // Channel 第一次开始写数据，而且缓冲区没有待发送的数据
        if (!channel_->isWriting() && outputBuffer_.readableBytes() == 0)
        {
            nwrote = ::write(channel_->fd(), data, len);
            if (nwrote >= 0)
            {
                // 发送成功
                // 更新待发送的数据量
                remaining = len - nwrote; // 单次写的数据量可能大于单次可写数据量

                if (remaining == 0 && writeComleteCallback_)
                {
                    // 数据写完，且存在写回调
                    // 数据写完了，就不会再写了，那么就没必要关注 epollout 事件了
                    loop_->queueInLoop(std::bind(
                        &TcpConnection::writeComleteCallback_,
                        shared_from_this()));
                }
            }
            else // 可能出错了
            {
                nwrote = 0;
                // 判断错误情形
                if (errno != EWOULDBLOCK)
                {
                    Log_ERROR("TcpConnection::sendInLoop ERROR \n");
                    if (errno == EPIPE || errno == ECONNRESET)
                    {
                        // 对端 socket 重置了
                        faultError = true;
                    }
                }
            }
        }

        if (!faultError && remaining > 0)
        {
            // 说明这一次没有发送完数据
            // 需要把剩余数据保存到缓冲区中，然后给channel注册epollout事件
            // Poller发现Tcp发送缓冲区有空间，会通知相应的socket->channel调用writeCallback回调方法
            // 最终继续调用T产品Connection::headleWrite方法把发送缓冲区中的数据发送完毕

            // 当前缓冲区剩余的待发送数据量
            size_t oldLen = outputBuffer_.readableBytes();
            if (oldLen + remaining >= highWaterMark_ && oldLen < highWaterMark_ && highWaterMarkCallback_)
            {
                loop_->queueInLoop(
                    std::bind(highWaterMarkCallback_,
                              shared_from_this(),
                              oldLen-remaining));
            }
            // 剩余数据写回发送缓冲区
            outputBuffer_.append((char *)data + nwrote, remaining);

            if (!channel_->isWriting())
            {
                channel_->enableReading(); // 一定要注册channel的写事件，否则poll不会给channel通知epollout
            }
        }
    }

    // 断开连接
    void TcpConnection::shutdown()
    {
    }

    void TcpConnection::shutdownInLoop(){

    }
}