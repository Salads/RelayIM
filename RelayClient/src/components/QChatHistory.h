#pragma once

#include <QWidget>
#include "ui_QChatHistory.h"

class QChatHistory : public QWidget
{
    Q_OBJECT

public:
    QChatHistory(QWidget *parent = nullptr);
    ~QChatHistory();

private:
    Ui::QChatHistoryClass ui;
};

