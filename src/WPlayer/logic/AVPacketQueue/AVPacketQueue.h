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
		 * @return: AVPacket*
		 */
		AVPacket* waitAndPop(const int timeout);

		/**
		 * @ fn: size
		 * @ brief: 包队列size.
		 * @return: int
		 */
		int size();
		
		/**
		 * @ fn: front
		 * @ brief: 获取队头.
		 * @return: AVPacket
		 */
		AVPacket* front();

		/**
		 * @ fn: release
		 * @ brief: 释放队列资源.
		 * @return: void
		 */
		void release();

	private:
		ThreadSafeQueue<AVPacket*> m_que;
	};
}
