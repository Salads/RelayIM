#include "QChatRoomsDialog.h"

QChatRoomsDialog::QChatRoomsDialog(QModelManager* manager, QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);

    m_manager = manager;
    
    QVBoxLayout* mainVLayout = new QVBoxLayout(this);
    QVBoxLayout* joinLayout = new QVBoxLayout();
    QHBoxLayout* joinButtonLayout = new QHBoxLayout();
    QVBoxLayout* createMainLayout = new QVBoxLayout();
    QHBoxLayout* createLayout = new QHBoxLayout();
    QHBoxLayout* createButtonLayout = new QHBoxLayout();

    QGroupBox* joinGroupBox = new QGroupBox();
    QGroupBox* createGroupBox = new QGroupBox();

    joinGroupBox->setTitle("Join Chatroom");
    createGroupBox->setTitle("Create Chatroom");

    mainVLayout->addWidget(joinGroupBox);
    mainVLayout->addWidget(createGroupBox);

    createGroupBox->setLayout(createMainLayout);
    joinGroupBox->setLayout(joinLayout);

    createMainLayout->addLayout(createLayout);
    createMainLayout->addLayout(createButtonLayout);

    m_joinableRoomsListView = new QListView();
    m_joinableRoomsListView->setModel(&m_model);
    m_joinableRoomsListView->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
    m_joinRoomButton = new QPushButton("Join Room");
    m_joinRoomButton->setMaximumWidth(125);
    m_joinRoomButton->setDisabled(true);

    m_createRoomLineEdit = new QLineEdit();
    m_createRoomLabel = new QLabel("New Room Name:");
    m_createButton = new QPushButton("Create");
    m_createButton->setMaximumWidth(125);

    joinLayout->addWidget(m_joinableRoomsListView);
    joinLayout->addLayout(joinButtonLayout);
    joinButtonLayout->addStretch();
    joinButtonLayout->addWidget(m_joinRoomButton);

    createLayout->addWidget(m_createRoomLabel);
    createLayout->addStretch();
    createLayout->addWidget(m_createRoomLineEdit);
    createButtonLayout->addStretch();
    createButtonLayout->addWidget(m_createButton);

    connect(m_manager, &QModelManager::Event_JoinRoomResponse, this, &QChatRoomsDialog::Slot_JoinRoomResponse);
    connect(m_joinableRoomsListView->selectionModel(), &QItemSelectionModel::currentChanged, this, &QChatRoomsDialog::Slot_JoinableRoomSelected);
    connect(m_joinRoomButton, &QPushButton::clicked, this, &QChatRoomsDialog::Slot_JoinRoomButtonClicked);
    connect(m_manager, &QModelManager::Event_ListChatRoomsResponse, this, &QChatRoomsDialog::Slot_JoinableChatRoomsReceived);
    connect(m_createButton, &QPushButton::clicked, this, &QChatRoomsDialog::Slot_CreateRoomButtonClicked);
    connect(m_manager, &QModelManager::Event_CreateRoomResponse, this, &QChatRoomsDialog::Slot_CreateRoomResponse);

    m_manager->GetClient()->SendRequestAllChatRooms();
}

void QChatRoomsDialog::Slot_CreateRoomResponse(PacketResponseReason reason, RoomID newRoomID, std::string newChatRoomName)
{
    if(reason == PacketResponseReason::Success)
    {
        close();
    }
    else
    {
        QMessageBox diag;
        diag.setText("Chat room name taken!");
        diag.exec();

        m_createButton->setText("Create");
    }
}

void QChatRoomsDialog::Slot_CreateRoomButtonClicked(bool checked)
{
    std::string desiredRoomname = m_createRoomLineEdit->text().toStdString();
    PacketResponseReason checkResult = CheckRoomname(desiredRoomname);
    if(checkResult == PacketResponseReason::Success)
    {
        m_manager->GetClient()->SendCreateChatRoom(m_createRoomLineEdit->text().toStdString());
        m_createButton->setDisabled(true);
        m_createButton->setText("Creating...");
    }
    else
    {
        std::string errorDesc;
        switch(checkResult)
        {
            case PacketResponseReason::ChatRoomNameInvalid:
                errorDesc = "Roomname must not be empty, and must NOT start with a space.";
                break;
            case PacketResponseReason::ChatRoomNameTaken:
                errorDesc = "Roomname is taken.";
                break;
            default:
                errorDesc = "Unknown error";
                break;
        }

        QMessageBox p;
        p.setWindowTitle("Roomname Error");
        p.setText(QString::fromStdString(errorDesc));
        p.exec();
    }
}

void QChatRoomsDialog::Slot_JoinableChatRoomsReceived(std::shared_ptr<std::vector<ChatRoomInfo>> chatRooms)
{
    // Remove chat rooms that have already been joined.
    std::shared_ptr<std::vector<ChatRoomInfo>> filteredRooms = std::make_shared<std::vector<ChatRoomInfo>>();
    std::vector<ChatRoomInfo>& vec = *chatRooms.get();
    for(ChatRoomInfo& room : vec)
    {
        if(!m_manager->HasJoinedRoom(room.m_roomID))
        {
            filteredRooms->push_back(room);
        }
    }

    m_model.ReplaceAll(filteredRooms);
}

void QChatRoomsDialog::Slot_JoinRoomButtonClicked(bool checked)
{
    QModelIndex currentIdx = m_joinableRoomsListView->currentIndex();
    if(currentIdx.isValid())
    {
        RoomID roomID(m_model.data(currentIdx, QChatRoomsModel::Role::RoomIDRole).toUInt());
        m_manager->GetClient()->SendJoinChatRoom(roomID);
        m_joinRoomButton->setText("Joining...");
        m_joinRoomButton->setDisabled(true);
    }
}

void QChatRoomsDialog::Slot_JoinableRoomSelected(const QModelIndex& current, const QModelIndex& prev)
{
    m_joinRoomButton->setDisabled(!current.isValid());
}

void QChatRoomsDialog::Slot_JoinRoomResponse(PacketResponseReason reason, RoomID roomID, std::string newRoomName)
{
    if(reason == PacketResponseReason::Success)
    {
        Log::Get()->ConditionalWriteLine(LOG_NETWORK_EVENTS, "Joined Room (%s), (ID=%u)", newRoomName, roomID);
        close();
    }
    else
    {
        std::string popupText = "Could not join room '" + newRoomName + "'. Reason: " + ResponseTypeToString(reason);

        QMessageBox diag;
        diag.setWindowTitle("Join Error");
        diag.setText(QString::fromStdString(popupText));
        diag.exec();

        m_joinRoomButton->setEnabled(true);
    }
}

PacketResponseReason QChatRoomsDialog::CheckRoomname(const std::string& newRoomname)
{
    if(newRoomname.empty() || newRoomname[0] == ' ')
    {
        return PacketResponseReason::ChatRoomNameInvalid;
    }

    if(m_model.RoomExists(newRoomname) || m_manager->GetModelForRooms()->RoomExists(newRoomname))
    {
        return PacketResponseReason::ChatRoomNameTaken;
    }

    return PacketResponseReason::Success;
}

QChatRoomsDialog::~QChatRoomsDialog()
{}

