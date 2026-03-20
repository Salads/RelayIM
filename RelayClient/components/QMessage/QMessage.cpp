#include "QMessage.h"

QMessage::QMessage(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    ui.messageLabel->setWordWrap(true);
    ui.messageLabel->setMaximumWidth(MessageWidth);
    ui.messageLabel->setMinimumWidth(MessageWidth);

    ui.usernameLabel->setMaximumWidth(UsernameWidth);
    ui.usernameLabel->setMinimumWidth(UsernameWidth);

    int totalWidth = MessageWidth + UsernameWidth;
    setMinimumWidth(totalWidth);
    setMaximumWidth(totalWidth);

    ui.usernameLabel->setFont(Font);
    ui.messageLabel->setFont(Font);
}

QMessage::~QMessage()
{}

void QMessage::SetContents(std::string username, std::string message)
{
    ui.usernameLabel->setText(QString::fromStdString(username) + ": ");
    ui.messageLabel->setText(QString::fromStdString(message));
}
