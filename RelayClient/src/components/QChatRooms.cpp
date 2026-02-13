#include "QChatRooms.h"
#include "QChatRoom.h"
#include "qboxlayout.h"

QChatRooms::QChatRooms(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    setMaximumWidth(120);

    QVBoxLayout* mainLayout = new QVBoxLayout(this); // Allows children to stretch out to size of parent widget
    mainLayout->setContentsMargins(0, 0, 0, 0);

    m_container = new QWidget(this);
    QVBoxLayout* containerLayout = new QVBoxLayout(m_container);
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setWidget(m_container);
    mainLayout->addWidget(m_scrollArea);

    for (int i = 0; i < 10; i++) 
    {
        QChatRoom* room = new QChatRoom(this);
        room->Initialize(i, "Example Room " + std::to_string(i));
        containerLayout->addWidget(room);
    }

    containerLayout->addStretch(1); // Pushes rooms to the top of the scroll area
}

QChatRooms::~QChatRooms()
{}

