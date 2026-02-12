#include "QMessage.h"

QMessage::QMessage(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    lineEditMessage = new QLineEdit(this);
    lineEditMessage->setReadOnly(true);

    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setFixedHeight(lineEditMessage->sizeHint().height() + 8);
    setFixedWidth(lineEditMessage->sizeHint().width() + 8);
}

QMessage::~QMessage()
{}

void QMessage::setMessage(const std::string& msg)
{
    lineEditMessage->setText(QString::fromStdString(msg));
    setFixedHeight(lineEditMessage->sizeHint().height() + 8);
    setFixedWidth(lineEditMessage->sizeHint().width() + 16);
}

std::string QMessage::getMessage() const
{
    return lineEditMessage->text().toStdString();
}