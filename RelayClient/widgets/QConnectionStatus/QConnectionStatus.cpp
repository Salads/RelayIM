#include "QConnectionStatus.h"

QConnectionStatus::QConnectionStatus(QWidget* parent)
    : QWidget(parent)
{
    setMinimumSize(250, 15);
    setMaximumSize(250, 15);

    setStatus(Status::NotConnected);
}

QConnectionStatus::~QConnectionStatus()
{
}

void QConnectionStatus::setStatus(QConnectionStatus::Status status)
{
    switch(status)
    {
        case Status::NotConnected:
            m_statusStr = "Not Connected";
            m_iconColor = Qt::GlobalColor::red;
            break;
        case Status::Connecting:
            m_statusStr = "Connecting...";
            m_iconColor = Qt::GlobalColor::darkYellow;
            break;
        case Status::ConnectedUnregistered:
            m_statusStr = "Connected (Unregistered)";
            m_iconColor = Qt::GlobalColor::yellow;
            break;
        case Status::ConnectedRegistered:
            m_statusStr = "Connected (Registered)";
            m_iconColor = Qt::GlobalColor::green;
            break;
    }

    m_status = status;

    update(); // Force paint event.
}

void QConnectionStatus::paintEvent(QPaintEvent*)
{
    QPainter painter(this);

    QRect widgetSize = geometry();
    int widgetHeight = widgetSize.height();
    int widgetWidth = widgetSize.width();

    int iconDim = 10;
    int iconLeftPadding = 5;

    int iconTopLeftX = iconLeftPadding; // Top-Left Origin
    int iconTopLeftY = (widgetHeight - iconDim) / 2; // Top-Left Origin

    painter.setBrush(m_iconColor);
    painter.drawEllipse(iconTopLeftX, iconTopLeftY, iconDim, iconDim);

    ////////////////////////////////////////////////////
    // Font Rendering
    ////////////////////////////////////////////////////
    QFontMetrics fontMetrics = painter.fontMetrics();

    int iconEndX = iconLeftPadding + iconDim;
    int textLeftPadding = 10;

    int textStartX = iconEndX + textLeftPadding;
    int baseline = (widgetHeight / 2) + (fontMetrics.ascent() - fontMetrics.descent()) / 2;
    QPoint textStart(textStartX, baseline);

    painter.drawText(textStart, m_statusStr);
}
