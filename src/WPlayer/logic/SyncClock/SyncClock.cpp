#include "SyncClock.h"
#include <chrono>

#include <math.h>
using namespace std::chrono;
namespace wplayer
{
	SyncClock::SyncClock()
	{
		//setClock(NAN);
		reset();
	}

	SyncClock::~SyncClock()
	{
	}

	time_t SyncClock::getMicroseconds()
	{
		// ��ȡ��ǰʱ���
		system_clock::time_point now = system_clock::now();
		// ��ϵͳ��Ԫ���㵽��ǰ�ļ��
		system_clock::duration dur = now.time_since_epoch();
		// ת����΢����ȡ��
		time_t ms = duration_cast<microseconds>(dur).count();
		return ms;
	}
	void SyncClock::init()
	{
	}

	void SyncClock::setClock(double pts)
	{
		//ʱ��ʱ��ת���ɺ���Ϊ��
		double time = getMicroseconds() / 1000000.0;
		setClockAt(pts, time);
	}

	double SyncClock::getClock()
	{
		double time = getMicroseconds() / 1000000.0;
		return time + m_dPtsDrift;
	}

	void SyncClock::reset()
	{
		m_dPts = 0.0;
		m_dPtsDrift = 0.0;
	}

	void SyncClock::setClockAt(double pts, double time)
	{
		m_dPts = pts;
		m_dPtsDrift = pts - time;
	}
}