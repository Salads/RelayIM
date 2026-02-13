#include "QChatRoom.h"

QChatRoom::QChatRoom(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    setMinimumSize(100, 40);
    setFixedSize(100, 40);

    m_roomNameLabel = new QLabel(this);
}

QChatRoom::~QChatRoom()
{}

void QChatRoom::Initialize(int roomId, const std::string roomName)
{
    m_roomId = roomId;
    m_roomNameLabel->setText(QString::fromStdString(roomName));
}
