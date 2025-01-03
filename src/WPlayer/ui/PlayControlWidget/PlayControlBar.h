#pragma once

#include <QWidget>
#include "ui_PlayControlBar.h"
namespace wplayer {
	class PlayControlBar : public QWidget
	{
		Q_OBJECT

	public:
		PlayControlBar(QWidget* parent);
		~PlayControlBar();

		void initUI();

		void initConnection();

		void onPlayBtnClicked();

		void onUpdateProgress(const int64_t& cur, const int64_t& dur);
	signals:
		void signalPlay();
		void signalPause();
		void signalFullScreen();

	public slots:
		// 调节音量
		void onVolumeChanged(int v);
		// seek操作
		void onSeek(int value);

	protected:
		virtual void paintEvent(QPaintEvent* e) override;

	private:
		Ui::PlayControlBar ui;
		QString m_strCur;						// 当前时间
		QString m_strDur;						// 总时长
		int64_t m_lDur;							// 总时长(单位秒)
		bool m_bProcessSliderPressed{ false };	// 进度条按压状态
		bool m_bSeek{ false };					// seek标志
	};
}