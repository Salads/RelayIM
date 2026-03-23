#include "RelayClient.h"

RelayClient::RelayClient(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this); // Generated UI from visual editor

    qRegisterMetaType<std::string>();
    qRegisterMetaType<PacketResponseReason>();  // Your custom enum
    qRegisterMetaType<PeerID>();
    qRegisterMetaType<RoomID>();
    qRegisterMetaType<std::shared_ptr<std::vector<ChatMessage>>>();

    setWindowTitle("Relay IM");

    setMinimumSize(c_minSize);
    resize(c_minSize);

    m_registerDialog = new QRegisterDialog(&m_manager);

    QHBoxLayout* hLayoutMainContent = new QHBoxLayout(this->centralWidget()); // Main Content Layout (ChatRooms, Chat Area)
    hLayoutMainContent->setContentsMargins(10, 10, 10, 10);

    QVBoxLayout* vLayoutChatRooms = new QVBoxLayout();

    QLabel* chatRoomsLabel = new QLabel("Chat Rooms");
    vLayoutChatRooms->addWidget(chatRoomsLabel, 0);

    hLayoutMainContent->addLayout(vLayoutChatRooms, 2);

    QHBoxLayout* buttonLayout = new QHBoxLayout();

    m_roomsListView = new QListView();
    m_roomsListView->setModel(m_manager.GetModelForRooms());
    m_roomsListView->setSelectionMode(QListView::SelectionMode::SingleSelection);
    m_roomsListView->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    vLayoutChatRooms->addWidget(m_roomsListView, 9);

    m_createOrJoinChatRoomButton = new QPushButton("Create/Join");
    m_createOrJoinChatRoomButton->setMaximumHeight(30);
    m_leaveChatRoomButton = new QPushButton("Leave");

    buttonLayout->addWidget(m_createOrJoinChatRoomButton);
    buttonLayout->addWidget(m_leaveChatRoomButton);
    vLayoutChatRooms->addLayout(buttonLayout, 1);

    m_chatWidget = new QChatWidget(&m_manager);

    hLayoutMainContent->addWidget(m_chatWidget, 8);

    m_connectionStatus = new QConnectionStatus();
    ui.m_statusBar->addWidget(m_connectionStatus);
    ui.m_statusBar->setStyleSheet("QStatusBar { background: gray }"); // TEMP(Salads): status bar QSS

    m_manager.Initialize();
    InitializeSignalConnections();

    // Sets chat widget room model when room on left is selected
    connect(m_roomsListView, &QListView::clicked, this, [this](const QModelIndex& clickedItem)
    {
        // NOTE(Salads): Via Docs: clicked emitted only when index is valid.
        QVariant roomID = clickedItem.data(QChatRoomsModel::Role::RoomIDRole);
        m_chatWidget->SetRoomID(static_cast<RoomID>(roomID.toUInt()));
    });

    connect(m_createOrJoinChatRoomButton, &QPushButton::clicked, this, [this](bool checked)
    {
        QChatRoomsDialog diag(&m_manager);
        diag.exec();
    });

    // Leave ChatRoom Packet
    connect(m_leaveChatRoomButton, &QPushButton::clicked, this, [this](bool checked)
    {
        QModelIndex currentIndex = m_roomsListView->currentIndex();
        if(currentIndex.isValid())
        {
            RoomID roomID = m_manager.GetModelForRooms()->data(currentIndex, QChatRoomsModel::Role::RoomIDRole).toUInt();
            m_manager.GetClient()->SendLeaveChatRoom(roomID);
        }
    });

    connect(m_roomsListView->selectionModel(), &QItemSelectionModel::currentChanged, this, [this](const QModelIndex& current, const QModelIndex& prev)
    {
        bool isSomethingSelected = current.isValid();
        m_leaveChatRoomButton->setEnabled(isSomethingSelected);

        if(isSomethingSelected)
        {
            RoomID roomID = m_manager.GetModelForRooms()->data(current, QChatRoomsModel::Role::RoomIDRole).toUInt();
            m_chatWidget->SetRoomID(roomID);
        }
    });
}

void RelayClient::SetCurrentRoom(RoomID roomID)
{
    QModelIndex newRoomIdx = m_manager.GetChatRoomIdx(roomID);
    if(newRoomIdx.isValid())
    {
        // The signal connection for currentChanged -> chat widget should handle chat widget room update.
        m_roomsListView->setCurrentIndex(newRoomIdx);
    }
}

void RelayClient::InitializeSignalConnections()
{
    connect(&m_manager, &QModelManager::Event_JoinRoomResponse,            this, &RelayClient::Slot_JoinRoomResponse);
    connect(&m_manager, &QModelManager::Event_CreateRoomResponse,          this, &RelayClient::Slot_CreateRoomResponse);
    connect(&m_manager, &QModelManager::Event_RoomUpdate_UserAboutToLeave, this, &RelayClient::Slot_RoomUpdate_UserLeft);
}

void RelayClient::Slot_JoinRoomResponse(PacketResponseReason reason, RoomID newRoomID, std::string newChatRoomName)
{
    if(reason == PacketResponseReason::Success)
    {
        SetCurrentRoom(newRoomID); // Set current room to our newly joined room
    }
}

void RelayClient::Slot_CreateRoomResponse(PacketResponseReason reason, RoomID newRoomID, std::string newChatRoomName)
{
    if(reason == PacketResponseReason::Success)
    {
        SetCurrentRoom(newRoomID); // Set current room to our newly created room
    }
}

void RelayClient::Slot_RoomUpdate_UserLeft(RoomID roomID, PeerID peerID)
{
    if(peerID == m_manager.GetLocalPeerID())
    {
        if(m_chatWidget->GetRoomID() == roomID)
        {
            m_chatWidget->SetRoomID(INVALID_PEER_ID);
        }
    }
}

void RelayClient::TryConnect()
{
    m_connectionStatus->SetStatus(QConnectionStatus::Status::Connecting);
    if(m_manager.Connect())
    {
        m_connectionStatus->SetStatus(QConnectionStatus::Status::ConnectedUnregistered);
        m_registerDialog->exec();
        m_connectionStatus->SetStatus(QConnectionStatus::Status::ConnectedRegistered);
    }
    else
    {
        m_connectionStatus->SetStatus(QConnectionStatus::Status::NotConnected);
        QErrorMessage diag;
        diag.showMessage("Could not connect to server. Exiting...");
        diag.exec();
        exit(-1);
    }
}

RelayClient::~RelayClient()
{
    m_manager.Shutdown();
}

