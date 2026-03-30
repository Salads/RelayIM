#include "RelayClient.h"

RelayClient::RelayClient(QModelManager* manager, QWidget *parent)
    : m_manager(manager), QMainWindow(parent)
{
    ui.setupUi(this);

    setWindowTitle("Relay IM");

    setMinimumSize(c_minSize);
    resize(c_minSize);

    QHBoxLayout* hLayoutMainContent = new QHBoxLayout(this->centralWidget());
    hLayoutMainContent->setContentsMargins(10, 10, 10, 10);

    QVBoxLayout* vLayoutChatRooms = new QVBoxLayout();

    QLabel* chatRoomsLabel = new QLabel("Chat Rooms");
    vLayoutChatRooms->addWidget(chatRoomsLabel, 0);

    hLayoutMainContent->addLayout(vLayoutChatRooms, 2);

    QHBoxLayout* buttonLayout = new QHBoxLayout();

    m_roomsListView = new QListView();
    m_roomsListView->setModel(m_manager->getModelForRooms());
    m_roomsListView->setSelectionMode(QListView::SelectionMode::SingleSelection);
    m_roomsListView->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    vLayoutChatRooms->addWidget(m_roomsListView, 9);

    m_createOrJoinChatRoomButton = new QPushButton("Create/Join");
    m_createOrJoinChatRoomButton->setMaximumHeight(30);
    m_leaveChatRoomButton = new QPushButton("Leave");

    buttonLayout->addWidget(m_createOrJoinChatRoomButton);
    buttonLayout->addWidget(m_leaveChatRoomButton);
    vLayoutChatRooms->addLayout(buttonLayout, 1);

    m_chatWidget = new QChatWidget(m_manager);

    hLayoutMainContent->addWidget(m_chatWidget, 8);

    m_connectionStatus = new QConnectionStatus();
    ui.m_statusBar->addWidget(m_connectionStatus);
    ui.m_statusBar->setStyleSheet("QStatusBar { background: gray }"); // TEMP(Salads): status bar QSS

    m_leaveChatRoomButton->setEnabled(false); // First load = no rooms. When we get rooms, the current should automatically be changed.

    m_manager->initialize();
    initializeSignalConnections();
}

void RelayClient::setCurrentRoom(RoomID roomID)
{
    QModelIndex newRoomIdx = m_manager->getChatRoomIdx(roomID);
    if(newRoomIdx.isValid())
    {
        // The signal connection for currentChanged -> chat widget should handle chat widget room update.
        m_roomsListView->setCurrentIndex(newRoomIdx);
    }
}

void RelayClient::initializeSignalConnections()
{
    connect(m_manager, &QModelManager::eventJoinRoomResponse,            this, &RelayClient::slotJoinRoomResponse);
    connect(m_manager, &QModelManager::eventCreateRoomResponse,          this, &RelayClient::slotCreateRoomResponse);
    connect(m_manager, &QModelManager::eventRoomUpdateUserAboutToLeave, this, &RelayClient::slotRoomUpdateUserLeft);

    // Sets chat widget room model when room on left is selected
    connect(m_roomsListView, &QListView::clicked, this, [this](const QModelIndex& clickedItem)
    {
        // NOTE(Salads): Via Docs: clicked emitted only when index is valid.
        QVariant roomID = clickedItem.data(QChatRoomsModel::Role::RoomIDRole);
        m_chatWidget->setRoomId(static_cast<RoomID>(roomID.toUInt()));
    });

    connect(m_createOrJoinChatRoomButton, &QPushButton::clicked, this, [this](bool checked)
    {
        QChatRoomsDialog diag(m_manager);
        diag.exec();
    });

    // Leave ChatRoom Packet
    connect(m_leaveChatRoomButton, &QPushButton::clicked, this, [this](bool checked)
    {
        QModelIndex currentIndex = m_roomsListView->currentIndex();
        if(currentIndex.isValid())
        {
            RoomID roomID(m_manager->getModelForRooms()->data(currentIndex, QChatRoomsModel::Role::RoomIDRole).toUInt());
            m_manager->getClient()->sendLeaveChatRoom(roomID);
        }
    });

    connect(m_roomsListView->selectionModel(), &QItemSelectionModel::currentChanged, this, [this](const QModelIndex& current, const QModelIndex& prev)
    {
        bool isSomethingSelected = current.isValid();
        m_leaveChatRoomButton->setEnabled(isSomethingSelected);

        if(isSomethingSelected)
        {
            RoomID roomID(m_manager->getModelForRooms()->data(current, QChatRoomsModel::Role::RoomIDRole).toUInt());
            m_chatWidget->setRoomId(roomID);
        }
    });
}

void RelayClient::slotJoinRoomResponse(PacketResponseReason reason, RoomID newRoomID, std::string newChatRoomName)
{
    if(reason == PacketResponseReason::Success)
    {
        setCurrentRoom(newRoomID); // Set current room to our newly joined room
    }
}

void RelayClient::slotCreateRoomResponse(PacketResponseReason reason, RoomID newRoomID, std::string newChatRoomName)
{
    if(reason == PacketResponseReason::Success)
    {
        setCurrentRoom(newRoomID); // Set current room to our newly created room
    }
}

void RelayClient::slotRoomUpdateUserLeft(RoomID roomID, PeerID peerID)
{
    if(peerID == m_manager->getLocalPeerId())
    {
        if(m_chatWidget->getRoomId() == roomID)
        {
            m_chatWidget->setRoomId(RoomID());
        }
    }
}

bool RelayClient::tryConnect()
{
    m_connectionStatus->setStatus(QConnectionStatus::Status::Connecting);
    if(m_manager->connectToServer())
    {
        m_connectionStatus->setStatus(QConnectionStatus::Status::ConnectedUnregistered);
        return true;
    }
    else
    {
        m_connectionStatus->setStatus(QConnectionStatus::Status::NotConnected);
        return false;
    }
}

RelayClient::~RelayClient()
{
    m_manager->shutdown();
}

void RelayClient::setStatusUI(QConnectionStatus::Status status)
{
    m_connectionStatus->setStatus(status);
}

void RelayClient::updateWindowTitle()
{
    std::string localUsername = m_manager->getUsernameByPeerId(m_manager->getLocalPeerId());
    QString newTitle = QString("Relay IM (%1)").arg(QString::fromStdString(localUsername));
    setWindowTitle(newTitle);
}
