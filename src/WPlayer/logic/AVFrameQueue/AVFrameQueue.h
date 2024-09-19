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
		AVFrame* waitAndPop(const int timeout);
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

	private:
		/**
		 * @ fn: release
		 * @ brief: �ͷŶ�����Դ.
		 * @return: void
		 */
		void release();

		ThreadSafeQueue<AVFrame*> m_que;
	};
}

