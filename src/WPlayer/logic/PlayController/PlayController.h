/*****************************************************************//**
 * @ file   PlayController.h
 * @ brief  播放控制类
 * 
 * @ author wuguochang
 * @ date   2024/5/13
 * @ note History record:
 * 
 *********************************************************************/
#pragma once

#include <QObject>
#include "module/Singleton.h"
#include "logic/PlayController/PlayControlState.h"
#include "logic/AVPacketQueue/AVPacketQueue.h"
#include "logic/AVFrameQueue/AVFrameQueue.h"
class GLRenderWidget;

namespace wplayer {
	class VideoOutput;
	class AudioOutput;
	class DemuxThread;
	class DecodeThread;
	class SyncClock;

	class PlayController : public QObject, public Singleton<PlayController>
	{
		Q_OBJECT;
		SINGLETON_CONFIG(PlayController)
	public:
		/**
		 * @ fn: init
		 * @ brief: 初始化播放资源.
		 * @return: int
		 */
		int init();
	signals:
		void signalUpdateProgress(const int64_t& curDur, const int64_t& dur);

	public slots:
		/**
		 * @ fn: play
		 * @ brief: 播放.
		 * @return: void
		 */
		void play();

		/**
		 * @ fn: pause
		 * @ brief: 暂停.
		 * @return: void
		 */
		void pause();

		/**
		 * @ fn: seek
		 * @ brief: 跳转seek.
		 * @ param: posTs
		 * @return: void
		 */
		void seek(const int64_t posTs);

		/**
		 * @ fn: stop
		 * @ brief: 停止.
		 * @return: void
		 */
		void stop();

		/**
		 * @ fn: setFileName
		 * @ brief: 设置播放文件名.
		 * @ param: name
		 * @return: void
		 */
		void setFileName(const QString& name);

		/**
		 * @ fn: getGLWidget
		 * @ brief: 获取GL窗口.
		 * @return: QWidget *
		 */
		QWidget* getGLWidget();

		/**
		 * @ fn: setGLWidget
		 * @ brief: 设置GL窗口.
		 * @ param: pWidget
		 * @return: void
		 */
		void setGLWidget(GLRenderWidget* pWidget);

		void setVolume(double v);
	private:
		int initAVOutput();
		void initConnection();
		int initDemuxThread();
		int initDecodeThread();

	private:
		PlayControlStatus m_ePlayStatue{ PCS_UNINIT };		// 播放状态
		QString m_strFileName;								// 播放文件名
		int64_t m_iDuration{ 0 };							// 播放时长

		VideoOutput* m_pVideoOutput{ nullptr };             // 视频输出
		AudioOutput* m_pAudioOutput{ nullptr };             // 音频输出
		AVPacketQueue m_aPacketQue, m_vPacketQue;           // 音视频包队列
		AVFrameQueue m_aFrameQue, m_vFrameQue;              // 音视频帧队列
		DemuxThread* m_pDemuxThread{ nullptr };             // 解封装线程
		DecodeThread* m_pVideoDecodeThread{ nullptr };      // 视频解码线程
		DecodeThread* m_pAudioDecodeThread{ nullptr };      // 音频解码线程
		SyncClock* m_pClock{ nullptr };						// 音画同步时钟
		QTimer* m_timerDur{ nullptr };						// 更新播放时长定时器
	};
}