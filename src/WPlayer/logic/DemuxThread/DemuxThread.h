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

class AVFormatContext;
namespace wplayer
{
	class AVPacketQueue;

	class DemuxThread: public ThreadBase
	{
	public:
		//DemuxThread() = delete;
		DemuxThread(AVPacketQueue* audioQue, AVPacketQueue* videoQue);
		~DemuxThread();
		int init(const std::string url);
		int stop();
		virtual void run() override;
	private:
		// ��Ƶ��id
		int m_iVideoStreamIdx = -1;
		// ��Ƶ��id
		int m_iAudioStreamIdx = -1;
		// ������Ϣ
		char m_strErr[256] = { 0 };
		// ���ļ���
		std::string m_strFileUrl;
		// ��Ƶ֡����
		AVPacketQueue* m_videoPktQue{ nullptr };
		// ��Ƶ֡����
		AVPacketQueue* m_audioPktQue{ nullptr };
		// ��װ������
		AVFormatContext* m_pFormatContext{ nullptr };
	};
}
