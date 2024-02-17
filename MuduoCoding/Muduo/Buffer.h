#pragma once

/**
 * 与 TcpConnection 相关的 Buffer 缓冲区
 */

#include <vector>
#include <string>
#include <algorithm>

namespace myMuduo
{
    class Buffer
    {
    public:
        static const size_t kCheapPrepend = 8;   // 8 字节大小用于记录数据包的长度
        static const size_t kInitialSize = 1024; // 1024 字节大小用于表示缓冲区大小

        explicit Buffer(size_t initialSize = kInitialSize)
            : buffer_(kCheapPrepend + initialSize),
              readerIndex_(kCheapPrepend), // 起始没有数据，读写起点相同
              writerIndex_(kCheapPrepend)
        {
        }

        // 缓冲区中待读取区域长度
        size_t readableBytes() const
        {
            // 写起点 - 读起点
            return writerIndex_ - readerIndex_;
        }

        // 缓冲区剩余可写长度
        size_t writableBytes() const
        {
            // 容器总长度 - 写起点
            return buffer_.size() - writerIndex_;
        }

        // 返回头部大小
        size_t prependableBytes() const
        {
            return readerIndex_;
        }

        // 返回缓冲区中，可读数据的起始地址
        const char *peek() const
        {
            return begin() + readerIndex_;
        }

        // onMessage string <= Buffer
        // 缓冲区中读写位置复位
        void retrieve(size_t len)
        {
            if (len < readableBytes())
            {
                // 应用只读区的可读缓冲区中前 len 个字节
                // 还剩 readerIndex_+len ~ writerIndex_
                readerIndex_ += len;
            }
            else
            {
                // len == readableBytes
                // 全部读取
                retrieveAll();
            }
        }

        void retrieveAll()
        {
            readerIndex_ = writerIndex_ = kCheapPrepend;
        }

        // 把OnMessage函数上的Buffer数据，转成string类型数据
        std::string retrieveAllAsString()
        {
            return retrieveAsString(readableBytes());
        }

        // 把Buffer.vector中可读区域中的数据读取出来，转成string类型返回
        std::string retrieveAsString(size_t len)
        {
            std::string result(peek(), len);
            retrieve(len); // 缓冲区中读写位置复位
            return result;
        }

        // 应对缓冲区剩余空间，不足以存储写入的数据情形
        // Buffer.size() - writerIndex_ < len
        void ensureWritableBytes(size_t len)
        {
            if (writableBytes() < len)
            {
                makeSpace(len); // 扩容函数
            }
        }

    private:
        // 获取 vector 底层起始地址
        // buffer_.begin()：迭代器（指针）
        // *buffer_.begin()：解引用 => 获取首元素值
        //  &*buffer_.begin()：取地址 => 获取首元素地址 => vector 起始地址
        char *begin() { return &*buffer_.begin(); }
        // 应对常对象
        const char *begin() const { return &*buffer_.begin(); }

        // 扩容函数
        void makeSpace(size_t len)
        {
            /**
             * kCheapPrepend | readered | reader | writer  | [现有缓冲区长度]
             * kCheapPrepend |                  len                    | [需求存储长度]
             * prependableBytes() => readerIndex_
             * writableBytes() => writerIndex
             * 如果 readered +  writer < len ： 需要扩容
             * 否则 移动元素
             */
            if (prependableBytes() + writableBytes() < len + kCheapPrepend)
            {
                buffer_.resize(writerIndex_ + len);
            }
            else
            {
                // 获取未读数据长度
                size_t readable = readableBytes();
                std::copy(begin() + readerIndex_,   // 起始位置
                          begin() + writerIndex_,   // 终止位置
                          begin() + kCheapPrepend); // 新起始位置
                // 更新位置表示
                readerIndex_ = kCheapPrepend;
                writerIndex_ = readerIndex_ + readable;
            }
        }

    private:
        /**
         * 为什么使用 vector 管理数据，而不是直接使用裸指针？
         *  => 因为基于容器时，若遇到大量数据！
         *  => 可以依托容器本身的方法来扩容，而不需要我们手动操作！
         *
         * 为什么私有方法中提供了裸指针？
         *  => 因为底层套接字使用的是裸指针，需要匹配便于操作。
         */
        std::vector<char> buffer_;
        size_t readerIndex_; // 正在被读的位置
        size_t writerIndex_; // 正在被写的位置
    };
}