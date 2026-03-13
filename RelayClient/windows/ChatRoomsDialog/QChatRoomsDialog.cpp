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
    m_joinableRoomsListView->setModel(m_manager->GetModelForRooms().get());
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

    connect(m_manager, &QModelManager::Event_JoinRoomResponse, this, [this](PacketResponseReason reason, RoomID roomID, std::string newRoomName)
    {
        if(reason == PacketResponseReason::Success)
        {
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
    }, Qt::QueuedConnection);

    connect(m_joinableRoomsListView->selectionModel(), &QItemSelectionModel::currentChanged, this, [this](const QModelIndex &current, const QModelIndex &prev)
    {
        m_joinRoomButton->setDisabled(!current.isValid());
    });

    connect(m_joinRoomButton, &QPushButton::clicked, this, [this](bool checked)
    {
        QModelIndex currentIdx = m_joinableRoomsListView->currentIndex();
        if(currentIdx.isValid())
        {
            RoomID roomID = m_manager->GetModelForRooms()->data(currentIdx, QChatRoomsModel::Role::RoomIDRole).toUInt();
            m_manager->GetClient()->SendJoinChatRoom(roomID);
            m_joinRoomButton->setText("Joining...");
            m_joinRoomButton->setDisabled(true);
        }
    });

    connect(m_manager, &QModelManager::Event_ListChatRoomsResponse, this, [this](std::shared_ptr<std::vector<ChatRoomInfo>> chatRooms)
    {
        m_manager->GetModelForRooms()->ReplaceAll(chatRooms);

    }, Qt::QueuedConnection);

    connect(m_createButton, &QPushButton::clicked, this, [this](bool checked)
    {
        if(!m_createRoomLineEdit->text().isEmpty())
        {
            m_manager->GetClient()->SendCreateChatRoom(m_createRoomLineEdit->text().toStdString());
            m_createButton->setDisabled(true);
            m_createButton->setText("Creating...");
        }
    });

    connect(m_manager, &QModelManager::Event_CreateRoomResponse, this, [this](PacketResponseReason reason)
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
    }, Qt::QueuedConnection);

    m_manager->GetClient()->SendRequestAllChatRooms();
}

QChatRoomsDialog::~QChatRoomsDialog()
{}

