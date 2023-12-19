#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_WPlayer.h"

class WPlayer : public QMainWindow
{
    Q_OBJECT

public:
    WPlayer(QWidget *parent = nullptr);
    ~WPlayer();

private:
    Ui::WPlayerClass ui;
};
