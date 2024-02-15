#include "Thread.h"
#include "CurrentThread.h"

#include <semaphore.h>

namespace myMuduo
{
    // 类的静态变量初始化
    std::atomic_int Thread::numCreated_ = 0;
    Thread::Thread(ThreadFunc func, const std::string &name)
        : started_(false),
          joined_(false),
          tid(0),
          func_(std::move(func)),
          funcName_(name)
    {
        // 设置默认的线程名称
        setDefaultName();
    }
    Thread::~Thread(){
        /**
         * 注意：
         *  => join：表明了是一个普通线程/工作中，在主线程中必须等待
         *  => detach：是进行线程分离 => 守护线程【不需要主线程进行等待】
        */
        if(started_ && !joined_){
            // 如果线程存在，就设置线程分离 => 守护线程
            // 便于自动释放资源，防止出现孤儿线程
            thread_->detach();
        }
    }
    // 线程启动
    void Thread::start(){
        started_ = true;    // 设置状态

        // 用信号量确保子线程启动
        sem_t sem;
        sem_init(&sem, false, 0);

        // 创建出子线程
        thread_ = std::shared_ptr<std::thread>(new std::thread([&](){
            // 获取线程的tid值
            tid_ = CurrentThread::tid();
            sem_post(&sem);
            func_();    // 开启一个新线程，专门执行线程函数【线程中肯定有eventLoop】
        }));

        // 到此为止，线程之间的调度没有顺序而言
        // 我们无法在这进行判断 tid
        // 但是函数结束之前，必须确保创建的子线程启动了
        sem_wait(&sem); // 只要子线程不启动，就会一直阻塞
    }
    // 线程等待
    void Thread::join(){
        joined_ = true;     // 设置状态标识
        thread_->join();
    }

    // 设置默认名称
    void Thread::setDefaultName(){
        int num = ++numCreated_;
        if(funcName_.empty()){
            char buf[32] = {0};
            // 设置线程名字
            snprintf(buf, sizoe(buf), "Thread %d" ,num);
            funcName_ = buf;
        }
    }
}