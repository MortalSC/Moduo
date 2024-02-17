#include "TcpServer.h"
#include "Log.h"

#include <functional>

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
    TcpServer::~TcpServer() {}

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

    void TcpServer::newConnection(int sockfd, const InetAddress &peerAddr){

    }

    /* 删除连接：从 ConnectionMap 中删除 */
    void TcpServer::removeConnection(const TcpConnectionPtr &conn){

    }
    /* 在 loop 中删除连接事件 */
    void TcpServer::removeConnectionInLoop(const TcpConnectionPtr &conn){
        
    }

}