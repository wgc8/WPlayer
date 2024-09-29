#pragma once
#include <QObject>
#ifdef __cplusplus
extern "C"
{
// ����ffmpegͷ�ļ�
//#include "libavutil/time.h"
#include "libavutil/rational.h"
}
#endif // __cplusplus

class GLRenderWidget;
class AVFrame;
class QWidget;
class QTimer;
class AVCodecParameters;
namespace wplayer
{
	class SyncClock;
	class AVFrameQueue;
	class VideoOutput :public QObject
	{
		Q_OBJECT
	public:
		VideoOutput(AVFrameQueue* videoFrameQue, SyncClock* clock);
		~VideoOutput();

		QWidget* getGLWidget();
		
		void initConnect();

		void setGLWidget(GLRenderWidget* p);

		/**
		 * @ fn: setFrame
		 * @ brief: ͨ����Ƶ��֡���ò���.
		 * @return: void
		 */
		int initFrameParam();

		/**
		 * @ fn: init
		 * @ brief: ��ʼ����Ƶ���Ų���.
		 * @ param: videoParams
		 * @return: int
		 */
		int init(AVCodecParameters* videoParams, const AVRational& timebase);
		/**
		 * @ fn: readFrame
		 * @ brief: ��ȡ��Ƶ֡.
		 * @return: void
		 */
		void readFrame();

		void play();

		void pause();

		void seek();

		void stop();
	private:
		bool m_bFirstFrame{ false };
		AVFrameQueue* m_pFrameQue{ nullptr };
		GLRenderWidget* m_glWidget{ nullptr };
		uint m_iWidth{ 0 };
		uint m_iHeight{ 0 };
		QTimer* m_timer{ nullptr };

		AVCodecParameters* m_pVideoParams{ nullptr };				// ����������
		SyncClock* m_pClock{ nullptr };								// ����ͬ��ʱ��
		AVRational m_timebase;										// ��Ƶʱ���
	};

}

