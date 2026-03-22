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
    m_messageLabel->setMinimumWidth(FixedMessageWidth);
    m_messageLabel->setMaximumWidth(FixedMessageWidth);
    m_messageLabel->setFixedWidth(FixedMessageWidth);
    m_messageLabel->setMargin(Margin);
    m_messageLabel->setContentsMargins(Padding, Padding, Padding, Padding);

    m_usernameLabel->setMaximumWidth(MaxUsernameWidth);
    m_usernameLabel->setMargin(Margin);
    m_usernameLabel->setContentsMargins(Padding, Padding, Padding, Padding);

    setMaximumWidth(TotalWidth);
    setMinimumWidth(TotalWidth);
    setContentsMargins(Padding, Padding, Padding, Padding);

    m_usernameLabel->setFont(Font);
    m_messageLabel->setFont(Font);

    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
    setFocusPolicy(Qt::NoFocus);
}

QMessage::~QMessage()
{}

void QMessage::SetContents(std::string username, std::string message)
{
    m_usernameLabel->setText(QString::fromStdString(username) + ": ");

    QFontMetrics metrics(m_usernameLabel->font());
    int width = metrics.horizontalAdvance(m_usernameLabel->text());
    m_usernameLabel->setFixedWidth(width);
    m_usernameLabel->setMinimumWidth(width);

    m_messageLabel->setText(QString::fromStdString(message));
    adjustSize();
}
