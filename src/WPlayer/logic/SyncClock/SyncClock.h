#pragma once
#include <ctime>

namespace wplayer
{
	class SyncClock
	{
	public:
		SyncClock();
		virtual ~SyncClock();

		void init();

		time_t getMicroseconds();
		/**
		 * @ fn: setClock
		 * @ brief: 设置时钟(音频线程不断调用以矫正时钟，防止音频丢帧后，视频跟不上).
		 * @ param: pts
		 * @return: void
		 */
		void setClock(double pts);

		double getClock();

	private:

		void setClockAt(double pts, double time);

	private:
		double m_dPts{ 0.0 };				//当前pts
		double m_dPtsDrift{ 0.0 };			//pts与时钟偏移值
	};
}
