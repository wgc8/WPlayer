/*****************************************************************//**
 * @ file   DemuxThread.cpp
 * @ brief  ���װ�߳�
 * 
 * @ author wuguochang
 * @ date   2023/12/19
 * @ note History record:
 * 
 *********************************************************************/
#include "DemuxThread.h"
#include "module/Log/easylogging++.h"
#include "logic/AVPacketQueue/AVPacketQueue.h"
#ifdef __cplusplus
extern "C"
{
// ����ffmpegͷ�ļ�
#include "libavutil/avutil.h"
#include "libavformat/avformat.h"
}
#endif // __cplusplus

#define MAX_PACKETQUEUE_SIZE 100		// ��������󳤶�
#define DEMUX_THREAD_SLEEP_TIME 10		// ˯��10����

namespace wplayer
{
	DemuxThread::DemuxThread(AVPacketQueue* audioQue, AVPacketQueue* videoQue): 
		m_queAudioPkt(audioQue),
		m_queVideoPkt(videoQue)
	{
		LOG(INFO) << "construct DemuxThread";
	}

	DemuxThread::~DemuxThread()
	{
		stop();
	}

	int DemuxThread::init(const std::string url)
	{
		m_strFileUrl = url;
		LOG(INFO) << "open file url: " << url;
		//LOG(INFO) << av_version_info();
		int ret = 0;
		//m_pFormatContext = avformat_alloc_context();
		ret = avformat_open_input(&m_pFormatContext, url.c_str(), NULL, NULL);
		if (ret < 0)
		{
			av_strerror(ret, m_strErr, sizeof(m_strErr));
			LOG(INFO) << "avformat_open_input failed, errCode: " << ret << " errStr: " << m_strErr;
			return -1;
		}
		ret = avformat_find_stream_info(m_pFormatContext, NULL);
		if (ret < 0)
		{
			av_strerror(ret, m_strErr, sizeof(m_strErr));
			LOG(INFO) << "avformat_find_stream_info failed, errCode: " << ret << " errStr: " << m_strErr;
			return -1;
		}
		av_dump_format(m_pFormatContext, 0, url.c_str(), 0);

		m_iAudioStreamIdx = av_find_best_stream(m_pFormatContext, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
		m_iVideoStreamIdx = av_find_best_stream(m_pFormatContext, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
		if (m_iAudioStreamIdx < 0 || m_iVideoStreamIdx < 0)
		{
			LOG(ERROR) << "no audio or video stream.";
			return -1;
		}
		LOG(INFO) << "demux thread init success.";
		return 0;
	}

	int DemuxThread::stop()
	{
		ThreadBase::stop();
		if (m_pFormatContext)
			avformat_close_input(&m_pFormatContext);
		return 0;
	}

	void DemuxThread::run()
	{
		LOG(INFO) << "start run demux thread.";
		int ret = 0;
		AVPacket pkt;
		for(;;)
		{
			if (m_bStop)
			{
				LOG(INFO) << "stop demux thread.";
				break;
			}
			// �����л�ѹ����ʱ��������������
			if (m_queAudioPkt->size() > MAX_PACKETQUEUE_SIZE || m_queVideoPkt->size() > MAX_PACKETQUEUE_SIZE)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(DEMUX_THREAD_SLEEP_TIME));
				continue;
			}
			ret = av_read_frame(m_pFormatContext, &pkt);
			if (ret < 0)
			{
				av_strerror(ret, m_strErr, sizeof(m_strErr));
				LOG(INFO) << "av_read_frame failed, errCode: " << ret << " errStr: " << m_strErr;
				break;
			}
			// �ҵ���Ƶ�����뵽��Ƶ����
			if (pkt.stream_index == m_iVideoStreamIdx)
			{
#ifdef DEBUG_MODE
				LOG(INFO) << "push video pkt, size: " << m_queVideoPkt->size();
#endif
				m_queVideoPkt->push(&pkt);
			}
			// �ҵ���Ƶ�����뵽��Ƶ����
			else if (pkt.stream_index == m_iAudioStreamIdx)
			{
#ifdef DEBUG_MODE
				LOG(INFO) << "push audio pkt, size: " << m_queAudioPkt->size();
#endif
				m_queAudioPkt->push(&pkt);
			} 
			else
			{
				av_packet_unref(&pkt);
			}
		}
		LOG(INFO) << "exit demux thread";
	}

	// ��ȡ��Ƶ����������
	AVCodecParameters* DemuxThread::getAudioCodecParames()
	{
		if (!m_pFormatContext || m_iAudioStreamIdx < 0)
			return nullptr;
		return m_pFormatContext->streams[m_iAudioStreamIdx]->codecpar;
	}

	// ��ȡ��Ƶ����������
	AVCodecParameters* DemuxThread::getVideoCodecParames()
	{
		if (!m_pFormatContext || m_iVideoStreamIdx < 0)
			return nullptr;
		return m_pFormatContext->streams[m_iVideoStreamIdx]->codecpar;
	}

	// ��ȡ��Ƶʱ��
	int64_t DemuxThread::getDuration()
	{
		if (m_pFormatContext)
		{
			return m_pFormatContext->duration;
		}
		return 0;
	}
	// ��ȡ��Ƶtimebase
	AVRational DemuxThread::getAudioTimeBase()
	{
		if (!m_pFormatContext || m_iAudioStreamIdx < 0)
			return AVRational{0,0};
		return m_pFormatContext->streams[m_iAudioStreamIdx]->time_base;
	}
	// ��ȡ��Ƶtimebase
	AVRational DemuxThread::getVideoTimeBase()
	{
		if (!m_pFormatContext || m_iVideoStreamIdx < 0)
			return AVRational{ 0,0 };
		return m_pFormatContext->streams[m_iVideoStreamIdx]->time_base;
	}
}
