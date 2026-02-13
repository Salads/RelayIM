#pragma once

#include <list>
#include <string>
#include <QWidget>
#include <qscrollbar.h>
#include <qscrollarea.h>
#include <qboxlayout.h>
#include "ui_QChatHistory.h"
#include "src/components/QMessage.h"
#include <qtimer.h>

class QChatHistory : public QWidget
{
    Q_OBJECT

public:
    QChatHistory(QWidget *parent = nullptr);
    ~QChatHistory();

    void addMessage(std::string message);

private:
    void updateMessages();

private:
    Ui::QChatHistoryClass ui;
    QVBoxLayout* m_layout;
    QWidget* m_messageContainer;
    QScrollArea* m_scrollArea;

    std::list<std::string> m_rawMessages;
    std::list<QMessage *> m_messages;
};

