#include "src/components/QChatWidget.h"
#include <qboxlayout.h>
#include <qframe.h>
#include "src/components/QChatHistory.h"
#include "src/components/QChatInput.h"

QChatWidget::QChatWidget(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    // Ensure this widget stretches to fill available space
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_roomNameLabel = new QLabel(this);
    m_roomNameLabel->setText("No chat room selected");

    QVBoxLayout *vLayoutMainContent = new QVBoxLayout(this);
    vLayoutMainContent->setContentsMargins(10, 0, 10, 0);

    m_chatHistory = new QChatHistory();
    m_chatInput = new QChatInput();

    vLayoutMainContent->addWidget(m_roomNameLabel, 0);
    vLayoutMainContent->addWidget(m_chatHistory, 7);
    vLayoutMainContent->addWidget(m_chatInput, 2);
}

QChatWidget::~QChatWidget()
{}

void QChatWidget::addChatMessage(const std::string& message)
{
    m_chatHistory->addMessage(message);
}

void QChatWidget::clear()
{
    m_chatHistory->clear();
}

void QChatWidget::setRoom(int roomId, const std::string& roomName)
{
    m_roomId = roomId;
    m_roomNameLabel->setText(roomName.c_str());
}