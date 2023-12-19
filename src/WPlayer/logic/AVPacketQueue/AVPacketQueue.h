/*****************************************************************//**
 * @ file   AVPacketQueue.h
 * @ brief  AV������
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
		 * @ brief: �ж϶���.
		 * @return: void
		 */
		void abort();

		/**
		 * @ fn: push
		 * @ brief: ���.
		 * @ param: pkt
		 * @return: int
		 */
		int push(AVPacket* pkt);

		/**
		 * @ fn: waitAndPop
		 * @ brief: ����pop.
		 * @ param: timeout
		 * @return: std::shared_ptr<AVPacket>
		 */
		std::shared_ptr<AVPacket> waitAndPop(const int timeout);

		/**
		 * @ fn: size
		 * @ brief: ������size.
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

		ThreadSafeQueue<AVPacket*> m_que;
	};
}
