/*****************************************************************//**
 * @ file   PlayController.h
 * @ brief  ���ſ�����
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
		 * @ brief: ��ʼ��������Դ.
		 * @return: int
		 */
		int init();
	signals:
		void signalUpdateProgress(const int64_t& curDur, const int64_t& dur);

	public slots:
		/**
		 * @ fn: play
		 * @ brief: ����.
		 * @return: void
		 */
		void play();

		/**
		 * @ fn: pause
		 * @ brief: ��ͣ.
		 * @return: void
		 */
		void pause();

		/**
		 * @ fn: seek
		 * @ brief: ��תseek.
		 * @ param: posTs
		 * @return: void
		 */
		void seek(const int64_t posTs);

		/**
		 * @ fn: stop
		 * @ brief: ֹͣ.
		 * @return: void
		 */
		void stop();

		/**
		 * @ fn: setFileName
		 * @ brief: ���ò����ļ���.
		 * @ param: name
		 * @return: void
		 */
		void setFileName(const QString& name);

		/**
		 * @ fn: getGLWidget
		 * @ brief: ��ȡGL����.
		 * @return: QWidget *
		 */
		QWidget* getGLWidget();

		/**
		 * @ fn: setGLWidget
		 * @ brief: ����GL����.
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
		PlayControlStatus m_ePlayStatue{ PCS_UNINIT };		// ����״̬
		QString m_strFileName;								// �����ļ���
		int64_t m_iDuration{ 0 };							// ����ʱ��

		VideoOutput* m_pVideoOutput{ nullptr };             // ��Ƶ���
		AudioOutput* m_pAudioOutput{ nullptr };             // ��Ƶ���
		AVPacketQueue m_aPacketQue, m_vPacketQue;           // ����Ƶ������
		AVFrameQueue m_aFrameQue, m_vFrameQue;              // ����Ƶ֡����
		DemuxThread* m_pDemuxThread{ nullptr };             // ���װ�߳�
		DecodeThread* m_pVideoDecodeThread{ nullptr };      // ��Ƶ�����߳�
		DecodeThread* m_pAudioDecodeThread{ nullptr };      // ��Ƶ�����߳�
		SyncClock* m_pClock{ nullptr };						// ����ͬ��ʱ��
		QTimer* m_timerDur{ nullptr };						// ���²���ʱ����ʱ��
	};
}