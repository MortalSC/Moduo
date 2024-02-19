#pragma once

/**
 * 提供服务器编程使用的类【对外提供服务器编程】
 */

#include "EventLoop.h"
#include "Acceptor.h"
#include "InetAddress.h"
#include "noncopyable.h"
#include "EventLoopThreadPool.h"
#include "Callbacks.h"
#include "TcpConnection.h"
#include "Buffer.h"

#include <functional>
#include <string>
#include <memory>
#include <atomic>
#include <unordered_map>

namespace myMuduo
{
    class TcpServer : noncopyable
    {
    public:
        using ThreadInitCallback = std::function<void(EventLoop *)>;
        /* 用于标识端口是否可重用 */
        enum Option
        {
            kNoReusePort,
            kReusePort
        };

        TcpServer(EventLoop *loop,
                  const InetAddress &listenAddr,
                  const std::string& name = std::string(),
                  Option option = kNoReusePort);
        ~TcpServer();

        /* 设置线程初始回调 */
        void setThreadInitCallback(const ThreadInitCallback &cb) { threadInitCallback_ = cb; }

        /* 设置新连接回调 */
        void setConnectionCallback(const ConnectionCallback &cb) { connectionCallback_ = cb; }

        /* 设置读写事件发生回调 */
        void setMessageCallback(const MessageCallback &cb) { messageCallback_ = cb; }

        /* 设置消息发送完成回调 */
        void setWriteCompleteCallback(const WriteComleteCallback &cb) { writeComleteCallback_ = cb; }

        /* 设置底层 subLoop/线程 个数 */
        void setThreadNum(int numThreads);

        /* 开启服务器监听：就是开启 mainLoop的Acceptor的listen */
        void start();

    private:
        void newConnection(int sockfd, const InetAddress &peerAddr);
        /* 删除连接：从 ConnectionMap 中删除 */
        void removeConnection(const TcpConnectionPtr &conn);
        /* 在 loop 中删除连接事件 */
        void removeConnectionInLoop(const TcpConnectionPtr &conn);

    private:
        using ConnectionMap = std::unordered_map<std::string, TcpConnectionPtr>;

        EventLoop *loop_;          // 关联 baseLoop
        const std::string ipPort_; // 打包 ip和port
        const std::string name_;

        std::unique_ptr<Acceptor> acceptor_; // 在 mainLoop 中运行的 Acceptor，监听新用户连接事件

        // 用户可以调用setThreadNum设置多个线程
        std::shared_ptr<EventLoopThreadPool> threadPool_; // one loop per thread

        ConnectionCallback connectionCallback_;     // 有新连接的回调
        MessageCallback messageCallback_;           // 有读写消息时的回调
        WriteComleteCallback writeComleteCallback_; // 消息发送完成的回调

        ThreadInitCallback threadInitCallback_; // loop 线程初始化回调

        std::atomic_int started_;

        int nextConnId_;

        ConnectionMap connections_; // 存储所有的连接
    };
}