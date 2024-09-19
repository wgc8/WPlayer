#pragma once
#include "stdint.h"
#include <QThread>
#include "logic/PlayController/PlayControlState.h"
#include <atomic>
#ifdef __cplusplus
extern "C"
{
	// ����ffmpegͷ�ļ�
#include "libavutil/samplefmt.h"
}
#endif // __cplusplus

class QAudioOutput;
class QAudioFormat;
class QAudioDeviceInfo;
class QIODevice;
class AVCodecParameters;
class SwrContext;
namespace wplayer
{
	class AVFrameQueue;
	struct AudioParams
	{
		int sampleRate;	// ������
		int channels;	// �ŵ���
		int frameSize;	// һ֡��������
		int64_t channelLayout;	// �ŵ�����
		AVSampleFormat sampleFormat;	// ������ʽ
	};

	class AudioOutput :public QThread
	{
		Q_OBJECT
	public:
		AudioOutput(AVFrameQueue* audioFrameQue);
		~AudioOutput();

		void run();
		int uninit();
		int init(AVCodecParameters* audioParms);
		void setVolume(double value);
		int seek();
		void play();
		void stop();
		void pause();
		int playControl(PlayControlStatus state);

		int64_t getAudioPts();
	private:
		void playLoop();
		int initOutput(AVCodecParameters* audioParms);
		int initDstAudioParams();
		int initSrcAudioParams(AVCodecParameters* audioParms);
		// ��ʼ����Ƶ�ز�������
		int initSwr();

	private:
		// ������Ϣ
		char m_strErr[256] = { 0 };
		// AudioBuffͷָ��
		uint8_t* m_pBuf{ nullptr };
		// buf��size
		uint32_t m_iBufSize = 0;
		// buf�Ѷ�ȡλ��
		uint8_t* m_pBufIdx{ nullptr };

		PlayControlStatus m_eStatus{ PCS_UNINIT };
		AudioParams m_struSrcAudioParams;
		AudioParams m_struDstAudioParams;
		AVFrameQueue* m_pQueAudioFrame{ nullptr };
		QAudioOutput* m_pAudioOutput{ nullptr };
		QIODevice* m_pAudioDevice{ nullptr };
		// ����������
		AVCodecParameters* m_pAudioParams{ nullptr };
		// �ز�����
		SwrContext* m_pSwrCxt{ nullptr };

		std::atomic<int64_t> m_aiCur{ 0 };
	};

}

