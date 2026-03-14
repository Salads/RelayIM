#include <QObject>

#include "models/QModelManager/QModelManager.h"

QModelManager::QModelManager(QObject* parent)
    : QObject(parent)
{}

QModelManager::~QModelManager()
{
    for(auto [roomID, model] : m_chatRoomMessagesModels.asKeyValueRange())
    {
        delete model;
    }

    m_chatRoomMessagesModels.clear();
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

void QModelManager::SetLocalPeerID(PeerID peerID)
{
    m_localPeerID = peerID;
}

void QModelManager::AddKnownUser(PeerID peerID, const std::string& username)
{
    m_knownUsers[peerID] = username;
}

void QModelManager::AddUserToRoom(PeerID peerID, RoomID roomID)
{
    m_userRooms[peerID].insert(roomID);
}

QModelIndex QModelManager::GetChatRoomIdx(RoomID roomID)
{
    qsizetype rowIdx = m_joinedChatRoomsModel.FindRoom(roomID);
    if(rowIdx == -1)
    {
        return QModelIndex();
    }

    return m_joinedChatRoomsModel.index(rowIdx);
}

bool QModelManager::HasJoinedRoom(RoomID roomID)
{
    return m_joinedChatRoomsModel.FindRoom(roomID) != -1;
}

void QModelManager::DeleteRoomMessagesModel(RoomID roomID)
{
    if(m_chatRoomMessagesModels.contains(roomID))
    {
        QChatRoomMessagesModel* old = m_chatRoomMessagesModels[roomID];
        m_chatRoomMessagesModels.remove(roomID);
        delete old;
    }
}

void QModelManager::AddJoinedChatRoom(RoomID roomID, const std::string& roomname)
{
    m_joinedChatRoomsModel.AddJoinedChatRoom(roomID, QString::fromStdString(roomname));

    DeleteRoomMessagesModel(roomID);

    m_chatRoomMessagesModels[roomID] = new QChatRoomMessagesModel();
    m_chatRoomMessagesModels[roomID]->Initialize(&m_knownUsers, roomID, roomname);
}

void QModelManager::RemoveJoinedChatRoom(RoomID roomID)
{
    m_joinedChatRoomsModel.RemoveJoinedChatRoom(roomID);
    DeleteRoomMessagesModel(roomID);
}

void QModelManager::AddMessageToRoom(RoomID roomID, PeerID peerID, const std::string& message)
{
    if(m_chatRoomMessagesModels.contains(roomID))
    {
        m_chatRoomMessagesModels[roomID]->AddMessage(peerID, message);
    }
    else
    {
        Log::Get()->ConditionalWriteLine(LOG_NETWORK_EVENTS, "Tried to add message to non-existing room! (Room:%u, Peer:%u)", roomID, peerID);
    }
}

void QModelManager::RemoveUserFromRoom(PeerID peerID, RoomID roomID)
{
    m_userRooms[peerID].remove(roomID);
}

PacketResponseReason QModelManager::CheckDesiredUsername(const std::string& desiredUsername)
{
    if(desiredUsername.empty() || desiredUsername[0] == ' ')
    {
        return PacketResponseReason::UsernameInvalid;
    }

    for(auto [key, val] : m_knownUsers.asKeyValueRange())
    {
        if(val == desiredUsername)
        {
            return PacketResponseReason::UsernameTaken;
        }
    }

    return PacketResponseReason::Success;
}

void QModelManager::InitializeClientCallbacks()
{
    if (m_callbacksInitialized) { return; }

    m_client.OnRegisterResponse = [this](PacketResponseReason reason, PeerID peerID, std::string username)
    {
        emit Event_RegisterResponse(reason, peerID, username);
    };

    m_client.OnListChatRoomsResponse = [this](std::shared_ptr<std::vector<ChatRoomInfo>> chatRooms)
    {
        emit Event_ListChatRoomsResponse(chatRooms);
    }; // ListChatRooms slot not needed, it's temporary data for when user is joining a chat room.

    m_client.OnJoinRoomResponse = [this](PacketResponseReason reason, RoomID newRoomID, std::string newChatRoomName)
    {
        emit Event_JoinRoomResponse(reason, newRoomID, newChatRoomName);
    };

    m_client.OnCreateRoomResponse = [this](PacketResponseReason reason, RoomID newRoomID, std::string newChatRoomName)
    {
        emit Event_CreateRoomResponse(reason, newRoomID, newChatRoomName);
    };

    m_client.OnRoomUpdate_NewMessage = [this](RoomID roomID, PeerID peerID, std::string message)
    {
        emit Event_RoomUpdate_Message(roomID, peerID, message);
    };

    m_client.OnRoomUpdate_FullUpdate = [this](RoomID roomID, std::shared_ptr<std::vector<ChatMessage>> messages)
    {
        emit Event_RoomUpdate_FULL(roomID, messages);
    };

    m_client.OnRoomUpdate_UserJoined = [this](RoomID roomID, PeerID newPeerID, std::string newName)
    {
        emit Event_RoomUpdate_UserJoined(roomID, newPeerID, newName);
    };

    m_client.OnRoomUpdate_UserLeft = [this](RoomID roomID, PeerID peerID)
    {
        emit Event_RoomUpdate_UserLeft(roomID, peerID);
    };

    m_callbacksInitialized = true;
}

PeerID QModelManager::GetLocalPeerID()
{
    return m_localPeerID;
}

QChatRoomMessagesModel* QModelManager::GetModelForRoom(RoomID roomID)
{
    if (!m_chatRoomMessagesModels.contains(roomID))
    {
        return nullptr;
    }

    return m_chatRoomMessagesModels[roomID];
}

QChatRoomsModel* QModelManager::GetModelForRooms()
{
    return &m_joinedChatRoomsModel;
}

RelayIMClient* QModelManager::GetClient()
{
    return &m_client;
}
