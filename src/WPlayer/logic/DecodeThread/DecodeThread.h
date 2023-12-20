/*****************************************************************//**
 * @ file   DecodeThread.h
 * @ brief  �������߳�
 * 
 * @ author wuguochang
 * @ date   2023/12/20
 * @ note History record:
 * 
 *********************************************************************/
#pragma once
#include "module/Thread/ThreadBase.h"

class AVCodecContext;
class AVCodecParameters;
namespace wplayer
{
	class AVFrameQueue;
	class AVPacketQueue;
	class DecodeThread : public ThreadBase
	{
	public:
		DecodeThread(AVPacketQueue* pktQue, AVFrameQueue* frmQue);
		// ���ÿ�������
		DecodeThread(DecodeThread&) = delete;
		~DecodeThread();
		/**
		 * @ fn: stop
		 * @ brief: ֹͣ�����߳�.
		 * @return: int
		 */
		int stop();
		/**
		 * @ fn: run
		 * @ brief: �߳�ִ�к���.
		 * @return: void
		 */
		virtual void run() override;
		/**
		 * @ fn: init
		 * @ brief: ��ʼ��������������.
		 * @return: int
		 */
		int init(AVCodecParameters* condecParams);
	private:
		// ������Ϣ
		char m_strErr[256] = { 0 };
		// ������������
		AVCodecContext* m_pCodecContext{ nullptr };
		// ֡����
		AVFrameQueue* m_queFrame{ nullptr };
		// ������
		AVPacketQueue* m_quePacket{ nullptr };
	};
}
