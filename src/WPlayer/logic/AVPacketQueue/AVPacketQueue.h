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
		 * @return: AVPacket*
		 */
		AVPacket* waitAndPop(const int timeout);

		/**
		 * @ fn: size
		 * @ brief: ������size.
		 * @return: int
		 */
		int size();
		
		/**
		 * @ fn: front
		 * @ brief: ��ȡ��ͷ.
		 * @return: AVPacket
		 */
		AVPacket* front();

		/**
		 * @ fn: release
		 * @ brief: �ͷŶ�����Դ.
		 * @return: void
		 */
		void release();

	private:
		ThreadSafeQueue<AVPacket*> m_que;
	};
}
