#include "ui/WPlayer.h"
#include "module/Application/WPlayerApplication.h"
#include "module/Log/easylogging++.h"

#include "logic/AudioOutput/AudioOutput.h"

#ifdef __cplusplus
extern "C"
{
     // 包含ffmpeg头文件
#include "libavutil/avutil.h"
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
}
#endif

INITIALIZE_EASYLOGGINGPP
using namespace wplayer;
int main(int argc, char *argv[])
{
    //av_register_all();

    WPlayerApplication a(argc, argv);
    //LOG(INFO) << av_version_info();
    LOG(INFO) << "run path: " << argv[0];

#if 0   // 调试
    AVPacketQueue m_aPacketQue, m_vPacketQue;
    AVFrameQueue m_aFrameQue, m_vFrameQue;
    DemuxThread* tmpDemux = new DemuxThread(&m_aPacketQue, &m_vPacketQue);
    tmpDemux->init("02.mp4");
    tmpDemux->start();

    DecodeThread* pAudioDecodec = new DecodeThread(&m_aPacketQue, &m_aFrameQue);
    DecodeThread* pVideoDecodec = new DecodeThread(&m_vPacketQue, &m_vFrameQue);
    int ret = 0;
    ret = pAudioDecodec->init(tmpDemux->getAudioCodecParames());
    if (ret < 0)
    {
        LOG(INFO) << "pAudioDecodec init failed";
    }
    ret = pVideoDecodec->init(tmpDemux->getVideoCodecParames());
    if (ret < 0)
    {
        LOG(INFO) << "pVideoDecodec init failed";
    }
    ret = pAudioDecodec->start();
    if (0 == ret)
    {
        LOG(INFO) << "start pAudioDecodec success";
    }
    ret = pVideoDecodec->start();
    if (0 == ret)
    {
        LOG(INFO) << "start pVideoDecodec success";
    }
    auto pAudioOutput = new AudioOutput(&m_aFrameQue);

    pAudioOutput->init(tmpDemux->getAudioCodecParames());
    pAudioOutput->start();
    pAudioOutput->playControl(PCS_PLAYING);

    Sleep(5000);
    tmpDemux->stop();
    pAudioDecodec->stop();
    pVideoDecodec->stop();

    delete tmpDemux;
    delete pAudioDecodec;
    delete pVideoDecodec;

#endif  // end 调试

    WPlayer w;
    w.init();
    w.show();

    return a.exec();
}
