#include "RelayClient.h"

RelayClient::RelayClient(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this); // Generated UI from visual editor

    m_registerDialog = new QRegisterDialog(&m_manager);

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

    m_createOrJoinChatRoomButton = new QPushButton("Create/Join");
    m_createOrJoinChatRoomButton->setMaximumHeight(30);

    buttonLayout->addWidget(m_createOrJoinChatRoomButton);
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

    m_connectionStatus = new QConnectionStatus();
    ui.m_statusBar->addWidget(m_connectionStatus);
    ui.m_statusBar->setStyleSheet("QStatusBar { background: gray }"); // TEMP(Salads): status bar QSS

    m_manager.Initialize();

    connect(m_createOrJoinChatRoomButton, &QPushButton::clicked, this, [this](bool checked)
    {
        QChatRoomsDialog diag(&m_manager);
        diag.exec();
    });
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

