#pragma once
/**
 * 与 TcpConnection 管理的回调类型定义
*/

#include <memory>
#include <functional>

namespace myMuduo{
    /* 前置类型 */
    class Buffer;
    class TcpConnection;
    class Timestamp;
    
    /* 指向 TcpConnection 的智能指针 */
    using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
    /* 关联 对端信息的回调函数 获取 */
    using ConnectionCallback = std::function<void (const TcpConnectionPtr&)>;
    /* 关闭 */
    using CloseCallback = std::function<void (const TcpConnectionPtr&)>;
    /**/
    using WriteComleteCallback = std::function<void (const TcpConnectionPtr&)>;
    /**/
    using MessageCallback = std::function<void (const TcpConnectionPtr&, Buffer*,Timestamp)>;
    /* 水位回调 */
    using HighWaterMarkCallback = std::function<void (const TcpConnectionPtr&, size_t)>;
}