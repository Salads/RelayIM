#include "QChatRooms.h"
#include "QChatRoom.h"
#include "qboxlayout.h"

QChatRooms::QChatRooms(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    setMaximumWidth(120);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QVBoxLayout* mainLayout = new QVBoxLayout(this); // Allows children to stretch out to size of parent widget
    mainLayout->setContentsMargins(0, 0, 0, 0);

    m_container = new QWidget(this);
    m_containerLayout = new QVBoxLayout(m_container);
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setWidget(m_container);
    mainLayout->addWidget(m_scrollArea);

    m_containerLayout->addStretch(1);

    for (int i = 0; i < 5; i++)
    {
        addRoom(i, "Short Name " + std::to_string(i));
    }
}

QChatRooms::~QChatRooms()
{}

void QChatRooms::addRoom(int roomId, const std::string roomName)
{
    QChatRoom* newRoom = new QChatRoom(this);
    newRoom->Initialize(roomId, roomName);
    m_containerLayout->insertWidget(m_containerLayout->count() - 1, newRoom);
    m_rooms.push_back(newRoom);

    connect(newRoom, &QChatRoom::clicked, this, [this, newRoom]() { onRoomClicked(newRoom); });
}

// TODO(Salads): QButtonGroup?
void QChatRooms::onRoomClicked(QChatRoom *clickedRoom)
{
    // Handle clicking already selected room (do nothing)
    if (clickedRoom == m_selectedRoom)
    {
        clickedRoom->setChecked(true);
        return;
    }

    m_selectedRoom = clickedRoom;

    for (QChatRoom* room : m_rooms)
    {
        if (room != clickedRoom)
        {
            room->setChecked(false);
        }
    }
}   