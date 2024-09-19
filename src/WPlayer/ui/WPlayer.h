#pragma once
#include <QtWidgets/QMainWindow>
#include "ui_WPlayer.h"

class GLRenderWidget;
namespace wplayer {
    class PlayControlBar;
    class WPlayer : public QMainWindow
    {
        Q_OBJECT

    public:
        WPlayer(QWidget* parent = nullptr);
        ~WPlayer();
        void init();
        void initUI();
        void initAVOutput();
        void initConnection();

    private:
        Ui::WPlayerClass ui;
        PlayControlBar* m_pPlayControlBar{ nullptr };
        GLRenderWidget* m_pGLWidget{ nullptr };
    };
}
