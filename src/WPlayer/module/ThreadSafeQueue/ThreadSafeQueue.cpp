#include "ThreadSafeQueue.h"

// �������
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
		return -1;	// �жϺ󵯳�
	}
	if (m_que.empty())
	{
		return -2;	// �ȴ���ʱ
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
		return std::shared_ptr<T>();;	// �жϺ󵯳�
	}
	if (m_que.empty())
	{
		return std::shared_ptr<T>();	// �ȴ���ʱ
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
	// �ı��жϱ�־λ����������߳�����״̬
	m_iAbort = 1;
	m_condVar.notify_all();
}
