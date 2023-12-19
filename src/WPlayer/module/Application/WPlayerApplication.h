#pragma once

#include <QApplication>

namespace wplayer 
{
	class WPlayerApplication : public QApplication
	{
		Q_OBJECT

	public:
		WPlayerApplication(int argc, char* argv[]);
		~WPlayerApplication();

	private:
		void setLogConfig();
		void initLog();
		void checkLogFileNumber();
	};
}