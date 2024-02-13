#pragma once

#include <string>

#include "noncopyable.h"
#include "Timestamp.h"
/**
 * 设置日志级别:
 * => INFO：程序一切正常
 * => ERROR：发生错误，部分功能没有执行
 * => FATAL：致命错误，导致程序无法继续执行
 * => DEBUG：调试信息，系统正常运行时一般时关闭的，通过宏来控制是否输出
 * =>
 */

namespace myMuduo
{
    // 定义宏：让用户便于操作，用户不关注你的单例对象的！
    // 注：宏是单行代码，因此使用了 \，注后面不要有空格
#define Log_INFO(logmsgFormat, ...)                       \
    do                                                    \
    {                                                     \
        Log &log = Log::instance();                       \
        log.setLogLevel(INFO);                            \
        char buf[1024] = {0};                             \
        snprintf(buf, 1024, logmsgFormat, ##__VA_ARGS__); \
        log.log(buf);                                     \
    } while (0)

#define Log_ERROR(logmsgFormat, ...)                      \
    do                                                    \
    {                                                     \
        Log &log = Log::instance();                       \
        log.setLogLevel(ERROR);                           \
        char buf[1024] = {0};                             \
        snprintf(buf, 1024, logmsgFormat, ##__VA_ARGS__); \
        log.log(buf);                                     \
    } while (0)

#define Log_FATAL(logmsgFormat, ...)                      \
    do                                                    \
    {                                                     \
        Log &log = Log::instance();                       \
        log.setLogLevel(FATAL);                           \
        char buf[1024] = {0};                             \
        snprintf(buf, 1024, logmsgFormat, ##__VA_ARGS__); \
        log.log(buf);                                     \
        exit(-1);                                         \
    } while (0)

#ifdef ONDEBUG
#define Log_DEBUG(logmsgFormat, ...)                      \
    do                                                    \
    {                                                     \
        Log &log = Log::instance();                       \
        log.setLogLevel(DEBUG);                           \
        char buf[1024] = {0};                             \
        snprintf(buf, 1024, logmsgFormat, ##__VA_ARGS__); \
        log.log(buf);                                     \
    } while (0)

#else

#define Log_DEBUG(logmsgFormat, ...)

#endif

    enum LogLevel
    {
        INFO,  // 程序正常输出提示信息
        ERROR, // 错误信息（部分功能可能未执行）
        FATAL, // 致命错误（导致程序无法继续执行）
        DEBUG  // 调试信息
    };

    /* 日志类（单例设计） */
    class Log : noncopyable
    {
    public:
        // 单例实例的获取
        static Log &instance();
        // 设置日志级别
        void setLogLevel(int level);
        // 打印日志信息
        void log(std::string msg);

    private:
        // int _logLevel;
        int logLevel_; // 借鉴修改前置下划线写法
        Log() {}
    };
};