#include "QChatInput.h"

QChatInput::QChatInput(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    setFixedHeight(50);
}

QChatInput::~QChatInput()
{}

