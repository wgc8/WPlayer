/*****************************************************************//**
 * @ file   DemuxThread.h
 * @ brief  解封装线程
 * 
 * @ author wuguochang
 * @ date   2023/12/19
 * @ note History record:
 * 
 *********************************************************************/
#pragma once
#include "module/Thread/ThreadBase.h"
#include <string>

class AVFormatContext;
class AVCodecParameters;
namespace wplayer
{
	class AVPacketQueue;

	class DemuxThread: public ThreadBase
	{
	public:
		//DemuxThread() = delete;
		DemuxThread(AVPacketQueue* audioQue, AVPacketQueue* videoQue);
		// 禁用拷贝构造
		DemuxThread(DemuxThread&) = delete;
		~DemuxThread();
		int init(const std::string url);
		int stop();
		virtual void run() override;

		/**
		 * @ fn: getAudioCodecParames
		 * @ brief: 获取音频解复用器上下文.
		 * @return: AVCodecParameters *
		 */
		AVCodecParameters* getAudioCodecParames();

		/**
		 * @ fn: getVideoCodecParames
		 * @ brief: 获取视频解复用器上下文.
		 * @return: AVCodecParameters *
		 */
		AVCodecParameters* getVideoCodecParames();
		/**
		 * @ fn: getDuration
		 * @ brief: 获取视频时长.
		 * @return: int64_t
		 */
		int64_t getDuration();
	private:
		// 视频流id
		int m_iVideoStreamIdx = -1;
		// 音频流id
		int m_iAudioStreamIdx = -1;
		// 报错信息
		char m_strErr[256] = { 0 };
		// 打开文件名
		std::string m_strFileUrl;
		// 视频帧队列
		AVPacketQueue* m_queVideoPkt{ nullptr };
		// 音频帧队列
		AVPacketQueue* m_queAudioPkt{ nullptr };
		// 封装上下文
		AVFormatContext* m_pFormatContext{ nullptr };
	};
}
