#include "QMessage.h"

QMessage::QMessage(QWidget *parent)
    : QWidget(parent)
{
    QHBoxLayout* layout = new QHBoxLayout(this);
    m_usernameLabel = new QLabel();
    m_messageLabel = new QLabel();

    layout->addWidget(m_usernameLabel);
    layout->addWidget(m_messageLabel);

    m_messageLabel->setWordWrap(true);
    m_messageLabel->setMinimumWidth(MessageWidth);
    m_messageLabel->setMaximumWidth(MessageWidth);
    setMaximumWidth(TotalWidth);

    m_usernameLabel->setFont(Font);
    m_messageLabel->setFont(Font);

    QPalette pal = QPalette();
    pal.setColor(QPalette::Window, "#669999");
    m_usernameLabel->setAutoFillBackground(true);
    m_usernameLabel->setPalette(pal);

    pal.setColor(QPalette::Window, "#669999");
    m_messageLabel->setAutoFillBackground(true);
    m_messageLabel->setPalette(pal);
}

QMessage::~QMessage()
{}

void QMessage::SetContents(std::string username, std::string message)
{
    m_usernameLabel->setText(QString::fromStdString(username) + ": ");

    QFontMetrics metrics(m_usernameLabel->font());
    int width = metrics.horizontalAdvance(m_usernameLabel->text());
    m_usernameLabel->setFixedWidth(width);
    m_usernameLabel->setMaximumWidth(width);
    m_usernameLabel->setMinimumWidth(width);

    m_messageLabel->setText(QString::fromStdString(message));
}
