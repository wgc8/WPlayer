/*****************************************************************//**
 * @ file   DecodeThread.h
 * @ brief  解码器线程
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
		// 禁用拷贝构造
		DecodeThread(DecodeThread&) = delete;
		~DecodeThread();
		/**
		 * @ fn: stop
		 * @ brief: 停止解码线程.
		 * @return: int
		 */
		int stop();
		/**
		 * @ fn: run
		 * @ brief: 线程执行函数.
		 * @return: void
		 */
		virtual void run() override;
		/**
		 * @ fn: init
		 * @ brief: 初始化解码器上下文.
		 * @return: int
		 */
		int init(AVCodecParameters* condecParams);
	private:
		// 报错信息
		char m_strErr[256] = { 0 };
		// 解码器上下文
		AVCodecContext* m_pCodecContext{ nullptr };
		// 帧队列
		AVFrameQueue* m_queFrame{ nullptr };
		// 包队列
		AVPacketQueue* m_quePacket{ nullptr };
	};
}
