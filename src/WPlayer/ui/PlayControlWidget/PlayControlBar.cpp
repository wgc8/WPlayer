#include "PlayControlBar.h"
#include "ui_PlayControlBar.h"
#include "logic/PlayController/PlayController.h"
#include "module/Log/easylogging++.h"
namespace wplayer {
	PlayControlBar::PlayControlBar(QWidget* parent)
		: QWidget(parent)
	{
		ui.setupUi(this);
		setAttribute(Qt::WA_StyledBackground);
		initConnection();

	}

	PlayControlBar::~PlayControlBar()
	{
	}

	void PlayControlBar::initConnection()
	{
		// 定义切换播放/暂停信号槽
		connect(ui.playBtn, &QPushButton::clicked, this, &PlayControlBar::onPlayBtnClicked);
		// 更新播放时长
		connect(&PlayController::getInstance(), &PlayController::signalUpdateProgress, this, &PlayControlBar::onUpdateProgress);
	}

	void PlayControlBar::onPlayBtnClicked()
	{
		if (ui.playBtn->text() == "play")
		{
			emit signalPlay();
			ui.playBtn->setText("pause");
		}
		else
		{
			emit signalPause();
			ui.playBtn->setText("play");
		}
	}

	void PlayControlBar::onUpdateProgress(const int64_t& cur, const int64_t& dur)
	{
		auto cpyCur = cur;
		if (dur != m_lDur)
		{
			auto cpyDur = dur;
			// 计算
			int durSec = cpyDur % 60;
			cpyDur /= 60;
			int durMin = cpyDur % 60;
			cpyDur /= 60;
			int durHour = cpyDur % 60;
			m_strDur = QString("%1").arg(durMin, 2, 10, QLatin1Char('0')) + ':' + QString("%1").arg(durSec, 2, 10, QLatin1Char('0'));
			if (durHour > 0)
			{
				m_strDur = QString::number(durHour) + ':' + m_strDur;
			}
			m_lDur = dur;
		}
		// 计算当前时间
		int curSec = cpyCur % 60;
		cpyCur /= 60;
		int curMin = cpyCur % 60;
		cpyCur /= 60;
		int curHour = cpyCur % 60;
		m_strCur = QString("%1").arg(curMin, 2, 10, QLatin1Char('0')) + ':' + QString("%1").arg(curSec, 2, 10, QLatin1Char('0'));
		if (curHour > 0)
		{
			m_strCur = QString::number(curHour) + ':' + m_strCur;
		}
		ui.label->setText(m_strCur + '/' + m_strDur);
	}

	void PlayControlBar::paintEvent(QPaintEvent * e)
	{
		QWidget::paintEvent(e);
	}
}