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
#include <atomic>
#include <mutex>
#ifdef __cplusplus
extern "C"
{
	// 包含ffmpeg头文件
#include "libavutil/rational.h"
}
#endif // __cplusplus
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

		/**
		 * @ fn: getAudioTimeBase
		 * @ brief: 获取音频时间基.
		 * @return: AVRational
		 */
		AVRational getAudioTimeBase();

		/**
		 * @ fn: getVideoTimeBase
		 * @ brief: 获取视频时间基.
		 * @return: AVRational
		 */
		AVRational getVideoTimeBase();

		/**
		 * @ fn: doSeekFile
		 * @ brief: 执行seek操作.
		 * @ param: posMicoSecondTs
		 * @return: int
		 */
		int doSeekFile(const int64_t dstTs, const int64_t curTs);
	private:
		// 视频流id
		int m_iVideoStreamIdx = -1;
		// 音频流id
		int m_iAudioStreamIdx = -1;
		// 报错信息
		char m_strErr[256] = { 0 };
		// 打开文件名
		std::string m_strFileUrl;
		// seek锁
		std::mutex m_mtxSeek;
		// 视频帧队列
		AVPacketQueue* m_queVideoPkt{ nullptr };
		// 音频帧队列
		AVPacketQueue* m_queAudioPkt{ nullptr };
		// 封装上下文
		AVFormatContext* m_pFormatContext{ nullptr };
	};
}
