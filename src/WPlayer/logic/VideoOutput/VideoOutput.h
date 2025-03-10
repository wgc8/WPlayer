#pragma once
#include <QObject>
#ifdef __cplusplus
extern "C"
{
// 包含ffmpeg头文件
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
		 * @ brief: 通过视频首帧设置参数.
		 * @return: void
		 */
		int initFrameParam();

		/**
		 * @ fn: init
		 * @ brief: 初始化视频播放参数.
		 * @ param: videoParams
		 * @return: int
		 */
		int init(AVCodecParameters* videoParams, const AVRational& timebase);
		/**
		 * @ fn: readFrame
		 * @ brief: 读取视频帧.
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

		AVCodecParameters* m_pVideoParams{ nullptr };				// 解码器参数
		SyncClock* m_pClock{ nullptr };								// 音画同步时钟
		AVRational m_timebase;										// 视频时间基
	};

}

