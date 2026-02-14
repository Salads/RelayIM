#include "QChatRoom.h"

QChatRoom::QChatRoom(QWidget *parent)
    : QPushButton(parent)
{
    ui.setupUi(this);

    setMinimumSize(100, 40);
    setFixedSize(100, 40);

    // Make this button toggleable
    setCheckable(true);
    setChecked(false);

    // Create label for room name
    m_roomNameLabel = new QLabel(this);
    m_roomNameLabel->setAlignment(Qt::AlignCenter);
    m_roomNameLabel->setStyleSheet("color: #000000; background-color: transparent; border: none;");
    m_roomNameLabel->setAttribute(Qt::WA_TransparentForMouseEvents, true);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(5, 5, 5, 5);
    layout->addWidget(m_roomNameLabel);
    setLayout(layout);
    setStyleSheet(
        "QPushButton { "
        "background-color: #ffffff; "
        "border: 1px solid transparent; "
        "border-radius: 5px; "
        "border-color: gray;"
        "} "
        "QPushButton:hover { "
        "background-color: #cccccc; "
        "} "
        "QPushButton:checked { "
        "background-color: #ffffff; "
        "border: 2px solid #0066ff; "
        "} "
    );
}

QChatRoom::~QChatRoom()
{}

void QChatRoom::Initialize(int roomId, const std::string roomName)
{
    m_roomInfo.m_roomId = roomId;
    m_roomInfo.m_roomName = roomName;
    m_roomNameLabel->setText(QString::fromStdString(roomName));
}

void QChatRoom::Initialize(ChatRoomInfo &info)
{
    m_roomInfo = info;
    m_roomNameLabel->setText(QString::fromStdString(info.m_roomName));
}

int QChatRoom::getRoomId()
{
    return m_roomInfo.m_roomId;
}

std::string QChatRoom::GetRoomName()
{
    return m_roomInfo.m_roomName;
}