#include "QMessage.h"

QMessage::QMessage(QWidget *parent)
    : QWidget(parent)
{
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(LeftPadding, 0, 0, 0);
    m_usernameLabel = new QLabel();
    m_messageLabel = new QLabel();

    layout->addWidget(m_usernameLabel);
    layout->addWidget(m_messageLabel);
    layout->addStretch();

    m_messageLabel->setWordWrap(true);
    m_messageLabel->setMargin(Margin);
    m_messageLabel->setContentsMargins(Padding, Padding, Padding, Padding);
    m_messageLabel->setAlignment(Qt::AlignmentFlag::AlignLeft);

    m_usernameLabel->setMargin(Margin);
    m_usernameLabel->setContentsMargins(Padding, Padding, Padding, Padding);
    m_usernameLabel->setAlignment(Qt::AlignmentFlag::AlignLeft | Qt::AlignmentFlag::AlignTop);
    setContentsMargins(Padding, Padding, Padding, Padding);

    m_usernameLabel->setFont(Font);
    m_messageLabel->setFont(Font);

    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
    setFocusPolicy(Qt::NoFocus);

    // For testing QMessage bounds when placed.
    //QPalette pal = QPalette();
    //pal.setColor(QPalette::Window, "#669999");
    //setAutoFillBackground(true);
    //setPalette(pal);
}

QMessage::~QMessage()
{}

void QMessage::SetContents(std::string username, std::string message, int viewportWidth)
{
    m_usernameLabel->setText(QString::fromStdString(username) + ": ");
    m_messageLabel->setText(QString::fromStdString(message));

    QMessageTextConstraints constraints = GetTextConstraints(username, message, viewportWidth);

    m_usernameLabel->setMinimumSize(constraints.m_usernameSize);
    m_usernameLabel->setMaximumSize(constraints.m_usernameSize);
    m_usernameLabel->setFixedSize(constraints.m_usernameSize);

    m_messageLabel->setMinimumSize(constraints.m_messageSize);
    m_messageLabel->setMaximumSize(constraints.m_messageSize);
    m_messageLabel->setFixedSize(constraints.m_messageSize);

    adjustSize(); // shrinkwrap
}

QMessageTextConstraints QMessage::GetTextConstraints(std::string username, std::string message, int viewportWidth)
{
    QFontMetrics metrics(Font);
    int usernameTextWidth = metrics.horizontalAdvance(QString::fromStdString(username));

    int usernameWidth = metrics.horizontalAdvance(QString::fromStdString(username), Qt::AlignLeft);
    QRect messageBounds = metrics.boundingRect(QRect(0, 0, viewportWidth - usernameWidth - LeftPadding, INT_MAX), Qt::AlignLeft | Qt::TextWordWrap, QString::fromStdString(message));

    QMessageTextConstraints result;
    result.m_usernameSize = QSize(usernameTextWidth, metrics.height());
    result.m_messageSize = QSize(messageBounds.width(), messageBounds.height());

    return result;
}
