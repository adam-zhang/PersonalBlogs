#ifndef __SINGLETON__H
#define __SINGLETON__H

#include <memory>
#include <mutex>

class FakeLock
{
	public:
		FakeLock() {}
		~FakeLock() {}
	public:
		void lock() {}
		void unlock() {}
};

class RealLock
{
		std::mutex mutex_;
	public:
		void lock()
		{
			mutex_.lock();
		}

		void unlock()
		{
			mutex_.unlock();
		}

};

template<typename T, typename Lock = FakeLock>
class Singleton
{
public:
	Singleton() {}
	~Singleton() {}
private:
	static T* instance_;
public:
	static T& instance()
	{
		if (!instance_)
		{
			Lock lock;
			lock.lock();
			if (!instance_)
			{
				instance_ = new T;
			}
			lock.unlock();
		}
		return *instance_;
	}
};

template<typename T, typename Lock>
T* Singleton<T, Lock>::instance_ = 0;

#endif//__SINGLETON__H
