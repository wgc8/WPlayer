#include "PlayControlBar.h"
#include "ui_PlayControlBar.h"
#include "logic/PlayController/PlayController.h"
#include "module/Log/easylogging++.h"
#define SLIDER_MAX_INTERVALS 1000

namespace wplayer {

	PlayControlBar::PlayControlBar(QWidget* parent)
		: QWidget(parent)
	{
		ui.setupUi(this);
		initUI();
		setAttribute(Qt::WA_StyledBackground);
		initConnection();

	}

	PlayControlBar::~PlayControlBar()
	{
	}

	void PlayControlBar::initUI()
	{
		// ��ʼ��������
		ui.processSlider->setMinimum(0);
		ui.processSlider->setMaximum(SLIDER_MAX_INTERVALS);
		ui.processSlider->setSingleStep(1);
		ui.processSlider->setValue(0);
		ui.processSlider->setTracking(false);
		// ��ʼ������������
		ui.volumeSlider->setMinimum(0);
		ui.volumeSlider->setMaximum(100);
		ui.volumeSlider->setSingleStep(1);
		ui.volumeSlider->setValue(100);
		ui.volumeSlider->setTracking(false);
	}

	void PlayControlBar::initConnection()
	{
		// �����л�����/��ͣ�źŲ�
		connect(ui.playBtn, &QPushButton::clicked, this, &PlayControlBar::onPlayBtnClicked);
		// ���²���ʱ��
		connect(&PlayController::getInstance(), &PlayController::signalUpdateProgress, this, &PlayControlBar::onUpdateProgress);
		// ��������
		connect(ui.volumeSlider, &QSlider::valueChanged, this, &PlayControlBar::onVolumeChanged);
		// ����
		connect(this, &PlayControlBar::signalPlay, &PlayController::getInstance(), &PlayController::play);
		// ��ͣ
		connect(this, &PlayControlBar::signalPause, &PlayController::getInstance(), &PlayController::pause);
		// ȫ��
		connect(ui.fullScreenBtn, &QPushButton::clicked, this, &PlayControlBar::signalFullScreen);
		// �������϶�
		connect(ui.processSlider, &QSlider::valueChanged, this, &PlayControlBar::onSeek);
		// ��������ѹ״̬
		connect(ui.processSlider, &QSlider::sliderPressed, [&] {m_bProcessSliderPressed = true; });
		connect(ui.processSlider, &QSlider::sliderReleased, [&] {m_bProcessSliderPressed = false; m_bSeek = true; });

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
			// ����
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
		// ���㵱ǰʱ��
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

		// ���ý�����ֵ����pressed״̬�ŸĽ�������ֵ
		if (!m_bProcessSliderPressed)
		{
			int value = 0.5 + SLIDER_MAX_INTERVALS * cur / dur;
			ui.processSlider->setValue(value);
		}
	}

	void PlayControlBar::onVolumeChanged(int value)
	{
		double percent = value / 100.0;
		PlayController::getInstance().setVolume(percent);
	}

	void PlayControlBar::onSeek(int value)
	{
		if (m_bSeek)
		{
			m_bSeek = false;
			// ��ʱ���� * (����������) * ΢�� * ����
			int64_t seekPosMicroSec = m_lDur * value / SLIDER_MAX_INTERVALS * 1000 * 1000 ;
 			LOG(INFO) << "slider presed. move to seekPos: " << seekPosMicroSec;
			PlayController::getInstance().seek(seekPosMicroSec);
		}
	}

	void PlayControlBar::paintEvent(QPaintEvent * e)
	{
		QWidget::paintEvent(e);
	}
}