#pragma once

#include <QWidget>
#include "src/components/QChatHistory.h"
#include "src/components/QChatInput.h"
#include "ui_QChatWidget.h"

class QChatWidget : public QWidget
{
    Q_OBJECT

public:
    QChatWidget(QWidget *parent = nullptr);
    ~QChatWidget();

    void addChatMessage(const std::string& message);

private:
    Ui::QChatWidgetClass ui;

    QChatHistory* m_chatHistory;
    QChatInput* m_chatInput;
};

