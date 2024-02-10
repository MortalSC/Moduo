#include "Log.h"

#include <iostream>

namespace myMuduo
{
    // 单例实例的获取
    Log &Log::instance()
    {
        static Log log;
        return log;
    }

    // 设置日志级别
    void Log::setLogLevel(int level)
    {
        logLevel_ = level;
    }

    // 打印日志信息
    // 格式逻辑：
    // [级别信息] time : msg
    void Log::log(std::string msg)
    {
        // Displays  the level of the log
        switch (logLevel_)
        {
        case INFO:
            std::cout << "[ INFO ]"; 
            break;
        case ERROR:
            std::cout << "[ ERROR ]"; 
            break;
        case FATAL:
            std::cout << "[ FATAL ]"; 
            break;
        case DEBUG:
            std::cout << "[ DEBUY ]"; 
            break;
        default:
            break;
        }

        // Displays time information and message
        std::cout << Timestamp::now().toTimeString() << " : " << msg << std::endl;
    }
};