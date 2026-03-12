#include <QObject>

#include "models/QModelManager/QModelManager.h"

QModelManager::QModelManager(QObject* parent) 
    : QObject(parent)
{
    m_chatRoomsModel = std::make_shared<QChatRoomsModel>();
}

bool QModelManager::Initialize()
{
    InitializeClientCallbacks();

    return m_client.Initialize();
}

bool QModelManager::Connect()
{
    return m_client.Connect();
}

void QModelManager::Shutdown()
{
    m_client.Shutdown();
}

void QModelManager::Slot_RegisterResponse(PacketResponseReason reason, PeerID peerID, std::string username)
{
    if(reason == PacketResponseReason::Success)
    {
        m_knownUsers[peerID] = username;
        m_localPeerID = peerID;
    }
}

void QModelManager::Slot_JoinRoomResponse(PacketResponseReason reason, RoomID newRoomID, std::string newChatRoomName)
{
    if(reason == PacketResponseReason::Success)
    {
        m_userRooms[m_localPeerID].insert(newRoomID);
        m_localRoomID = newRoomID;
        m_chatRoomsModel->AddChatRoom(newRoomID, QString::fromStdString(newChatRoomName));
        m_chatRoomMessagesModels[newRoomID] = std::make_shared<QChatRoomMessagesModel>();
        m_chatRoomMessagesModels[newRoomID]->Initialize(&m_knownUsers, newRoomID, newChatRoomName);
        
    }
}

void QModelManager::Slot_CreateRoomResponse(PacketResponseReason reason, RoomID newRoomID, std::string newChatRoomName)
{
    if(reason == PacketResponseReason::Success)
    {
        m_userRooms[m_localPeerID].insert(newRoomID);
        m_localRoomID = newRoomID;
        m_chatRoomsModel->AddChatRoom(newRoomID, QString::fromStdString(newChatRoomName));
        m_chatRoomMessagesModels[newRoomID] = std::make_shared<QChatRoomMessagesModel>();
        m_chatRoomMessagesModels[newRoomID]->Initialize(&m_knownUsers, newRoomID, newChatRoomName);
    }
}

void QModelManager::Slot_RoomUpdate_Message(RoomID roomID, PeerID peerID, std::string message)
{
    if(m_chatRoomMessagesModels.contains(roomID))
    {
        m_chatRoomMessagesModels[roomID]->AddMessage(peerID, QString::fromStdString(message));
    }
}

void QModelManager::Slot_RoomUpdate_FULL(RoomID roomID, std::shared_ptr<std::vector<ChatMessage>> messages)
{
    std::vector<ChatMessage>* pMessages = messages.get();
    for(int i = 0; i < pMessages->size(); i++)
    {
        m_chatRoomMessagesModels[roomID]->AddMessage(pMessages->at(i).m_senderID, QString::fromStdString(pMessages->at(i).m_message));
    }
}

void QModelManager::Slot_RoomUpdate_UserJoined(RoomID roomID, PeerID newPeerID, std::string newName)
{
    m_knownUsers[newPeerID] = newName;
    m_userRooms[newPeerID].insert(roomID);
}

void QModelManager::Slot_RoomUpdate_UserLeft(RoomID roomID, PeerID peerID)
{
    m_userRooms[peerID].remove(roomID);
}

void QModelManager::InitializeClientCallbacks()
{
    if (m_callbacksInitialized) { return; }

    m_client.OnRegisterResponse = [this](PacketResponseReason reason, PeerID peerID, std::string username)
    {
        emit Event_RegisterResponse(reason, peerID, username);
    };
    connect(this, &QModelManager::Event_RegisterResponse, this, &QModelManager::Slot_RegisterResponse, Qt::QueuedConnection);

    m_client.OnListChatRoomsResponse = [this](std::shared_ptr<std::vector<ChatRoomInfo>> chatRooms)
    {
        emit Event_ListChatRoomsResponse(chatRooms);
    }; // ListChatRooms slot not needed, it's temporary data for when user is joining a chat room.

    m_client.OnJoinRoomResponse = [this](PacketResponseReason reason, RoomID newRoomID, std::string newChatRoomName)
    {
        emit Event_JoinRoomResponse(reason, newRoomID, newChatRoomName);
    };
    connect(this, &QModelManager::Event_JoinRoomResponse, this, &QModelManager::Slot_JoinRoomResponse, Qt::QueuedConnection);

    m_client.OnCreateRoomResponse = [this](PacketResponseReason reason, RoomID newRoomID, std::string newChatRoomName)
    {
        emit Event_CreateRoomResponse(reason, newRoomID, newChatRoomName);
    };
    connect(this, &QModelManager::Event_CreateRoomResponse, this, &QModelManager::Slot_CreateRoomResponse, Qt::QueuedConnection);

    m_client.OnRoomUpdate_NewMessage = [this](RoomID roomID, PeerID peerID, std::string message)
    {
        emit Event_RoomUpdate_Message(roomID, peerID, message);
    };
    connect(this, &QModelManager::Event_RoomUpdate_Message, this, &QModelManager::Slot_RoomUpdate_Message, Qt::QueuedConnection);

    m_client.OnRoomUpdate_FullUpdate = [this](RoomID roomID, std::shared_ptr<std::vector<ChatMessage>> messages)
    {
        emit Event_RoomUpdate_FULL(roomID, messages);
    };
    connect(this, &QModelManager::Event_RoomUpdate_FULL, this, &QModelManager::Slot_RoomUpdate_FULL, Qt::QueuedConnection);

    m_client.OnRoomUpdate_UserJoined = [this](RoomID roomID, PeerID newPeerID, std::string newName)
    {
        emit Event_RoomUpdate_UserJoined(roomID, newPeerID, newName);
    };
    connect(this, &QModelManager::Event_RoomUpdate_UserJoined, this, &QModelManager::Slot_RoomUpdate_UserJoined, Qt::QueuedConnection);

    m_client.OnRoomUpdate_UserLeft = [this](RoomID roomID, PeerID peerID)
    {
        emit Event_RoomUpdate_UserLeft(roomID, peerID);
    };
    connect(this, &QModelManager::Event_RoomUpdate_UserLeft, this, &QModelManager::Slot_RoomUpdate_UserLeft, Qt::QueuedConnection);

    m_callbacksInitialized = true;
}

PeerID QModelManager::GetLocalPeerID()
{
    return m_localPeerID;
}

RoomID QModelManager::GetLocalRoomID()
{
    return m_localRoomID;
}

std::shared_ptr<QChatRoomMessagesModel> QModelManager::GetModelForRoom(RoomID roomID)
{
    if (!m_chatRoomMessagesModels.contains(roomID))
    {
        return nullptr;
    }

    return m_chatRoomMessagesModels[roomID];
}

std::shared_ptr<QChatRoomsModel> QModelManager::GetModelForRooms()
{
    return m_chatRoomsModel;
}

RelayIMClient* QModelManager::GetClient()
{
    return &m_client;
}
