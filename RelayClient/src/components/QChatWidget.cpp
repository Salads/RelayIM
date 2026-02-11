#include "QChatWidget.h"
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

    QChatHistory* chatHistory = new QChatHistory();
    QChatInput* chatInput = new QChatInput();

    vLayoutMainContent->addWidget(chatHistory, 8);
    vLayoutMainContent->addWidget(chatInput, 2);

    chatHistory->setStyleSheet("background-color: #555555;");
    chatInput->setStyleSheet("background-color: #555555;");

    // TODO(Salads): Temporary, when we add child widgets this should be unnecessary.
    chatHistory->setAttribute(Qt::WA_StyledBackground, true);
    chatInput->setAttribute(Qt::WA_StyledBackground, true);
}

QChatWidget::~QChatWidget()
{}

