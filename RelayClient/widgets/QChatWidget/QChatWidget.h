#pragma once

#include <QWidget>

#include "widgets/QChatHistory/QChatHistory.h"
#include "components/QChatInput/QChatInput.h"
#include "ui_QChatWidget.h"

#include <qlabel.h>

class QChatWidget : public QWidget
{
    Q_OBJECT

public:
    QChatWidget(QWidget *parent = nullptr);
    ~QChatWidget();

    void addChatMessage(const std::string& message);
    void setRoom(int roomId, const std::string& roomName);
    void clear();

private:
    Ui::QChatWidgetClass ui;

    int m_roomId;
    QLabel* m_roomNameLabel;
    QChatHistory* m_chatHistory;
    QChatInput* m_chatInput;
};

