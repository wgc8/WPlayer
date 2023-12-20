#include "ui/WPlayer.h"
#include "module/Application/WPlayerApplication.h"
#include "module/Log/easylogging++.h"
#include "logic/DemuxThread/DemuxThread.h"
#include "logic/AVPacketQueue/AVPacketQueue.h"
#include "logic/AVFrameQueue/AVFrameQueue.h"
#include "logic/DecodeThread/DecodeThread.h"

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

#if 1   // 调试
    AVPacketQueue aPacketQue, vPacketQue;
    AVFrameQueue aFrameQue, vFrameQue;
    DemuxThread* tmpDemux = new DemuxThread(&aPacketQue, &vPacketQue);
    tmpDemux->init("test.mp4");
    tmpDemux->start();

    //DecodeThread* pVideoDecodec = new DecodeThread(&vPacketQue, &vFrameQue);
    DecodeThread* pAudioDecodec = new DecodeThread(&aPacketQue, &aFrameQue);

    int ret = pAudioDecodec->init(tmpDemux->getAudioCodecParmes());
    if (ret < 0)
    {
        LOG(INFO) << "pAudioDecodec init failed";
    }
    pAudioDecodec->start();


    Sleep(2000);
    tmpDemux->stop();
    pAudioDecodec->stop();

    delete tmpDemux;
    delete pAudioDecodec;
#endif  // end 调试

    WPlayer w;
    w.show();

    return a.exec();
}
