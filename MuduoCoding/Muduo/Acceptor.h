#pragma once

/**
 * Acceptor是运行在MainReactor上的，即在 baseLoop 中
 */

#include "noncopyable.h"
#include "Socket.h"
#include "Channel.h"

#include <functional>

namespace myMuduo
{
    class EventLoop;
    class InetAddress;
    class Acceptor : noncopyable
    {
    public:
        using NewConnectionCallback = std::function<void(int sockfd, const InetAddress &)>;

        Acceptor(EventLoop *loop, const InetAddress &listenAddr, bool reuseport);
        ~Acceptor();
        void setNewConnectionCallback(const NewConnectionCallback &cb)
        {
            newConnectionCallback_ = cb;
        }

        bool listening() const { return listening_; }
        void listen();

    private:
        void headleRead();

        EventLoop *loop_; // 就是指向 baseLoop
        Socket acceptSocket_;
        Channel acceptChannel_;
        NewConnectionCallback newConnectionCallback_;
        bool listening_;
    };
}