#include "Buffer.h"

#include <errno.h>
#include <sys/uio.h>
#include <unistd.h>
namespace myMuduo
{
    // 从 fd 上读取数据
    // muduo 工作在 LT 模式
    // Buffer缓冲区是有大小限制的，但是从fd上读取数据时，我们不知道tcp数据的最终大小
    // 单次最多读取 64k 数据
    ssize_t Buffer::readFd(int fd, int *saveErrno)
    {
        // 准备一个栈上的内存空间 64k
        char extrabuf[65536];
        /**
         * struct iovec 两个成员：
         *  => iov_base：需要读取的起始地址
         *  => iov_len：读取的长度
         */
        struct iovec vec[2];
        // 获取缓冲区中剩余可写大小【不一定够存储fd上读出的数据】
        const size_t writable = writableBytes();

        // 先将数据读取入 Buffer 缓冲区！
        vec[0].iov_base = begin() + writerIndex_;
        vec[0].iov_len = writable;
        // 如果 Buffer 不够，就临时存储再 extrabuf 中
        vec[1].iov_base = extrabuf;
        vec[1].iov_len = sizeof(extrabuf);

        const int iovcnt = (writable < sizeof(extrabuf)) ? 2 : 1;
        const ssize_t n = ::readv(fd, vec, iovcnt);
        if (n < 0)
        {
            *saveErrno = errno;
        }
        else if (n <= writable)
        {
            // Buffer 可写空间足够
            writerIndex_ += n;
        }
        else
        {
            // Buffer 可写空间不够
            // extrabuf 中也有数据
            writerIndex_ = buffer_.size();  // Buffer 已经被充分利用
            append(extrabuf, n - writable); // 扩容再填充数据
        }
        return n;
    }

    ssize_t Buffer::writeFd(int fd, int *saveErrno)
    {
        ssize_t n = ::write(fd, peek(), readableBytes());
        if(n < 0){
            *saveErrno = errno;
        }
        return n;
    }
}