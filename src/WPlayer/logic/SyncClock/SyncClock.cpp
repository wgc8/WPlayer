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
		// 获取当前时间点
		system_clock::time_point now = system_clock::now();
		// 从系统纪元计算到当前的间隔
		system_clock::duration dur = now.time_since_epoch();
		// 转换成微妙再取整
		time_t ms = duration_cast<microseconds>(dur).count();
		return ms;
	}
	void SyncClock::init()
	{
	}

	void SyncClock::setClock(double pts)
	{
		//时钟时间转化由毫秒为秒
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