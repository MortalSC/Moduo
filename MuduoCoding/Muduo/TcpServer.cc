#include "TcpServer.h"
#include "Log.h"
#include "TcpConnection.h"

#include <functional>
#include <strings.h>

namespace myMuduo
{
    static EventLoop *CheckLoopNotNull(EventLoop *loop)
    {
        if (loop == nullptr)
        {
            Log_FATAL("%s:%s:%d mainLoop is Null!\n", __FILE__, __FUNCTION__, __LINE__);
        }
        return loop;
    }
    TcpServer::TcpServer(EventLoop *loop,
                         const InetAddress &listenAddr,
                         const std::string &name,
                         Option option)
        : loop_(CheckLoopNotNull(loop)), // 给定的 loop 不能为空！！！
          ipPort_(listenAddr.toIpPort()),
          name_(name),
          acceptor_(new Acceptor(loop, listenAddr, option == kReusePort)), // mainLoop 上执行 Acceptor[创建fd、listen ...]
          threadPool_(new EventLoopThreadPool(loop, name)),                // 创建线程池对象，默认只有主线程
          connectionCallback_(),
          messageCallback_(),
          nextConnId_(1)
    {
        // 当有新用户连接时，会执行 TcpServer::newConnection 新连接回调
        // 在代码上就是在 Acceptor::headleRead() 中去执行
        acceptor_->setNewConnectionCallback(std::bind(&TcpServer::newConnection, this,
                                                      std::placeholders::_1, std::placeholders::_2));
    }
    TcpServer::~TcpServer()
    {
        for (auto &item : connections_)
        {
            // second 就是 TcpConnection
            // 使用局部智能指针对象指向 TcpConnection
            // 如果直接去除原来的强智能指针，就没法在后面调用销毁连接操作了
            TcpConnectionPtr conn(item.second);
            // 删除智能指针：有智能指针时，无法释放资源
            item.second.reset();
            conn->getloop()->runInLoop(
                std::bind(&TcpConnection::connectDistroyed, conn));
        }
    }

    /* 设置底层 subLoop/线程 个数 */
    void TcpServer::setThreadNum(int numThreads)
    {
        // 调用底层设置线程数量
        // 之后，若调用 EventLoopThreadPool::start 就可以创建指定个数的线程
        // 并加入 线程管理数组threads_ / loop 管理数组
        threadPool_->setThreadNum(numThreads);
    }

    /* 开启服务器监听：就是开启 mainLoop的Acceptor的listen */
    void TcpServer::start()
    {

        if (started_++ == 0)
        { // 防止一个TcpServer对象被多次启动
            // 启动线程池对象管理线程
            // 会根据指定的线程个数创建线程并放入 threads_ & subLoop_ 进行管理
            threadPool_->start(threadInitCallback_);
            // 当前 mainLoop 通过调用 runInLoop
            /**
             * 1. 如果是 mainLoop 中执行，就会调用 Acceptor::listen
             *  => 启动新用户连接监听
             *  => 把监听到的新用户连接包装注册到 Poller，指定监听的事件行为
             */
            loop_->runInLoop(std::bind(&Acceptor::listen, acceptor_.get()));
        }
    }

    // 有一个新连接产生，就会执行Acceptor::headlRead回调
    void TcpServer::newConnection(int sockfd, const InetAddress &peerAddr)
    {
        // 使用轮询算法获取一个subLoop
        EventLoop *ioLoop = threadPool_->getNextLoop();

        // 组装新连接名字
        char buf[64] = {0};
        snprintf(buf, sizeof(buf), "-%s#%d", ipPort_.c_str(), nextConnId_);
        ++nextConnId_; // 只在 MainLoop 中执行，就是只有一个线程能执行，不涉及线程安全问题
        std::string connName = name_ + buf;

        Log_INFO("TcpServer::newConnection [%s] - new connection [%s] from %s \n",
                 name_.c_str(), connName.c_str(), peerAddr.toIpPort().c_str());

        // 通过sockfd获取其他绑定本机的ip/port
        sockaddr_in local;
        bzero(&local, sizeof(local));
        socklen_t addrLen = sizeof(local);
        if (::getsockname(sockfd, (sockaddr *)&local, &addrLen) < 0)
        {
            Log_ERROR("sockets::getLocalAddr\n");
        }
        InetAddress localAddr(local);

        // 根据连接成功的sockfd，创建TcpConnection连接对象
        TcpConnectionPtr conn(
            new TcpConnection(
                ioLoop, connName, sockfd, // Socket Channel
                localAddr, peerAddr));

        // 存连接
        connections_[connName] = conn;

        // 下面的回调是用户设置的
        // 关联逻辑
        // TcpServer => TcpConnection => Channel => Poller => notify channel 调用回调
        conn->setConnectionCallback(connectionCallback_);
        conn->setMessageCallback(messageCallback_);
        conn->setWriteCompleteCallback(writeComleteCallback_);
        // 设置如何关闭连接的回调
        conn->setCloseCallback(
            std::bind(&TcpServer::removeConnection, this, std::placeholders::_1));

        // 直接调用 TcpConnection::connectEstablished
        // 使 TcpConnection 对象连接状态改变 kConnecting => kConnected
        ioLoop->runInLoop(std::bind(&TcpConnection::connectEstablished, conn));
    }

    /* 删除连接：从 ConnectionMap 中删除 */
    void TcpServer::removeConnection(const TcpConnectionPtr &conn)
    {
        loop_->runInLoop(
            std::bind(&TcpServer::removeConnectionInLoop, this, conn));
    }
    /* 在 loop 中删除连接事件 */
    void TcpServer::removeConnectionInLoop(const TcpConnectionPtr &conn)
    {
        Log_INFO("TcpServer::removeConnectionInLoop [%s] - connection %s \n",
                 name_.c_str(), conn->name().c_str());

        // 在管理map中删除对应连接
        connections_.erase(conn->name());

        // 获取 TcpConnection 所在的 Loop
        EventLoop *ioLoop = conn->getloop();
        ioLoop->queueInLoop(
            std::bind(&TcpConnection::connectDistroyed, conn));
    }

}