#include "QChatView.h"

QChatView::QChatView(QModelManager* manager, QWidget *parent)
    : m_manager(manager), QScrollArea(parent)
{
    setAlignment(Qt::AlignmentFlag::AlignBottom);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

QChatView::~QChatView()
{}

int QChatView::getViewportWidth()
{
    QScrollBar* vBar = verticalScrollBar();
    return width() - (vBar->isVisible() ? vBar->width() : 0);
}

void QChatView::resizeEvent(QResizeEvent* event)
{
    QScrollArea::resizeEvent(event);
    emit onResize(event->oldSize(), event->size());
}

