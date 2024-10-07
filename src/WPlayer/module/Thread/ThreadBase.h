#ifndef __THRED_BASE_H__
#define __THRED_BASE_H__
#include <thread>
class ThreadBase
{
public:
	ThreadBase();
	virtual ~ThreadBase();

	bool isRunning() { return !m_bStop; };
	void stop();
	int start();
	virtual void run() = 0;
protected:
	bool m_bStop{ true };
	std::thread* m_pThread{ nullptr };
};
#endif	//__THRED_BASE_H__

