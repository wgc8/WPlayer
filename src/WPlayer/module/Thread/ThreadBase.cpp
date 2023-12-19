#include "ThreadBase.h"

ThreadBase::ThreadBase()
{
}

ThreadBase::~ThreadBase()
{
	if (m_pThread)
	{
		m_pThread->join();
		delete m_pThread;
		m_pThread = nullptr;
	}
}

void ThreadBase::stop()
{
	m_bStop = true;
	if (m_pThread)
	{
		m_pThread->join();
		delete m_pThread;
		m_pThread = nullptr;
	}
}

void ThreadBase::start()
{
	m_bStop = false;
	try
	{
		m_pThread = new std::thread(&ThreadBase::run, this);
	}
	catch(...)
	{

	}
}
