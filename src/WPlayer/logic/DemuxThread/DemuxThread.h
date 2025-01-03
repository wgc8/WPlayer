/*****************************************************************//**
 * @ file   DemuxThread.h
 * @ brief  ���װ�߳�
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
	// ����ffmpegͷ�ļ�
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
		// ���ÿ�������
		DemuxThread(DemuxThread&) = delete;
		~DemuxThread();
		int init(const std::string url);
		int stop();
		virtual void run() override;

		/**
		 * @ fn: getAudioCodecParames
		 * @ brief: ��ȡ��Ƶ�⸴����������.
		 * @return: AVCodecParameters *
		 */
		AVCodecParameters* getAudioCodecParames();

		/**
		 * @ fn: getVideoCodecParames
		 * @ brief: ��ȡ��Ƶ�⸴����������.
		 * @return: AVCodecParameters *
		 */
		AVCodecParameters* getVideoCodecParames();
		/**
		 * @ fn: getDuration
		 * @ brief: ��ȡ��Ƶʱ��.
		 * @return: int64_t
		 */
		int64_t getDuration();

		/**
		 * @ fn: getAudioTimeBase
		 * @ brief: ��ȡ��Ƶʱ���.
		 * @return: AVRational
		 */
		AVRational getAudioTimeBase();

		/**
		 * @ fn: getVideoTimeBase
		 * @ brief: ��ȡ��Ƶʱ���.
		 * @return: AVRational
		 */
		AVRational getVideoTimeBase();

		/**
		 * @ fn: doSeekFile
		 * @ brief: ִ��seek����.
		 * @ param: posMicoSecondTs
		 * @return: int
		 */
		int doSeekFile(const int64_t dstTs, const int64_t curTs);
	private:
		// ��Ƶ��id
		int m_iVideoStreamIdx = -1;
		// ��Ƶ��id
		int m_iAudioStreamIdx = -1;
		// ������Ϣ
		char m_strErr[256] = { 0 };
		// ���ļ���
		std::string m_strFileUrl;
		// seek��
		std::mutex m_mtxSeek;
		// ��Ƶ֡����
		AVPacketQueue* m_queVideoPkt{ nullptr };
		// ��Ƶ֡����
		AVPacketQueue* m_queAudioPkt{ nullptr };
		// ��װ������
		AVFormatContext* m_pFormatContext{ nullptr };
	};
}
