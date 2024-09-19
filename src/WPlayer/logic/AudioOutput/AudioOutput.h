#pragma once
#include "stdint.h"
#include <QThread>
#include "logic/PlayController/PlayControlState.h"
#include <atomic>
#ifdef __cplusplus
extern "C"
{
	// 包含ffmpeg头文件
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
		int sampleRate;	// 采样率
		int channels;	// 信道数
		int frameSize;	// 一帧的样本数
		int64_t channelLayout;	// 信道布局
		AVSampleFormat sampleFormat;	// 采样格式
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
		// 初始化音频重采样器；
		int initSwr();

	private:
		// 报错信息
		char m_strErr[256] = { 0 };
		// AudioBuff头指针
		uint8_t* m_pBuf{ nullptr };
		// buf的size
		uint32_t m_iBufSize = 0;
		// buf已读取位置
		uint8_t* m_pBufIdx{ nullptr };

		PlayControlStatus m_eStatus{ PCS_UNINIT };
		AudioParams m_struSrcAudioParams;
		AudioParams m_struDstAudioParams;
		AVFrameQueue* m_pQueAudioFrame{ nullptr };
		QAudioOutput* m_pAudioOutput{ nullptr };
		QIODevice* m_pAudioDevice{ nullptr };
		// 解码器参数
		AVCodecParameters* m_pAudioParams{ nullptr };
		// 重采样器
		SwrContext* m_pSwrCxt{ nullptr };

		std::atomic<int64_t> m_aiCur{ 0 };
	};

}

