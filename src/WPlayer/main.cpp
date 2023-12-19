#include "ui/WPlayer.h"
#include "module/Application/WPlayerApplication.h"
#include "module/Log/easylogging++.h"
#include "logic/DemuxThread/DemuxThread.h"
#include "logic/AVPacketQueue/AVPacketQueue.h"
//#ifdef __cplusplus
//extern "C"
//{
//     // 包含ffmpeg头文件
//#include "libavutil/avutil.h"
//#include "libavformat/avformat.h"
//}
//#endif

INITIALIZE_EASYLOGGINGPP
using namespace wplayer;
int main(int argc, char *argv[])
{
    //av_register_all();

    WPlayerApplication a(argc, argv);
    //LOG(INFO) << av_version_info();
    LOG(INFO) << "run path: " << argv[0];

    // 调试
    AVPacketQueue aQue, vQue;

    DemuxThread* tmpDemux = new DemuxThread(&aQue, &vQue);
    tmpDemux->init("test.mp4");
    tmpDemux->start();

    Sleep(2000);
    tmpDemux->stop();

    // end 调试

    WPlayer w;
    w.show();

    return a.exec();
}
