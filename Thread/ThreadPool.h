#pragma once

#include <assert.h>
#include <vector>
#include "threadgroup.h"
#include "TaskQueue.h"
#include "ThreadExecutor.h"
#include <iostream>

namespace stiching
{

class ThreadPool : public TaskExecutor
{
public:
    enum Priority
    {
        PRIORITY_LOWEST = 0,
        PRIORITY_LOW,
        PRIORITY_NORMAL,
        PRIORITY_HIGH,
        PRIORITY_HIGHEST
    };

    //num:线程池线程个数
    ThreadPool(int num = 1,
               Priority priority = PRIORITY_HIGHEST,
               bool autoRun = true) : _thread_num(num), _priority(priority)
    {
        if (autoRun)
        {
            start();
        }
    }
    ~ThreadPool()
    {
        shutdown();
        wait();
    }

    //把任务打入线程池并异步执行
    bool async(Task &&task, bool may_sync = true) override
    {
        if (may_sync && _thread_group.is_this_thread_in())
        {
            task();
        }
        else
        {
            _queue.push_task(std::move(task));
        }
        return true;
    }
    bool async_first(Task &&task, bool may_sync = true) override
    {
        if (may_sync && _thread_group.is_this_thread_in())
        {
            task();
        }
        else
        {
            _queue.push_task_first(std::move(task));
        }
        return true;
    }
    bool sync(Task &&task) override
    {
        semaphore sem;
        bool flag = async([&]() {
            task();
            sem.post();
        });
        if (flag)
        {
            sem.wait();
        }
        return flag;
    }
    bool sync_first(Task &&task) override
    {
        semaphore sem;
        bool flag = async_first([&]() {
            task();
            sem.post();
        });
        if (flag)
        {
            sem.wait();
        }
        return flag;
    }

    uint64_t size()
    {
        return _queue.size();
    }

    static bool setPriority(Priority priority = PRIORITY_NORMAL,
                            thread::native_handle_type threadId = 0)
    {
        // set priority
#if defined(_WIN32)
        static int Priorities[] = {THREAD_PRIORITY_LOWEST, THREAD_PRIORITY_BELOW_NORMAL, THREAD_PRIORITY_NORMAL, THREAD_PRIORITY_ABOVE_NORMAL, THREAD_PRIORITY_HIGHEST};
        if (priority != PRIORITY_NORMAL && SetThreadPriority(GetCurrentThread(), Priorities[priority]) == 0)
        {
            return false;
        }
        return true;
#else
        static int Min = sched_get_priority_min(SCHED_OTHER);
        if (Min == -1)
        {
            return false;
        }
        static int Max = sched_get_priority_max(SCHED_OTHER);
        if (Max == -1)
        {
            return false;
        }
        static int Priorities[] = {Min, Min + (Max - Min) / 4, Min + (Max - Min) / 2, Min + (Max - Min) * 3 / 4, Max};

        if (threadId == 0)
        {
            threadId = pthread_self();
        }
        struct sched_param params;
        params.sched_priority = Priorities[priority];
        return pthread_setschedparam(threadId, SCHED_OTHER, &params) == 0;
#endif
    }

    void start()
    {
        if (_thread_num <= 0)
            return;
        auto total = _thread_num - _thread_group.size();
        for (int i = 0; i < total; ++i)
        {
            _thread_group.create_thread(bind(&ThreadPool::run, this));
        }
    }

private:
    void run()
    {
        ThreadPool::setPriority(_priority);
        TaskExecutor::Task task;
        while (true)
        {
            startSleep();
            if (!_queue.get_task(task))
            {
                //空任务，退出线程
                break;
            }
            sleepWakeUp();
            try
            {
                task();
                task = nullptr;
            }
            catch (std::exception &ex)
            {
                std::cout << ex.what();
            }
        }
    }

    void wait()
    {
        _thread_group.join_all();
    }

    void shutdown()
    {
        _queue.push_exit(_thread_num);
    }

private:
    TaskQueue<TaskExecutor::Task> _queue;
    thread_group _thread_group;
    int _thread_num;
    Priority _priority;
};
} // namespace stiching