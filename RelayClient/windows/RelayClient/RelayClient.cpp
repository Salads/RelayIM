#include "RelayClient.h"
#include "models/QChatRoomsModel/QChatRoomsModel.h"

#include <QBoxLayout>

RelayClient::RelayClient(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this); // Generated UI from visual editor

    QHBoxLayout* hLayoutMainContent = new QHBoxLayout(this->centralWidget()); // Main Content Layout (ChatRooms, Chat Area)
    hLayoutMainContent->setContentsMargins(10, 10, 10, 10);

    QVBoxLayout* vLayoutChatRooms = new QVBoxLayout();

    QLabel* chatRoomsLabel = new QLabel("Chat Rooms");
    vLayoutChatRooms->addWidget(chatRoomsLabel, 0);

    hLayoutMainContent->addLayout(vLayoutChatRooms, 2);

    QHBoxLayout* buttonLayout = new QHBoxLayout();

    m_roomsListView = new QListView();
    m_roomsListView->setModel(m_manager.GetModelForRooms().get());
    m_roomsListView->setSelectionMode(QListView::SelectionMode::SingleSelection);
    m_roomsListView->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    vLayoutChatRooms->addWidget(m_roomsListView, 9);

    m_createChatRoomButton = new QPushButton("Create Room");
    m_createChatRoomButton->setMaximumHeight(30);

    m_joinChatRoomButton = new QPushButton("Join Room");
    m_joinChatRoomButton->setMaximumHeight(30);

    buttonLayout->addWidget(m_createChatRoomButton);
    buttonLayout->addWidget(m_joinChatRoomButton);
    vLayoutChatRooms->addLayout(buttonLayout, 1);

    m_chatWidget = new QChatWidget(&m_manager);

    hLayoutMainContent->addWidget(m_chatWidget, 8);

    connect(m_roomsListView, &QListView::clicked, this, [this](const QModelIndex& clickedItem) 
    {
        // NOTE(Salads): Via Docs: clicked emitted only when index is valid.
        QVariant roomID = clickedItem.data(QChatRoomsModel::Role::RoomIDRole);
        m_chatWidget->setRoom(static_cast<RoomID>(roomID.toUInt()));
    });

    this->setMinimumSize(550, 350);
}

RelayClient::~RelayClient()
{}

