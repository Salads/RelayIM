#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_RelayClient.h"
#include <qboxlayout.h>

class RelayClient : public QMainWindow
{
    Q_OBJECT

public:
    RelayClient(QWidget *parent = nullptr);
    ~RelayClient();

private:
    Ui::RelayClientClass ui;
};

