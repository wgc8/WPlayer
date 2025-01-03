#pragma once
#include "module/ThreadSafeQueue/ThreadSafeQueue.h"
#include <memory>

class AVFrame;
namespace wplayer {
	class AVFrameQueue
	{
	public:
		AVFrameQueue();
		~AVFrameQueue();
		/**
		 * @ fn: abort
		 * @ brief: �ж�.
		 * @return: void
		 */
		void abort();
		/**
		 * @ fn: push
		 * @ brief: ���.
		 * @ param: pkt
		 * @return: int
		 */
		int push(AVFrame* pkt);
		/**
		 * @ fn: waitAndPop
		 * @ brief: ������.
		 * @ param: timeout
		 * @return: AVFrame*
		 */
		AVFrame* waitAndPop(const int timeout = 0);
		/**
		 * @ fn: front
		 * @ brief: ��ȡ��ͷ.
		 * @return: AVFrame
		 */
		AVFrame* front();
		/**
		 * @ fn: size
		 * @ brief: ��ȡ����size.
		 * @return: int
		 */
		int size();

		///**
		// * @ fn: reset
		// * @ brief: ���ö��б�־λ.
		// * @return: void
		// */
		//void reset();

		/**
		 * @ fn: release
		 * @ brief: �ͷŶ�����Դ.
		 * @return: void
		 */
		void release();
	private:
		ThreadSafeQueue<AVFrame*> m_que;
	};
}

