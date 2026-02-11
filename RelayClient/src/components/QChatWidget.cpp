#include "QChatWidget.h"
#include <qboxlayout.h>
#include <qframe.h>

QChatWidget::QChatWidget(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    QVBoxLayout *vLayoutMainContent = new QVBoxLayout(this);
    this->setLayout(vLayoutMainContent);

    QFrame *chatAreaFrame = new QFrame(this);
    vLayoutMainContent->addWidget(chatAreaFrame);
    chatAreaFrame->setFrameStyle(QFrame::Panel | QFrame::Raised);

    QVBoxLayout* vLayoutChatArea = new QVBoxLayout();

}

QChatWidget::~QChatWidget()
{}

