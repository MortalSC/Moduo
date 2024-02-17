#pragma once

#include "noncopyable.h"
#include "InetAddress.h"
#include "Callbacks.h"
#include "Buffer.h"
#include "Timestamp.h"

#include <memory>
#include <string>
#include <atomic>

namespace myMuduo
{
    class Channel;
    class EventLoop;
    class Socket;
    /**
     * TcpConnection：是在 SubLoop 中运行的实体
     * 工作逻辑：
     *  => TcpServer.Acceptor：有新用户连接，通过accept函数拿到connfd
     *  => TcpConnection 设置回调；封装Channel；注册到Poller；监听Channel的回调操作
     */
    class TcpConnection
        : noncopyable,
          public std::enable_shared_from_this<TcpConnection>
    {
    public:
        TcpConnection(EventLoop *loop,
                      const std::string &name,
                      int sockfd, // 来自TcpServer
                      const InetAddress &localAddr,
                      const InetAddress &peerAddr);
        ~TcpConnection();

        EventLoop *getloop() const { return loop_; }
        const std::string &name() const { return name_; }
        const InetAddress &localAddress() const { return localAddr_; }
        const InetAddress &peerAddress() const { return peerAddr_; }

        bool connected() const { return state_ == kConnected; }

        // 发送数据
        void send(const void *message, int len);
        // 断开连接
        void shutdown();

        void setConnectionCallback(const ConnectionCallback &cb)
        {
            connectionCallback_ = cb;
        }

        void setMessageCallback(const MessageCallback &cb)
        {
            messageCallback_ = cb;
        }

        void setWriteCompleteCallback(const WriteComleteCallback &cb)
        {
            writeComleteCallback_ = cb;
        }

        void setHighWaterMarkCallback(const HighWaterMarkCallback &cb)
        {
            highWaterMarkCallback_ = cb;
        }

        void setCloseCallback(const CloseCallback &cb)
        {
            closeCallback_ = cb;
        }

        // 建立连接
        void connectEstablished();
        // 连接销毁
        void connectDistroyed();


    private:
        void headleRead(Timestamp receiveTime);
        void headleWrite();
        void headleClose();
        void headleError();

        void send(const std::string& buf);

        void sendInLoop(const void *message, size_t len);
        void shutdownInLoop();

    private:
        EventLoop *loop_; // 不是 baseLoop，是 subLoop
        const std::string name_;
        enum StateE
        {                   // 表示连接状态
            kDisconnected, // 断开
            kConnecting,    // 正在连接
            kConnected,     // 已连接
            kDisconnecting  // 正在断开
        };
        void setState(StateE state){state_ = state;}

        std::atomic_int state_; // 状态标识
        bool reading_;

        std::unique_ptr<Socket> socket_;
        std::unique_ptr<Channel> channel_;

        const InetAddress localAddr_; // 记录当前主机
        const InetAddress peerAddr_;  // 记录对端

        ConnectionCallback connectionCallback_;     // 有新连接的回调
        MessageCallback messageCallback_;           // 有读写消息时的回调
        WriteComleteCallback writeComleteCallback_; // 消息发送完成的回调
        HighWaterMarkCallback highWaterMarkCallback_;
        CloseCallback closeCallback_;

        size_t highWaterMark_; // 水位值

        Buffer inputBuffer_;    // 接收缓冲区
        Buffer outputBuffer_;   // 发送缓冲区
    };
}