/*****************************************************************//**
 * @ file   DecodeThread.cpp
 * @ brief  �������߳�
 * 
 * @ author wuguochang
 * @ date   2023/12/20
 * @ note History record:
 * 
 *********************************************************************/
#include "DecodeThread.h"
#include "logic/AVPacketQueue/AVPacketQueue.h"
#include "logic/AVFrameQueue/AVFrameQueue.h"
#include "module/Log/easylogging++.h"
#include <memory>
#ifdef __cplusplus
extern "C"
{
	// ����ffmpegͷ�ļ�
#include "libavutil/avutil.h"
#include "libavCodec/avcodec.h"

}
#endif // __cplusplus

#define MAX_FRAMEQUEUE_SIZE 16		// ��������󳤶�
#define DECODEC_THREAD_SLEEP_TIME 10		// ˯��10����

namespace wplayer
{
	DecodeThread::DecodeThread(AVPacketQueue* pktQue, AVFrameQueue* frmQue):
		m_quePacket(pktQue),
		m_queFrame(frmQue)
	{
	}
	DecodeThread::~DecodeThread()
	{
		LOG(INFO) << "~DecodeThread";
		if (m_pThread)
		{
			stop();
		}
		if (m_pCodecContext)
		{
			avcodec_close(m_pCodecContext);
		}
	}

	int DecodeThread::stop()
	{
		ThreadBase::stop();
		return 0;
	}

	// �����߳�ִ�к���
	void DecodeThread::run()
	{
		LOG(INFO) << "start Decodec Thread.";
		AVFrame* tmpFrm = av_frame_alloc();
		// ��ȡ������
		for (;;)
		{
			// �˳��߳�
			if (m_bStop)
			{
				LOG(INFO) << "quit thread then, bStop: " << m_bStop;
				break;
			}
			// �����л�ѹ����ʱ��������������
			if (m_queFrame->size() > MAX_FRAMEQUEUE_SIZE)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(DECODEC_THREAD_SLEEP_TIME));
				continue;
			}
			auto pPkt = m_quePacket->waitAndPop(10);
			if (!pPkt)
			{
#ifdef DEBUG_MODE
				LOG(INFO) << "not get packet";
#endif
				continue;
			}
			int ret = avcodec_send_packet(m_pCodecContext, pPkt);
			av_packet_free(&pPkt);
			if (ret < 0)
			{
				av_strerror(ret, m_strErr, sizeof(m_strErr));
				LOG(INFO) << "avcodec_send_packet failed, errCode: " << ret << " errStr: " << m_strErr;

			}
			// ��ȡ��������֡
			for (;;)
			{
				ret = avcodec_receive_frame(m_pCodecContext, tmpFrm);
				if (0 == ret)
				{
					m_queFrame->push(tmpFrm);
					//LOG(INFO) << m_pCodecContext->codec->name << " frame que size: " << m_queFrame->size();
				}
				else if (AVERROR(EAGAIN) == ret)
				{
					break;
				}
				else
				{
					m_bStop = true;
					av_strerror(ret, m_strErr, sizeof(m_strErr));
					LOG(INFO) << "avcodec_receive_frame failed, errCode: " << ret << " errStr: " << m_strErr;
					break;
				}
			}
		}
		// �ͷ���ʱ֡����
		av_frame_free(&tmpFrm);
		LOG(INFO) << "exit Decode Thread.";
	}

	// ��ʼ��������������
	int DecodeThread::init(AVCodecParameters* condecParams)
	{
		if (!condecParams)
		{
			LOG(ERROR) << "DecodeThread::init failed, condecParams is nullptr";
			return -1;
		}
		m_pCodecContext = avcodec_alloc_context3(nullptr);
		
		if (!m_pCodecContext)
		{
			LOG(ERROR) << "DecodeThread::init failed, m_pCodecContext is  nullptr";
			return -1;
		}
		// ���������������䵽��������
		int ret = avcodec_parameters_to_context(m_pCodecContext, condecParams);
		if (ret < 0)
		{
			av_strerror(ret, m_strErr, sizeof(m_strErr));
			LOG(INFO) << "avcodec_parameters_to_context failed, errCode: " << ret << " errStr: " << m_strErr;
			return -2;
		}
		// �ұ�������Ϣ
		AVCodec* codec = avcodec_find_decoder(m_pCodecContext->codec_id);
		if (!codec)
		{
			av_strerror(ret, m_strErr, sizeof(m_strErr));
			LOG(INFO) << "avcodec_find_decoder failed, errCode: " << ret << " errStr: " << m_strErr;
			return -1;
		}
		ret = avcodec_open2(m_pCodecContext, codec, NULL);
		if (ret < 0)
		{
			av_strerror(ret, m_strErr, sizeof(m_strErr));
			LOG(INFO) << "avcodec_open2 failed, errCode: " << ret << " errStr: " << m_strErr;
			return -2;
		}
		LOG(INFO) << "init Decondec thread success.";
		return 0;
	}
}