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
		 * @return: AVFrame*
		 */
		AVFrame* waitAndPop(const int timeout = 0);
		/**
		 * @ fn: front
		 * @ brief: 获取队头.
		 * @return: AVFrame
		 */
		AVFrame* front();
		/**
		 * @ fn: size
		 * @ brief: 获取队列size.
		 * @return: int
		 */
		int size();

		///**
		// * @ fn: reset
		// * @ brief: 重置队列标志位.
		// * @return: void
		// */
		//void reset();

		/**
		 * @ fn: release
		 * @ brief: 释放队列资源.
		 * @return: void
		 */
		void release();
	private:
		ThreadSafeQueue<AVFrame*> m_que;
	};
}

