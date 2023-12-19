#include "GlobalDefine.h"
#include <QString>
#include <QDateTime>
#include <QLocale>


namespace wplayer
{
    // 日志开头
    const char* strIdentigication
    {
        "\n                                ********************************************                                  \n"
        "                                ********************************************                                  \n"
        "                                **  **    START RUNNING PLAYER    **  **                                  \n"
        "                                ********************************************                                  \n"
        "                                ********************************************                                  \n"
    };
   
    QString buildDateTime()
    {
        QString dateTime;
        dateTime += __DATE__;
        dateTime += __TIME__;
        dateTime.replace("  ", " 0");
        QDateTime buildTime = QLocale(QLocale::English).toDateTime(dateTime, "MMM dd yyyyhh:mm:ss");
        return "V1.0.0Build" + buildTime.toString("yyMMddhhmm");
    }

    const QString softwareVersion = buildDateTime();
}
