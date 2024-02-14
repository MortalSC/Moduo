#include "CurrentThread.h"



namespace myMuduo{
    namespace CurrentThread{
        // t_cachedTid 设置默认是 0，具体 tid 通过 tid() 函数获取【内联声明定义在头文件中了】
        __thread int t_cachedTid = 0;
        void cacheTid()
        {
            if(t_cachedTid == 0){
                // 通过Linux系统调用获取当前线程 tid 值
                t_cachedTid = static_cast<pid_t>(::syscall(SYS_gettid));
            }
        }
    }
}