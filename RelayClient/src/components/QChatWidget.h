#pragma once

#include <QWidget>
#include "ui_QChatWidget.h"

class QChatWidget : public QWidget
{
    Q_OBJECT

public:
    QChatWidget(QWidget *parent = nullptr);
    ~QChatWidget();

private:
    Ui::QChatWidgetClass ui;
};

