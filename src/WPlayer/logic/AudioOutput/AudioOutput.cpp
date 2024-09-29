#include "AudioOutput.h"
#include "module/Log/easylogging++.h"
#include "logic/AVFrameQueue/AVFrameQueue.h"
#include <QAudioOutput>
#include <QAudioFormat>
#include <QAudioDeviceInfo>
#ifdef __cplusplus
extern "C"
{
// 包含ffmpeg头文件
#include "libavcodec/avcodec.h"
#include "libswresample/swresample.h"
}
#endif // __cplusplus


namespace wplayer
{
	AudioOutput::AudioOutput(AVFrameQueue* audioFrameQue, SyncClock* clock):
		m_pQueAudioFrame(audioFrameQue),
		m_pClock(clock)
	{
		
	}
	AudioOutput::~AudioOutput()
	{
		// TODO线程没退出，会崩溃
		swr_free(&m_pSwrCxt);
		av_free(m_pBuf);
	}

	// 音频播放子线程执行函数
	void AudioOutput::run()
	{
		for (;;)
		{
			switch (m_eStatus)
			{
			case PCS_UNINIT: {
				LOG(ERROR) << "audio output is not inited or error.";
				return;
			}
			case PCS_IDLE: {
				msleep(100);
				break;
			}
			case PCS_PLAYING:
				playLoop();
				break;
			}
			// TODO 补充后续状态
			msleep(100);
		}
	}

	int AudioOutput::initOutput(AVCodecParameters* audioParms)
	{
		if (!audioParms)
		{
			LOG(ERROR) << "audioParms is nullptr";
			return -1;
		}
		int sampleSize = 8 * av_get_bytes_per_sample((AVSampleFormat)audioParms->format);
		LOG(INFO) << "open audio info, sample rate" << audioParms->sample_rate << " channel count: " << audioParms->channels << " sample size: " << sampleSize;
		QAudioFormat audioFormmat;
		audioFormmat.setSampleRate(m_struDstAudioParams.sampleRate);
		audioFormmat.setChannelCount(m_struDstAudioParams.channels);
		audioFormmat.setSampleSize(16);
		audioFormmat.setSampleType(QAudioFormat::SignedInt);
		audioFormmat.setByteOrder(QAudioFormat::LittleEndian); //次序

		audioFormmat.setCodec("audio/pcm");
		QAudioDeviceInfo deviceInfo(QAudioDeviceInfo::defaultOutputDevice());
		if (!m_pAudioOutput)
		{
			m_pAudioOutput = new QAudioOutput(deviceInfo, audioFormmat, this);
			LOG(INFO) << "init audio output";
		}
		// TODO 先使用默认方式
		//m_pAudioOutput->setBufferSize()
		m_pAudioDevice = m_pAudioOutput->start();
		m_eStatus = PCS_IDLE;
		return 0;
	}
	int AudioOutput::uninit()
	{
		return 0;
	}
	// 初始化音频输出模线程
	int AudioOutput::init(AVCodecParameters* audioParms, const AVRational& timebase)
	{
		if (!audioParms)
		{
			LOG(INFO) << "audioParams is nullptr";
			return -1;
		}
		m_timebase = timebase;
		m_pAudioParams = audioParms;
		int ret = 0;
		ret = initDstAudioParams();
		if (ret < 0)
		{
			LOG(ERROR) << "initDstAudioParams failed, ret: " << ret;
			return ret;
		}
		ret = initSrcAudioParams(audioParms);
		if (ret < 0)
		{
			LOG(ERROR) << "initSrcAudioParams failed, ret: " << ret;
			return ret;
		}
		ret = initOutput(audioParms);
		if (ret < 0)
		{
			LOG(ERROR) << "initOutput failed, ret: " << ret;
			return ret;
		}
		ret = initSwr();
		if (ret < 0)
		{
			LOG(ERROR) << "initSwr failed, ret: " << ret;
			return ret;
		}
		return 0;
	}
	
	// 设置音量
	void AudioOutput::setVolume(double value)
	{
		if (m_pAudioOutput)
		{
			m_pAudioOutput->setVolume(value);
		}
	}

	void AudioOutput::play()
	{
		if (!isRunning())
		{
			start();
		}
		m_eStatus = PCS_PLAYING;
	}

	void AudioOutput::stop()
	{
	}

	/**
	 * @ fn: playLoop
	 * @ brief: 播放循环主体.
	 * @return: void
	 */
	void AudioOutput::playLoop()
	{
		while (m_eStatus == PCS_PLAYING)
		{
			AVFrame* frame = m_pQueAudioFrame->waitAndPop(10);
			if (frame)
			{
				//uint8_t** in = (uint8_t**)frame->extended_buf;
				uint8_t* data[2] = { 0 };
				// 重采样后输出一帧有多少样本
				int outSamples = av_rescale_rnd(frame->nb_samples,
					m_struDstAudioParams.sampleRate, m_struSrcAudioParams.sampleRate, AV_ROUND_UP);
				
				int outBytes = av_samples_get_buffer_size(NULL, 
					m_struDstAudioParams.channels, 
					outSamples, 
					m_struDstAudioParams.sampleFormat, 0);	// 获取一帧数据缓冲区大小
				if (outBytes < 0)
				{
					av_strerror(outBytes, m_strErr, sizeof(m_strErr));
					LOG(INFO) << "av_samples_get_buffer_size, errCode: " << outBytes << " errStr: " << m_strErr;
					return;
				}
				// 分配重采样输出的一帧中所有数据的buf内存
				av_fast_malloc(&m_pBuf, &m_iBufSize, outBytes);
				if (!m_pBuf)
				{
					LOG(ERROR) << "m_pBuf is nullptr";
					return;
				}
				// fixed const
				const uint8_t** in =(const uint8_t**)frame->data;
				int len = swr_convert(m_pSwrCxt, &m_pBuf, outBytes, in, frame->nb_samples);
				if (len < 0)
				{
					av_strerror(len, m_strErr, sizeof(m_strErr));
					LOG(INFO) << "swr_convert failed, errCode: " << len << " errStr: " << m_strErr;
					return;
				}
				int tmp = outBytes;
#if 1			// 只要存在空间就往声卡写，直到帧数据写完
				while (tmp > 0)
				{
					uint8_t* writePos = m_pBuf;
					int freeBytes = m_pAudioOutput->bytesFree();
					int writeBytes = min(freeBytes, tmp);
					m_pAudioDevice->write((const char*)writePos, writeBytes);
					writePos += writeBytes;
					tmp -= writeBytes;
					msleep(10);
				}
#else			// 有足够空间再整体写入
				while (tmp > m_pAudioOutput->bytesFree())
				{
					msleep(10);
				}
				m_pAudioDevice->write((const char*)m_pBuf, tmp);
#endif
				av_frame_free(&frame);
			}
			else
			{
				LOG(ERROR) << "not get audio frame.";
				return;
			}
			//msleep(10);
		}
	}

	/**
	 * @ fn: initDstAudioParams
	 * @ brief: 初始化输出音频参数.
	 * @return: int
	 */
	int AudioOutput::initDstAudioParams()
	{
		// 先设置默认值，以后考虑从媒体文件中解析
		m_struDstAudioParams.channels = 2;
		m_struDstAudioParams.channelLayout = av_get_default_channel_layout(m_struDstAudioParams.channels);
		m_struDstAudioParams.frameSize = 1024;	// 默认一帧1024个样本
		m_struDstAudioParams.sampleRate = 48000;
		m_struDstAudioParams.sampleFormat = AV_SAMPLE_FMT_S16;
		return 0;
	}

	// 初始化输入音频参数
	int AudioOutput::initSrcAudioParams(AVCodecParameters* audioParms)
	{
		if (!audioParms)
		{
			LOG(ERROR) << "audioParms is nullptr";
			return -1;
		}
		int sampleSize = 8 * av_get_bytes_per_sample((AVSampleFormat)audioParms->format);
		// 读取音频码流格式信息
		m_struSrcAudioParams.channels = audioParms->channels;
		m_struSrcAudioParams.channelLayout = audioParms->channel_layout;
		m_struSrcAudioParams.frameSize = audioParms->frame_size;
		m_struSrcAudioParams.sampleRate = audioParms->sample_rate;
		m_struSrcAudioParams.sampleFormat = (AVSampleFormat)audioParms->format;
		LOG(INFO) << "open audio info, sample rate" << audioParms->sample_rate << " channel count: " << audioParms->channels << " sample size: " << sampleSize;
		return 0;
	}

	// 初始化重采样器
	int AudioOutput::initSwr()
	{
		// 分配重采样器上下文
		m_pSwrCxt = swr_alloc_set_opts(NULL,
			m_struDstAudioParams.channelLayout,
			m_struDstAudioParams.sampleFormat,
			m_struDstAudioParams.sampleRate,
			m_struSrcAudioParams.channelLayout,
			m_struSrcAudioParams.sampleFormat,
			m_struSrcAudioParams.sampleRate,NULL,
			NULL);
		if (!m_pSwrCxt)
		{
			LOG(ERROR) << "swr_alloc_set_opts failed.";
			return -1;
		}
		int ret = swr_init(m_pSwrCxt);
		if (ret < 0)
		{
			swr_free(&m_pSwrCxt);
			av_strerror(ret, m_strErr, sizeof(m_strErr));
			LOG(INFO) << "swr_init failed, errCode: " << ret << " errStr: " << m_strErr;
			return -2;
		}
		return 0;
	}

	// 播放控制
	int AudioOutput::playControl(PlayControlStatus state)
	{
		if (state < PCS_PLAYING)
		{
			LOG(ERROR) << "illegal play state, state: " << state;
			return -1;
		}
		m_eStatus = state;
		return 0;
	}
}
