#include "ThreadSafeQueue.h"

// 塞入队列
template<typename T>
int ThreadSafeQueue<T>::push(const T&& t)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	if (1 == m_iAbort)
	{
		return -1;
	}
	m_que.push(std::forward <T>(t));
	m_condVar.notify_one();
	return 0;
}

template<typename T>
int ThreadSafeQueue<T>::waitAndPop(T& t, int timeout)
{
	std::unique_lock(std::mutex) lock(m_mutex);
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
}

template<typename T>
std::shared_ptr<T> ThreadSafeQueue<T>::waitAndPop(int timeout)
{
	std::unique_lock(std::mutex) lock(m_mutex);
	m_condVar.wait_for(lock,std::chrono::milliseconds(timeout), [this]() {
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
}

template<typename T>
int ThreadSafeQueue<T>::tryPop(T& t)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	if (m_que.empty())
		return -1;
	t = std::move(m_que.front());
	m_que.pop();
	return 0;
}

template<typename T>
std::shared_ptr<T> ThreadSafeQueue<T>::tryPop()
{
	std::lock_guard<std::mutex> lock(m_mutex);
	if (m_que.empty())
		return std::shared_ptr<T>();
	std::shared_ptr<T> pItem = std::make_shared<T>(m_que.front());
	m_que.pop();
	return pItem;
}

template<typename T>
const bool ThreadSafeQueue<T>::isEmpty()
{
	std::lock_guard<std::mutex> lock(m_mutex);
	return m_que.empty();
}

template<typename T>
int ThreadSafeQueue<T>::getSize()
{
	std::lock_guard<std::mutex> lock(m_mutex);
	return m_que.size();
}

template<typename T>
void ThreadSafeQueue<T>::abort()
{
	// 改变中断标志位，解除所有线程阻塞状态
	m_iAbort = 1;
	m_condVar.notify_all();
}
