#pragma once 

#include <iostream>
#include <string>
namespace myMuduo{
    class Timestamp{
    public:
    // 构造函数（默认 + 含参）
        Timestamp();
       explicit Timestamp(int64_t ms);
    // 获取当前时间
        static Timestamp now();
    // 格式化时间输出
        std::string toTimeString() const;
    private:
        int64_t micSeconds_;
    };
};