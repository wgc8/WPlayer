#include "WPlayerApplication.h"
#include "module/Log/easylogging++.h"
#include "module/GlobalDefine/GlobalDefine.h"
#include <QDir>
#include <QDateTime>

#define MAX_LOG_FILE_NUMBER 10

static unsigned int logNum; // ��ǰ�����Ѽ�¼��־�ļ�����

void rolloutHandler(const char* filename, size_t size)
{
    // ����־�ļ���������
    QString logFileName(filename);
    QString backLogFileName = logFileName.left(logFileName.lastIndexOf(".log")) + "_backup" + QString::number(logNum++) + ".log";
    QFile logFile(logFileName);
    if (QFile::exists(backLogFileName))
    {
        // ������ɾ�����ٸ�д
        if (!QFile::remove(backLogFileName))
        {
            LOG(WARNING) << "remove old log file failed.";
        }
    }
    // д������־�ļ����б���
    if (!logFile.rename(backLogFileName))
    {
        LOG(WARNING) << "backup log file fail.";
    }
}

namespace wplayer
{

	WPlayerApplication::WPlayerApplication(int argc, char* argv[]):
		QApplication(argc, argv)
	{
        initLog();
        LOG(INFO) << "current software version: " << softwareVersion;
    }

	WPlayerApplication::~WPlayerApplication()
	{}

    void WPlayerApplication::setLogConfig()
    {
        QString logPath = QApplication::applicationDirPath() + "/./Log";

        QDir logDir(logPath);
        if (!logDir.exists())
        {
            logDir.mkpath(logPath);
        }
        QString logFileName = logPath + "/Log_%datetime{%Y%M%d%H%m%s}.log";
        el::Configurations defaultConf;
        defaultConf.setToDefault();
        defaultConf.set(el::Level::Global, el::ConfigurationType::Filename, logFileName.toLocal8Bit().data());
        defaultConf.set(el::Level::Global, el::ConfigurationType::MaxLogFileSize, "10485760");      // 10M
        defaultConf.set(el::Level::Global, el::ConfigurationType::Format, "[%datetime][%level] %msg {%fbase:%line:%func}");
        el::Loggers::reconfigureLogger("default", defaultConf);
        // ������־size���
        el::Loggers::addFlag(el::LoggingFlag::StrictLogFileSizeCheck);
        // ע����־�����ص�����
        el::Helpers::installPreRollOutCallback(rolloutHandler);
    }

    void WPlayerApplication::initLog()
    {
        setLogConfig();
        // ��־�������
        checkLogFileNumber();
        LOG(INFO) << strIdentigication;
        LOG(INFO) << "START DATETIME:" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toStdString();
    }

    void WPlayerApplication::checkLogFileNumber()
    {
        QString logPath = QApplication::applicationDirPath() + "/./Log";
        QDir logDir(logPath);
        if (logDir.exists())
        {
            // ��ȡ�ļ����µ��ļ��б�
            QList<QFileInfo> fileInfoList = logDir.entryInfoList(QDir::Files);
            if (fileInfoList.count() > MAX_LOG_FILE_NUMBER)
            {
                // ��������޸�ʱ����ļ���Ϣ��������
                std::sort(fileInfoList.begin(), fileInfoList.end(), [](const QFileInfo& a, const QFileInfo& b) {
                    return a.lastModified() < b.lastModified();
                    });

                // ɾ�������޸�ʱ����ļ�
                for (int i = 0; i < fileInfoList.count() - MAX_LOG_FILE_NUMBER; ++i) {
                    QString filePath = fileInfoList[i].absoluteFilePath();
                    if (QFile::remove(filePath)) {
                        LOG(INFO) << "Deleted file: " << fileInfoList[i].baseName();
                    }
                    else {
                        LOG(WARNING) << "Failed to delete file: " << fileInfoList[i].baseName();
                    }
                }
            }
        }
        else {
            LOG(WARNING) << "log dir does exist.";
        }
    }
}