/*****************************************************************//**
 * @ file   DecodeThread.cpp
 * @ brief  解码器线程
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
	// 包含ffmpeg头文件
#include "libavutil/avutil.h"
#include "libavCodec/avcodec.h"

}
#endif // __cplusplus

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

	// 解码线程执行函数
	void DecodeThread::run()
	{
		LOG(INFO) << "start Decodec Thread.";
		AVFrame* tmpFrm = av_frame_alloc();
		// 读取包队列
		for (;;)
		{
			// 退出线程
			if (m_bStop)
			{
				LOG(INFO) << "quit thread then, bStop: " << m_bStop;
				break;
			}
			auto pSharedPkt = m_quePacket->waitAndPop(10);
			if (!pSharedPkt)
			{
				LOG(INFO) << "not get packet";
				continue;
			}
			auto pPkt = pSharedPkt.get();
			int ret = avcodec_send_packet(m_pCodecContext, pPkt);
			av_packet_free(&pPkt);
			if (ret < 0)
			{
				av_strerror(ret, m_strErr, sizeof(m_strErr));
				LOG(INFO) << "avcodec_send_packet failed, errCode: " << ret << " errStr: " << m_strErr;

			}
			// 获取包解码后的帧
			for (;;)
			{
				ret = avcodec_receive_frame(m_pCodecContext, tmpFrm);
				if (0 == ret)
				{
					m_queFrame->push(tmpFrm);
					//LOG(INFO) << "que size" << m_queFrame->size();
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
		// 释放临时帧对象
		av_frame_free(&tmpFrm);
		LOG(INFO) << "exit Decode Thread.";
	}

	// 初始化解码器上下文
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
		// 将解码器参数适配到上下文中
		int ret = avcodec_parameters_to_context(m_pCodecContext, condecParams);
		if (ret < 0)
		{
			av_strerror(ret, m_strErr, sizeof(m_strErr));
			LOG(INFO) << "avcodec_parameters_to_context failed, errCode: " << ret << " errStr: " << m_strErr;
			return -2;
		}
		// 找编码器信息
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