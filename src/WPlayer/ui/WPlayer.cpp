#include "WPlayer.h"
#include "module/GLWidget/GLRenderWidget.h"
#include "ui/PlayControlWidget/PlayControlBar.h"
#include "logic/PlayController/PlayController.h"
#include "module/Log/easylogging++.h"

namespace wplayer {
    WPlayer::WPlayer(QWidget* parent)
        : QMainWindow(parent)
    {
        ui.setupUi(this);
    }

    void WPlayer::init()
    {
        initAVOutput();
        initUI();
        initConnection();
        PlayController::getInstance().setFileName("test.mp4");

    }

    void WPlayer::initUI()
    {
        m_pPlayControlBar = new PlayControlBar(this);
        ui.verticalLayout->addWidget(m_pPlayControlBar);
    }

    void WPlayer::initAVOutput()
    {
        m_pGLWidget = new GLRenderWidget(this);
        if (m_pGLWidget)
        {
            ui.gridLayout->addWidget((QWidget*)m_pGLWidget);
            PlayController::getInstance().setGLWidget(m_pGLWidget);
        }
    }

    void WPlayer::initConnection()
    {

        connect(m_pPlayControlBar, &PlayControlBar::signalFullScreen, this, &QWidget::showFullScreen);
    }

    WPlayer::~WPlayer()
    {
    }
}