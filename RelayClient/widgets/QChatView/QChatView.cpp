#include "QChatView.h"

QChatView::QChatView(QModelManager* manager, QWidget *parent)
    : m_manager(manager), QScrollArea(parent)
{
    ui.setupUi(this);
    m_model = new QChatModel(this, m_manager);
    setWidget(m_model);
    setAlignment(Qt::AlignmentFlag::AlignBottom);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

QChatView::~QChatView()
{}

void QChatView::SetRoom(RoomID roomID)
{
    m_model->SetRoom(roomID);
}

int QChatView::GetViewportWidth()
{
    QScrollBar* vBar = verticalScrollBar();
    return width() - (vBar->isVisible() ? vBar->width() : 0);
}

void QChatView::resizeEvent(QResizeEvent* event)
{
    QScrollArea::resizeEvent(event);

    if(event->oldSize().width() != event->size().width())
    {
        m_model->HandleResize();
    }
    else if(event->oldSize().height() != event->size().height())
    {
        m_model->RenderObjects();
    }
}

