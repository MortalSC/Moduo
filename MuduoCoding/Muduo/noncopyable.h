#pragma once

/**
 * noncopyable 类的作用：
 * => 允许派生类可以正常的构造和析构！
 * => 限制派生类进行拷贝构造和赋值！
 * 【编程思路借鉴：一劳永逸！避免每个类都要搞限制】
 * 
 * 提问：
 * 1. 为什么删除 拷贝构造 和 赋值运算符重载？
 * 2. 为什么要把 构造函数 和 析构函数 设置为保护属性？
*/

namespace myMuduo{
    class noncopyable{
    public:
        noncopyable(const noncopyable&) = delete;
        void operator = (const noncopyable&) = delete;
    protected:
        noncopyable() = default;
        ~noncopyable() = default;
    private:
    };
};