#include "QMessage.h"

QMessage::QMessage(QWidget *parent)
    : QWidget(parent)
{
    m_usernameLabel = new QLabel(this);
    m_messageEdit = new QTextEdit(this);

    m_messageEdit->setContentsMargins(Padding, Padding, Padding, Padding);
    m_messageEdit->setAlignment(Qt::AlignmentFlag::AlignLeft);
    m_messageEdit->setCursorWidth(0);
    m_messageEdit->setReadOnly(true);
    m_messageEdit->setFrameStyle(QFrame::NoFrame);
    m_messageEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_messageEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_messageEdit->setTextInteractionFlags(Qt::NoTextInteraction);
    m_messageEdit->setAutoFillBackground(false);
    m_messageEdit->setLineWrapMode(QTextEdit::LineWrapMode::WidgetWidth);
    m_messageEdit->document()->setDocumentMargin(0);

    m_usernameLabel->setMargin(Margin);
    m_usernameLabel->setContentsMargins(Padding, Padding, Padding, Padding);
    m_usernameLabel->setAlignment(Qt::AlignmentFlag::AlignLeft | Qt::AlignmentFlag::AlignTop);
    setContentsMargins(Padding, Padding, Padding, Padding);

    m_usernameLabel->setFont(Font);
    m_messageEdit->setFont(Font);

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

void QMessage::setContents(std::string username, std::string message, int viewportWidth)
{
    m_usernameLabel->setText(QString::fromStdString(username) + ": ");
    m_messageEdit->setText(QString::fromStdString(message));

    QMessageTextConstraints constraints = getTextConstraints(username, message, viewportWidth);

    m_usernameLabel->setMinimumSize(constraints.m_usernameSize);
    m_usernameLabel->setMaximumSize(constraints.m_usernameSize);
    m_usernameLabel->setFixedSize(constraints.m_usernameSize);

    m_messageEdit->setMinimumSize(constraints.m_messageSize);
    m_messageEdit->setMaximumSize(constraints.m_messageSize);
    m_messageEdit->setFixedSize(constraints.m_messageSize);

    m_usernameLabel->move(LeftPadding, 0);
    m_messageEdit->move(LeftPadding + constraints.m_usernameSize.width() + TextPadding, 0);

    adjustSize(); // shrinkwrap
}

QMessageTextConstraints QMessage::getTextConstraints(std::string username, std::string message, int viewportWidth)
{
    QFontMetrics metrics(Font);
    int usernameTextWidth = metrics.horizontalAdvance(QString::fromStdString(username));

    int usernameWidth = metrics.horizontalAdvance(QString::fromStdString(username), Qt::AlignLeft);
    QRect messageBounds = metrics.boundingRect(QRect(0, 0, viewportWidth - usernameWidth - (LeftPadding * 2) - (TextPadding * 2), INT_MAX), 
                                               Qt::AlignLeft | Qt::TextWrapAnywhere, 
                                               QString::fromStdString(message));

    QMessageTextConstraints result;
    result.m_usernameSize = QSize(usernameTextWidth, metrics.height());
    result.m_messageSize = QSize(messageBounds.width(), messageBounds.height());

    return result;
}
