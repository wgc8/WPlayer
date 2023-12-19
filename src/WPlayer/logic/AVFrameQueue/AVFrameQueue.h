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
		 * @ brief: 中断.
		 * @return: void
		 */
		void abort();
		/**
		 * @ fn: push
		 * @ brief: 入队.
		 * @ param: pkt
		 * @return: int
		 */
		int push(AVFrame* pkt);
		/**
		 * @ fn: waitAndPop
		 * @ brief: 阻塞读.
		 * @ param: timeout
		 * @return: std::shared_ptr<AVFrame>
		 */
		std::shared_ptr<AVFrame> waitAndPop(const int timeout);
		/**
		 * @ fn: front
		 * @ brief: 获取队头.
		 * @return: std::shared_ptr<AVFrame>
		 */
		std::shared_ptr<AVFrame> front();
	private:
		/**
		 * @ fn: release
		 * @ brief: 释放队列资源.
		 * @return: void
		 */
		void release();

		ThreadSafeQueue<AVFrame*> m_que;
	};
}

