#include "widgets/QChatWidget/QChatWidget.h"

QChatWidget::QChatWidget(QModelManager *manager, QWidget *parent)
    : QWidget(parent)
{
    m_manager = manager;

    // Ensure this widget stretches to fill available space
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_roomNameLabel = new QLabel(this);
    m_roomNameLabel->setText("- No Chatroom -");

    QVBoxLayout *vLayoutMainContent = new QVBoxLayout(this);
    vLayoutMainContent->setContentsMargins(10, 0, 10, 0);

    m_chatListView = new QChatView(m_manager);
    m_chatModel = new QChatModel(m_chatListView, m_manager);

    m_chatInput = new QChatInput(m_manager);

    vLayoutMainContent->addWidget(m_roomNameLabel, 0);
    vLayoutMainContent->addWidget(m_chatListView, 7);
    vLayoutMainContent->addWidget(m_chatInput, 2);

    setMinimumWidth(m_chatListView->minimumWidth());
}

void QChatWidget::SetRoomID(RoomID roomID)
{
    if(roomID == INVALID_ROOM_ID)
    {
        m_roomNameLabel->setText("- No Chatroom -");
    }
    else
    {
        std::string roomname = m_manager->GetRoomnameByRoomID(roomID);
        m_roomNameLabel->setText(QString::fromStdString("# " + roomname));
    }

    m_chatModel->SetRoom(roomID);
    m_chatInput->SetRoomID(roomID);
    m_currentRoomID = roomID;
}

RoomID QChatWidget::GetRoomID()
{
    return m_currentRoomID;
}