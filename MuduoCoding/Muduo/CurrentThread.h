#pragma once

#include <unistd.h>
#include <sys/syscall.h>

namespace myMuduo{
    namespace CurrentThread{
        // extern 全局声明
        // __thread：等价于 thread_local
        // t_cachedTid 本身是全局变量！
        // 使用__thread修饰之后，可以使得每一个线程中拷贝一份，操作中就是操作拷贝本【不存在线程共享问题】
        extern __thread int t_cachedTid;
        
        void cacheTid();
        // 获取当前线程 id
        inline int tid(){
            // __builtin_expect 底层函数：租用
            if(__builtin_expect(t_cachedTid==0,0)){
                cacheTid();
            }
            return t_cachedTid;
        }
    }
}