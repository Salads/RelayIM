#include "QChatView.h"

QChatView::QChatView(QModelManager* manager, QWidget *parent)
    : m_manager(manager), QScrollArea(parent)
{
    ui.setupUi(this);
    m_model = new QChatModel(this, m_manager);
    setWidget(m_model);
}

QChatView::~QChatView()
{}

void QChatView::SetRoom(RoomID roomID)
{
    m_model->SetRoom(roomID);
}
