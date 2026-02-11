#pragma once

#include <QWidget>
#include "ui_QChatInput.h"

class QChatInput : public QWidget
{
    Q_OBJECT

public:
    QChatInput(QWidget *parent = nullptr);
    ~QChatInput();

private:
    Ui::QChatInputClass ui;
};

