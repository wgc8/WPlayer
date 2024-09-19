#include "VideoOutput.h"
#include "module/GLWidget/GLRenderWidget.h"
#include "module/Log/easylogging++.h"
#include "logic/AVFrameQueue/AVFrameQueue.h"
#include <QTimer>
#ifdef __cplusplus
extern "C"
{
// ����ffmpegͷ�ļ�
#include "libavutil/avutil.h"
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
}
#endif

namespace wplayer
{
	VideoOutput::VideoOutput(AVFrameQueue* videoFrameQue):
		m_pFrameQue(videoFrameQue),
		m_timer(new QTimer(this))
	{
		m_glWidget = new GLRenderWidget(nullptr);
		if (!m_glWidget)
		{
			LOG(ERROR) << "new glWidget fail!";
			return;
		}
		initConnect();
		// ���û���������
		m_timer->setInterval(20);
	}

	VideoOutput::~VideoOutput()
	{
		// glWidget�ĳ��з���ui�У��˴������ٹ��������ͷ�
		//if (m_glWidget)
		//{
		//	delete m_glWidget;
		//	m_glWidget = nullptr;
		//}
	}

	QWidget* VideoOutput::getGLWidget()
	{
		return m_glWidget;
	}

	void VideoOutput::initConnect()
	{
		// ����ʱ���ö�ʱ���ķ�ʽ������Ƶ�����δ��������ͬ��
		if (m_timer)
		{
			connect(m_timer, &QTimer::timeout, this, &VideoOutput::readFrame);
		}
	}

	void VideoOutput::setGLWidget(GLRenderWidget* p)
	{
		m_glWidget = p;
	}

	int VideoOutput::initFrameParam()
	{
		AVFrame* frame = m_pFrameQue->front();
		if (!frame)
		{
#ifdef DEBUG_MODE
			LOG(ERROR) << "not get video frame.";
#endif // DEBUG_MODE
			return -1;
		}
		if (frame->format != AV_PIX_FMT_YUV420P)
		{
			LOG(ERROR) << "get invalid pixel format: " << (int)frame->format;
			return -2;
		}
		m_iHeight = frame->height;
		m_iWidth = frame->width;
		// Ŀǰֻ֧��yuv420
		m_glWidget->reset(m_iWidth, m_iHeight, m_iHeight * m_iHeight * 3 / 2);
	}

	// ��ʼ����Ƶ���Ų���
	int VideoOutput::init(AVCodecParameters* videoParams)
	{
		if (!videoParams)
		{
			LOG(INFO) << "VideoOut init failed, videoParams is nullptr";
			return -1;
		}
		m_pVideoParams = videoParams;
		m_iHeight = m_pVideoParams->height;
		m_iWidth = m_pVideoParams->width;
		// Ŀǰֻ֧��yuv420
		// glTextureImage2D�������ԭ�������buff����Ŀռ䲻����yuv420ֻҪ1.5����rgb��Ҫ3�����ؼ�Ҫȷ�����buff��ô���䣬��ô��
		m_glWidget->reset(m_iWidth, m_iHeight, m_iWidth * m_iHeight * 3 / 2);
		return 0;
	}

	void VideoOutput::readFrame()
	{
		AVFrame* frame = m_pFrameQue->waitAndPop(10);
		if (frame)
		{
			AVPixelFormat format =(AVPixelFormat) frame->format;
			if (m_glWidget)
			{
				int bytesY = m_iWidth * m_iHeight;
				// Y����
				m_glWidget->setFrame(0, frame->data[0], bytesY);
				// U����
				m_glWidget->setFrame(bytesY, frame->data[1], bytesY / 4);
				// V����
				m_glWidget->setFrame(bytesY * 5 / 4, frame->data[2], bytesY / 4);
				m_glWidget->update();
			}
			else
			{
				LOG(ERROR) << "m_glWidget is nullptr";
			}
			// �ͷ���Դ
			av_frame_free(&frame);
		}
		else
		{
#ifdef DEBUG_MODE
			LOG(ERROR) << "not get video frame.";
#endif // DEBUG_MODE
		}
	}

	void VideoOutput::play()
	{
		m_timer->start();
		//readFrame();
	}
}
