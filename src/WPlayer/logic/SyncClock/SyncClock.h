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
		 * @ brief: ����ʱ��(��Ƶ�̲߳��ϵ����Խ���ʱ�ӣ���ֹ��Ƶ��֡����Ƶ������).
		 * @ param: pts
		 * @return: void
		 */
		void setClock(double pts);

		double getClock();

	private:

		void setClockAt(double pts, double time);

	private:
		double m_dPts{ 0.0 };				//��ǰpts
		double m_dPtsDrift{ 0.0 };			//pts��ʱ��ƫ��ֵ
	};
}
