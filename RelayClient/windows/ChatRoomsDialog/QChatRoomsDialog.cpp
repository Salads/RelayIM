#include "QChatRoomsDialog.h"

QChatRoomsDialog::QChatRoomsDialog(QModelManager* manager, QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);

    m_manager = manager;
    
    QVBoxLayout* mainVLayout = new QVBoxLayout(this);
    QVBoxLayout* joinLayout = new QVBoxLayout();
    QVBoxLayout* createMainLayout = new QVBoxLayout();
    QHBoxLayout* createLayout = new QHBoxLayout();

    QGroupBox* joinGroupBox = new QGroupBox();
    QGroupBox* createGroupBox = new QGroupBox();

    joinGroupBox->setTitle("Join Chatroom");
    createGroupBox->setTitle("Create Chatroom");

    mainVLayout->addWidget(joinGroupBox);
    mainVLayout->addWidget(createGroupBox);

    createGroupBox->setLayout(createMainLayout);
    joinGroupBox->setLayout(joinLayout);

    createMainLayout->addLayout(createLayout);

    m_joinableRoomsListView = new QListView();
    m_joinableRoomsListView->setModel(&m_model);

    m_createRoomLineEdit = new QLineEdit();
    m_createRoomLabel = new QLabel("New Room Name:");
    m_createButton = new QPushButton("Create");
    m_createButton->setMaximumWidth(125);

    joinLayout->addWidget(m_joinableRoomsListView);
    createLayout->addWidget(m_createRoomLabel);
    createLayout->addStretch();
    createLayout->addWidget(m_createRoomLineEdit);
    createMainLayout->addWidget(m_createButton);

    connect(m_manager, &QModelManager::Event_ListChatRoomsResponse, this, [this](std::shared_ptr<std::vector<ChatRoomInfo>> chatRooms)
    {
        qDebug() << "Event_ListChatRoomsResponse - List Received ";
        m_model.ReplaceAll(chatRooms);
        qDebug() << "Event_ListChatRoomsResponse - Replaced All ";

    }, Qt::QueuedConnection);

    connect(m_createButton, &QPushButton::clicked, this, [this](bool checked)
    {
        if(!m_createRoomLineEdit->text().isEmpty())
        {
            qDebug() << "Create Chat Room - Sending Request";
            m_manager->GetClient()->SendCreateChatRoom(m_createRoomLineEdit->text().toStdString());
            m_createButton->setDisabled(true);
            m_createButton->setText("Creating...");
            qDebug() << "Create Chat Room - Sent Request";
        }
    });

    connect(m_manager, &QModelManager::Event_CreateRoomResponse, this, [this](PacketResponseReason reason)
    {
        qDebug() << "Event_CreateRoomResponse";
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

    qDebug() << "JoinChatRooms - Sending Request ";
    m_manager->GetClient()->SendRequestAllChatRooms();
    qDebug() << "JoinChatRooms - Sent Request";
}

QChatRoomsDialog::~QChatRoomsDialog()
{}

