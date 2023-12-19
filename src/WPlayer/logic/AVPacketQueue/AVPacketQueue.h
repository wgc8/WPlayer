/*****************************************************************//**
 * @ file   AVPacketQueue.h
 * @ brief  AV包队列
 * 
 * @ author wuguochang
 * @ date   2023/12/19
 * @ note History record:
 * 
 *********************************************************************/
#pragma once
#include "module/ThreadSafeQueue/ThreadSafeQueue.h"
#include <memory>
class AVPacket;
namespace wplayer {
	class AVPacketQueue
	{
	public:
		AVPacketQueue();
		~AVPacketQueue();
		/**
		 * @ fn: abort
		 * @ brief: 中断队列.
		 * @return: void
		 */
		void abort();

		/**
		 * @ fn: push
		 * @ brief: 入队.
		 * @ param: pkt
		 * @return: int
		 */
		int push(AVPacket* pkt);

		/**
		 * @ fn: waitAndPop
		 * @ brief: 阻塞pop.
		 * @ param: timeout
		 * @return: std::shared_ptr<AVPacket>
		 */
		std::shared_ptr<AVPacket> waitAndPop(const int timeout);

		/**
		 * @ fn: size
		 * @ brief: 包队列size.
		 * @return: int
		 */
		int size();
	private:
		/**
		 * @ fn: release
		 * @ brief: 释放队列资源.
		 * @return: void
		 */
		void release();

		ThreadSafeQueue<AVPacket*> m_que;
	};
}
