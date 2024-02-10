#include "Timestamp.h"

#include <time.h>
#include <iostream>

namespace myMuduo
{
    // 构造函数（默认 + 含参）
    Timestamp::Timestamp() : micSeconds_(0) {}
    Timestamp::Timestamp(int64_t ms)
        : micSeconds_(ms)
    {
    }
    // 获取当前时间
    Timestamp Timestamp::now()
    {
        return Timestamp(time(NULL));
    }
    // 格式化时间输出
    std::string Timestamp::toTimeString() const
    {
        char buf[128] = {0};
        tm *tm_time = localtime(&micSeconds_);
        snprintf(buf, 128, "%4d/%02d/%02d %02d:%02d:%02d",
                 tm_time->tm_year + 1900,
                 tm_time->tm_mon + 1,
                 tm_time->tm_mday,
                 tm_time->tm_hour,
                 tm_time->tm_min,
                 tm_time->tm_sec);
    }

};


// int main(){

//     std::cout << myMuduo::Timestamp::now().toTimeString() << std::endl;

//     return 0;
// }