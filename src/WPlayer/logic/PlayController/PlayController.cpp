#include "PlayController.h"
#include "module/Log/easylogging++.h"
#include "module/GLWidget/GLRenderWidget.h"
#include "logic/DemuxThread/DemuxThread.h"
#include "logic/DecodeThread/DecodeThread.h"
#include "logic/VideoOutput/VideoOutput.h"
#include "logic/AudioOutput/AudioOutput.h"
#include "logic/SyncClock/SyncClock.h"
#include <QTimer>
#include <QFile>
#include <algorithm>
extern "C"
{
    // 包含ffmpeg头文件
#include "libavutil/avutil.h"
#include "libavformat/avformat.h"

}

namespace wplayer {
	PlayController::PlayController(QObject *parent)
		: QObject(parent)
	{
        // 初始化解封装队列
        if (initDemuxThread() < 0)
        {
            LOG(ERROR) << "init DemuxThread failed.";
            return;
        }
        // 初始化解码线程
        if (initDecodeThread() < 0)
        {
            LOG(ERROR) << "init DecodeThread failed.";
            return;
        }
        // 初始化时钟
        m_pClock = new SyncClock();

        if (!m_pClock)
        {
            LOG(ERROR) << "init clock failed";
            return;
        }
        // 初始化音视频输出
        if (initAVOutput() < 0)
        {
            LOG(ERROR) << "init AVOutput failed.";
            return;
        }
        m_ePlayStatue = PCS_IDLE;

        m_timerDur = new QTimer();
        m_timerDur->setInterval(1000);
        m_timerDur->start();
        initConnection();
    }

	PlayController::~PlayController()
	{
        if (m_pDemuxThread)
        {
            delete m_pDemuxThread;
            m_pDemuxThread = nullptr;
        }
        if (m_pAudioDecodeThread)
        {
            delete m_pAudioDecodeThread;
            m_pAudioDecodeThread = nullptr;
        }
        if (m_pVideoDecodeThread)
        {
            delete m_pVideoDecodeThread;
            m_pVideoDecodeThread = nullptr;
        }
        if (m_pAudioOutput)
        {
            delete m_pAudioOutput;
            m_pAudioOutput = nullptr;
        }
        if (m_pVideoOutput)
        {
            delete m_pVideoOutput;
            m_pVideoOutput = nullptr;
        }
	}

    // 设置播放文件名
    void PlayController::setFileName(const QString& name)
    {
        if (QFile::exists(name))
        {
            m_strFileName = name;
            init();
        }
        else
        {
            LOG(INFO) << "file not exist.";
        }
    }

    int PlayController::init()
    {
        if (m_strFileName.isEmpty())
        {
            LOG(ERROR) << "played file name is empty";
            return -1;
        }
        m_pClock->init();
        int ret;
        ret = m_pDemuxThread->init(m_strFileName.toStdString());
        if (ret < 0)
        {
            LOG(ERROR) << "DemuxThread init failed.";
            return -1;
        }
        ret = m_pAudioDecodeThread->init(m_pDemuxThread->getAudioCodecParames());
        if (ret < 0)
        {
            LOG(ERROR) << "AudioDecodeThread init failed";
            return -1;
        }
        ret = m_pVideoDecodeThread->init(m_pDemuxThread->getVideoCodecParames());
        if (ret < 0)
        {
            LOG(ERROR) << "VideoDecodeThread init failed";
            return -1;
        }
        ret = m_pAudioOutput->init(m_pDemuxThread->getAudioCodecParames(), m_pDemuxThread->getAudioTimeBase());
        if (ret < 0)
        {
            LOG(ERROR) << "AudioOutput init failed";
            return -1;
        }        
        ret = m_pVideoOutput->init(m_pDemuxThread->getVideoCodecParames(), m_pDemuxThread->getVideoTimeBase());
        if (ret < 0)
        {
            LOG(ERROR) << "AudioOutput init failed";
            return -1;
        }
        // TODO 考虑上面一串用do while(0)

        m_iDuration = m_pDemuxThread->getDuration();
        // 显示视频播放进度，四舍五入
        emit signalUpdateProgress(0, (m_iDuration + AV_TIME_BASE / 2) / AV_TIME_BASE);
        return ret;
    }

    // 暂停
    void PlayController::pause()
    {
        LOG(INFO) << "pause enter";
        if (m_pAudioOutput)
        {
            m_pAudioOutput->pause();
        }
        if (m_pVideoOutput)
        {
            m_pVideoOutput->pause();
        }
        LOG(INFO) << "pause end";
    }

    // 跳转
    void PlayController::seek()
    {
    }

    // 停止
    void PlayController::stop()
    {
        if (m_ePlayStatue != PCS_PLAYING)
        {
            LOG(ERROR) << "cannot stop, play state: " << int(m_ePlayStatue);
            return;
        }
        if (m_pDemuxThread)
        {
            m_pDemuxThread->stop();
        }
        if (m_pAudioDecodeThread)
        {
            m_pAudioDecodeThread->stop();
        }
        if (m_pVideoDecodeThread)
        {
            m_pVideoDecodeThread->stop();
        }
        m_aPacketQue.abort();
        m_vPacketQue.abort();
        m_aFrameQue.abort();
        m_vFrameQue.abort();
        m_ePlayStatue = PCS_IDLE;
    }

    // 播放
    void PlayController::play()
    {
        LOG(INFO) << "play enter";
        // TODO 传不存在的文件名进来，一点播放就崩溃
        if (m_pDemuxThread && !m_pDemuxThread->isRunning())
        {
            m_pDemuxThread->start();
        }
        if (m_pAudioDecodeThread && !m_pAudioDecodeThread->isRunning())
        {
            m_pAudioDecodeThread->start();
        }
        if (m_pVideoDecodeThread && !m_pVideoDecodeThread->isRunning())
        {
            m_pVideoDecodeThread->start();
        }
        if (m_pVideoOutput)
            m_pVideoOutput->play();
        if (m_pAudioOutput)
            m_pAudioOutput->play();
        LOG(INFO) << "play end";
    }

    // 获取GLWidget指针
    QWidget* PlayController::getGLWidget()
    {
        if (m_pVideoOutput)
        {
            return m_pVideoOutput->getGLWidget();
        }
        return nullptr;
    }

    void PlayController::setGLWidget(GLRenderWidget* pWidget)
    {
        if (pWidget)
        {
            m_pVideoOutput->setGLWidget(pWidget);
        }
    }

    void PlayController::setVolume(double v)
    {
        if (m_pAudioOutput)
        {
            m_pAudioOutput->setVolume(v);
            LOG(INFO) << "change audio volume: " << v;
        }
    }

    int PlayController::initAVOutput()
    {
        m_pAudioOutput = new AudioOutput(&m_aFrameQue, m_pClock);
        m_pVideoOutput = new VideoOutput(&m_vFrameQue, m_pClock);
        if (!m_pAudioOutput || !m_pVideoOutput)
        {
            return -1;
        }
        return 0;
    }

    void PlayController::initConnection()
    {
        connect(m_timerDur, &QTimer::timeout, [&] {
            int64_t curSecond = 0;
            if (m_pClock)
            {
                curSecond = static_cast<int64_t>(m_pClock->getPts());
            }
            //curSecond = min(0, curSecond);
            emit signalUpdateProgress(curSecond, (m_iDuration + AV_TIME_BASE / 2) / AV_TIME_BASE); });
    }

    int PlayController::initDemuxThread()
    {
        m_pDemuxThread = new DemuxThread(&m_aPacketQue, &m_vPacketQue);
        if (m_pDemuxThread)
        {
            return 0;
        }
        return -1;
    }

    int PlayController::initDecodeThread()
    {
        m_pAudioDecodeThread = new DecodeThread(&m_aPacketQue, &m_aFrameQue);
        m_pVideoDecodeThread = new DecodeThread(&m_vPacketQue, &m_vFrameQue);
        if (!m_pAudioDecodeThread || !m_pVideoDecodeThread)
        {
            return -1;
        }
        return 0;
    }
}