#include "src/components/QChatWidget.h"
#include <qboxlayout.h>
#include <qframe.h>
#include "src/components/QChatHistory.h"
#include "src/components/QChatInput.h"

QChatWidget::QChatWidget(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    QVBoxLayout *vLayoutMainContent = new QVBoxLayout(this);
    vLayoutMainContent->setContentsMargins(10, 0, 10, 0);

    m_chatHistory = new QChatHistory();
    m_chatInput = new QChatInput();

    vLayoutMainContent->addWidget(m_chatHistory, 8);
    vLayoutMainContent->addWidget(m_chatInput, 2);

    m_chatHistory->setStyleSheet("background-color: #555555;");
    m_chatInput->setStyleSheet("background-color: #555555;");

    // TODO(Salads): Temporary, when we add child widgets this should be unnecessary.
    m_chatHistory->setAttribute(Qt::WA_StyledBackground, true);
    m_chatInput->setAttribute(Qt::WA_StyledBackground, true);
}

QChatWidget::~QChatWidget()
{}

void QChatWidget::addChatMessage(const std::string& message)
{
    m_chatHistory->addMessage(message);
}

