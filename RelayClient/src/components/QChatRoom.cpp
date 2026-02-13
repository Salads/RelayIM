#include "QChatRoom.h"

QChatRoom::QChatRoom(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    setMinimumSize(100, 40);
    setFixedSize(100, 40);

    setStyleSheet("background-color: #2E2E2E; border-radius: 5px;");    

    m_roomNameLabel = new QLabel(this);
}

QChatRoom::~QChatRoom()
{}

void QChatRoom::Initialize(int roomId, const std::string roomName)
{
    m_roomId = roomId;
    m_roomNameLabel->setText(QString::fromStdString(roomName));
}
