#pragma once

#include <atomic>
#include <mutex>
#include <condition_variable>

using namespace std;
namespace stiching
{
class semaphore
{
public:
	explicit semaphore(unsigned int initial = 0)
	{
#if defined(HAVE_SEM)
		sem_init(&_sem, 0, initial);
#else
		_count = 0;
#endif
	}
	~semaphore()
	{
#if defined(HAVE_SEM)
		sem_destroy(&_sem);
#endif
	}
	void post(unsigned int n = 1)
	{
#if defined(HAVE_SEM)
		while (n--)
		{
			sem_post(&_sem);
		}
#else
		_count += n;
		if (n == 1)
		{
			_condition.notify_one();
		}
		else
		{
			_condition.notify_all();
		}
#endif
	}
	void wait()
	{
#if defined(HAVE_SEM)
		sem_wait(&_sem);
#else
		unique_lock<mutex> lock(_mutex);
		while (_count == 0)
		{
			_condition.wait(lock);
		}
		--_count;
#endif
	}

private:
#if defined(HAVE_SEM)
	sem_t _sem;
#else
	atomic_int _count;
	mutex _mutex;
	condition_variable_any _condition;
#endif
};
} // namespace stiching