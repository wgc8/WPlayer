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
		 * @return: std::shared_ptr<AVFrame>
		 */
		std::shared_ptr<AVFrame> waitAndPop(const int timeout);
		/**
		 * @ fn: front
		 * @ brief: ��ȡ��ͷ.
		 * @return: std::shared_ptr<AVFrame>
		 */
		std::shared_ptr<AVFrame> front();
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

