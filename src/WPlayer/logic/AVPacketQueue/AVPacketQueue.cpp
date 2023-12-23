#include "AVPacketQueue.h"
#include "module/Log/easylogging++.h"

#ifdef __cplusplus
extern "C"
{
	// 包含ffmpeg头文件
#include "libavcodec/avcodec.h"
}
#endif // __cplusplus

namespace wplayer {
	AVPacketQueue::AVPacketQueue()
	{
	}
	
	AVPacketQueue::~AVPacketQueue()
	{
		// TODO
		abort();
	}
	
	void AVPacketQueue::abort()
	{
		release();
		m_que.abort();
	}

	int AVPacketQueue::push(AVPacket* pkt)
	{
		AVPacket* tmpPkt = av_packet_alloc();
		av_packet_move_ref(tmpPkt, pkt);
		return m_que.push(tmpPkt);
	}
	
	AVPacket* AVPacketQueue::waitAndPop(const int timeout)
	{
		AVPacket* tmpPkt = nullptr;
		int ret = m_que.waitAndPop(tmpPkt, timeout);
		if (ret < 0)
		{
			return nullptr;
		}
		return tmpPkt;
	}

	int AVPacketQueue::size()
	{
		return m_que.getSize();
	}

	void AVPacketQueue::release()
	{
		while (true)
		{
			AVPacket* tmpPkt = nullptr;
			int ret = m_que.waitAndPop(tmpPkt, 1);
			if (ret < 0)
			{
				break;
			}
			else
			{
				av_packet_free(&tmpPkt);
			}
		}
	}
}