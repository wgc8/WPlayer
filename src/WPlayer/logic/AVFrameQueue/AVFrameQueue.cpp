#include "AVFrameQueue.h"
#include "module/Log/easylogging++.h"

#ifdef __cplusplus
extern "C"
{
	// 包含ffmpeg头文件
#include "libavcodec/avcodec.h"
}
#endif // __cplusplus

namespace wplayer {
	AVFrameQueue::AVFrameQueue()
	{
	}

	AVFrameQueue::~AVFrameQueue()
	{
		// TODO
		abort();
	}

	void AVFrameQueue::abort()
	{
		release();
		m_que.abort();
	}

	int AVFrameQueue::push(AVFrame* pkt)
	{
		AVFrame* tmpFrm = av_frame_alloc();
		av_frame_move_ref(tmpFrm, pkt);
		return m_que.push(tmpFrm);
	}

	std::shared_ptr<AVFrame> AVFrameQueue::waitAndPop(const int timeout)
	{
		AVFrame* tmpFrm = nullptr;
		int ret = m_que.waitAndPop(tmpFrm, timeout);
		if (ret < 0)
		{
			LOG(ERROR) << "AVFrameQueue pop failed, ret: " << ret;
		}
		return std::make_shared<AVFrame>(*tmpFrm);
	}

	std::shared_ptr<AVFrame> AVFrameQueue::front()
	{
		return nullptr;
	}

	void AVFrameQueue::release()
	{
		while (true)
		{
			AVFrame* frame = nullptr;
			int ret = m_que.waitAndPop(frame, 1);
			if (ret < 0)
			{
				break;
			}
			else
			{
				av_frame_free(&frame);
			}
		}
	}
}
