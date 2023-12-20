#pragma once
#include <memory>
#include <queue>
#include <mutex>
#include <condition_variable>

template <typename T>
class ThreadSafeQueue
{
public:
	ThreadSafeQueue() = default;
	~ThreadSafeQueue() {};

	int push(const T& t) {
		std::lock_guard<std::mutex> lock(m_mutex);
		if (1 == m_iAbort)
		{
			return -1;
		}
		m_que.push(t);
		m_condVar.notify_one();
		return 0;
	};

	int waitAndPop(T& t, const int timeout = 0) {
		std::unique_lock<std::mutex> lock(m_mutex);
		m_condVar.wait_for(lock, std::chrono::milliseconds(timeout), [this]() {
			return !m_que.empty() | m_iAbort;
			});
		if (1 == m_iAbort)
		{
			return -1;	// 中断后弹出
		}
		if (m_que.empty())
		{
			return -2;	// 等待超时
		}
		t = std::move(m_que.front());
		m_que.pop();
		return 0;
	};

	std::shared_ptr<T> waitAndPop(const int timeout = 0) {
		std::unique_lock<std::mutex> lock(m_mutex);
		m_condVar.wait_for(lock, std::chrono::milliseconds(timeout), [this]() {
			return !m_que.empty() | m_iAbort;
			});
		if (1 == m_iAbort)
		{
			return std::shared_ptr<T>();;	// 中断后弹出
		}
		if (m_que.empty())
		{
			return std::shared_ptr<T>();	// 等待超时
		}
		std::shared_ptr<T> pItem = std::make_shared<T>(m_que.front());
		m_que.pop();
		return pItem;
	};

	int tryPop(T& t) {
		std::lock_guard<std::mutex> lock(m_mutex);
		if (m_que.empty())
			return -1;
		t = std::move(m_que.front());
		m_que.pop();
		return 0;
	};

	std::shared_ptr<T> tryPop() {
		std::lock_guard<std::mutex> lock(m_mutex);
		if (m_que.empty())
			return std::shared_ptr<T>();
		std::shared_ptr<T> pItem = std::make_shared<T>(m_que.front());
		m_que.pop();
		return pItem;
	};
	
	const bool isEmpty() {
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_que.empty();
	};
	
	int getSize() {
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_que.size();
	};
	
	void abort() {
		// 改变中断标志位，解除所有线程阻塞状态
		m_iAbort = 1;
		m_condVar.notify_all();
	};

	std::shared_ptr<T> front() 
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		if (1 == m_iAbort)
		{
			return std::shared_ptr<T>();
		}
		if (m_que.empty())
		{
			return std::shared_ptr<T>();
		}
		std::shared_ptr<T> pItem = std::make_shared<T>(m_que.front());
		return pItem;
	}

	int front(T& val)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		if (1 == m_iAbort)
		{
			return -1;
		}
		if (m_que.empty())
		{
			return -2;
		}
		val = m_que.front();
		return 0;
	}
private:
	// 队列
	std::queue<T> m_que;
	// 队列访问互斥量
	mutable std::mutex m_mutex;
	// 唤醒条件变量
	std::condition_variable m_condVar;
	// 中断标志
	int m_iAbort{ 0 };
};
