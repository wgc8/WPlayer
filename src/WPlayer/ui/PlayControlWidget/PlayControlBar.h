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
		void initConnection();

		void onPlayBtnClicked();

		void onUpdateProgress(const int64_t& cur, const int64_t& dur);
	signals:
		void signalPlay();
		void signalPause();

	protected:
		virtual void paintEvent(QPaintEvent* e) override;

	private:
		Ui::PlayControlBar ui;
		QString m_strCur;					// ��ǰʱ��
		QString m_strDur;					// ��ʱ��
		int64_t m_lDur;						// ��ʱ��(��λ��)
	};
}