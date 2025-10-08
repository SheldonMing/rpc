#pragma once

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

// 线程安全的队列
template <typename T>
class LockQueue
{
public:
    LockQueue() = default;
    ~LockQueue() = default;

    // 多个线程操作同一个队列
    void push(const T &val)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_queue.push(val);
        m_condVar.notify_all();
    }
    // 一个线程从队列中取出元素
    T pop()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_condVar.wait(lock, [&]()
                       { return !m_queue.empty(); });
        T val = m_queue.front();
        m_queue.pop();
        return val;
    }

private:
    std::queue<T> m_queue;             // 队列
    std::mutex m_mutex;                // 互斥锁
    std::condition_variable m_condVar; // 条件变量
};