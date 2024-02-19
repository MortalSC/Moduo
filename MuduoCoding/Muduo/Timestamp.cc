#include "Timestamp.h"

#include <time.h>
#include <iostream>

namespace myMuduo
{
    // ���캯����Ĭ�� + ���Σ�
    Timestamp::Timestamp() : micSeconds_(0) {}
    Timestamp::Timestamp(int64_t ms)
        : micSeconds_(ms)
    {
    }
    // ��ȡ��ǰʱ��
    Timestamp Timestamp::now()
    {
        return Timestamp(time(NULL));
    }
    // ��ʽ��ʱ�����
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
        return buf;
    }

};


// int main(){

//     std::cout << myMuduo::Timestamp::now().toTimeString() << std::endl;

//     return 0;
// }