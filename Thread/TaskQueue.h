#pragma once

#include <list>
#include <deque>
#include <atomic>
#include <mutex>
#include <functional>
#include "semaphore.h"

using namespace std;

namespace stiching
{
template <typename T>
class TaskQueue
{
public:
    //打入任务至列队
    void push_task(T &&task_func)
    {
        {
            lock_guard<decltype(_mutex)> lock(_mutex);
            _queue.emplace_back(std::move(task_func));
        }
        _sem.post();
    }
    void push_task_first(T &&task_func)
    {
        {
            lock_guard<decltype(_mutex)> lock(_mutex);
            _queue.emplace_front(std::move(task_func));
        }
        _sem.post();
    }
    //清空任务列队
    void push_exit(unsigned int n)
    {
        _sem.post(n);
    }
    //从列队获取一个任务，由执行线程执行
    bool get_task(T &tsk)
    {
        _sem.wait();
        lock_guard<decltype(_mutex)> lock(_mutex);
        if (_queue.size() == 0)
        {
            return false;
        }
        //改成右值引用后性能提升了1倍多！
        tsk = std::move(_queue.front());
        _queue.pop_front();
        return true;
    }
    uint64_t size() const
    {
        lock_guard<decltype(_mutex)> lock(_mutex);
        return _queue.size();
    }

private:
    list<T> _queue;
    mutable mutex _mutex;
    semaphore _sem;
};

} // namespace stiching