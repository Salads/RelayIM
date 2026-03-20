#include <QObject>

#include "models/QModelManager/QModelManager.h"

QModelManager::QModelManager(QObject* parent)
    : QObject(parent)
{}

QModelManager::~QModelManager()
{
    m_chatRoomMessages.clear();
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
    if(m_chatRoomMessages.contains(roomID))
    {
        m_chatRoomMessages.remove(roomID);
    }
}

void QModelManager::AddJoinedChatRoom(RoomID roomID, const std::string& roomname)
{
    DeleteRoomMessagesModel(roomID);
    m_joinedChatRoomsModel.AddJoinedChatRoom(roomID, QString::fromStdString(roomname));
    m_chatRoomMessages[roomID] = QList<ChatMessage>();
}

void QModelManager::RemoveJoinedChatRoom(RoomID roomID)
{
    m_joinedChatRoomsModel.RemoveJoinedChatRoom(roomID);
    DeleteRoomMessagesModel(roomID);
}

void QModelManager::AddMessageToRoom(RoomID roomID, PeerID peerID, const std::string& message)
{
    if(m_chatRoomMessages.contains(roomID))
    {
        m_chatRoomMessages[roomID].emplaceBack(peerID, message);
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
        qDebug() << "About to emit signal";
        emit NetEvent_RegisterResponse(reason, peerID, username, QPrivateSignal());
        qDebug() << "Signal emitted";
    };

    m_client.OnListChatRoomsResponse = [this](std::shared_ptr<std::vector<ChatRoomInfo>> chatRooms)
    {
        emit NetEvent_ListChatRoomsResponse(chatRooms, QPrivateSignal());
    };

    m_client.OnJoinRoomResponse = [this](PacketResponseReason reason, RoomID newRoomID, std::string newChatRoomName)
    {
        emit NetEvent_JoinRoomResponse(reason, newRoomID, newChatRoomName, QPrivateSignal());
    };

    m_client.OnCreateRoomResponse = [this](PacketResponseReason reason, RoomID newRoomID, std::string newChatRoomName)
    {
        emit NetEvent_CreateRoomResponse(reason, newRoomID, newChatRoomName, QPrivateSignal());
    };

    m_client.OnRoomUpdate_NewMessage = [this](RoomID roomID, PeerID peerID, std::string message)
    {
        emit NetEvent_RoomUpdate_Message(roomID, peerID, message, QPrivateSignal());
    };

    m_client.OnRoomUpdate_FullUpdate = [this](RoomID roomID, std::shared_ptr<std::vector<ChatMessage>> messages)
    {
        emit NetEvent_RoomUpdate_FULL(roomID, messages, QPrivateSignal());
    };

    m_client.OnRoomUpdate_UserJoined = [this](RoomID roomID, PeerID newPeerID, std::string newName)
    {
        emit NetEvent_RoomUpdate_UserJoined(roomID, newPeerID, newName, QPrivateSignal());
    };

    m_client.OnRoomUpdate_UserLeft = [this](RoomID roomID, PeerID peerID)
    {
        emit NetEvent_RoomUpdate_UserLeft(roomID, peerID, QPrivateSignal());
    };

    connect(this, &QModelManager::NetEvent_RegisterResponse,      this, &QModelManager::NetSlot_RegisterResponse,      static_cast<Qt::ConnectionType>(Qt::BlockingQueuedConnection | Qt::UniqueConnection));
    connect(this, &QModelManager::NetEvent_ListChatRoomsResponse, this, &QModelManager::NetSlot_ListChatRoomsResponse, static_cast<Qt::ConnectionType>(Qt::BlockingQueuedConnection | Qt::UniqueConnection));
    connect(this, &QModelManager::NetEvent_JoinRoomResponse,      this, &QModelManager::NetSlot_JoinRoomResponse,      static_cast<Qt::ConnectionType>(Qt::BlockingQueuedConnection | Qt::UniqueConnection));
    connect(this, &QModelManager::NetEvent_CreateRoomResponse,    this, &QModelManager::NetSlot_CreateRoomResponse,    static_cast<Qt::ConnectionType>(Qt::BlockingQueuedConnection | Qt::UniqueConnection));
    connect(this, &QModelManager::NetEvent_RoomUpdate_Message,    this, &QModelManager::NetSlot_RoomUpdate_Message,    static_cast<Qt::ConnectionType>(Qt::BlockingQueuedConnection | Qt::UniqueConnection));
    connect(this, &QModelManager::NetEvent_RoomUpdate_FULL,       this, &QModelManager::NetSlot_RoomUpdate_FULL,       static_cast<Qt::ConnectionType>(Qt::BlockingQueuedConnection | Qt::UniqueConnection));
    connect(this, &QModelManager::NetEvent_RoomUpdate_UserJoined, this, &QModelManager::NetSlot_RoomUpdate_UserJoined, static_cast<Qt::ConnectionType>(Qt::BlockingQueuedConnection | Qt::UniqueConnection));
    connect(this, &QModelManager::NetEvent_RoomUpdate_UserLeft,   this, &QModelManager::NetSlot_RoomUpdate_UserLeft,   static_cast<Qt::ConnectionType>(Qt::BlockingQueuedConnection | Qt::UniqueConnection));

    m_callbacksInitialized = true;
}

PeerID QModelManager::GetLocalPeerID()
{
    return m_localPeerID;
}

QList<ChatMessage>* QModelManager::GetMessagesForRoom(RoomID roomID)
{
    if (!m_chatRoomMessages.contains(roomID))
    {
        return nullptr;
    }

    return &m_chatRoomMessages[roomID];
}

QChatRoomsModel* QModelManager::GetModelForRooms()
{
    return &m_joinedChatRoomsModel;
}

RelayIMClient* QModelManager::GetClient()
{
    return &m_client;
}

std::string QModelManager::GetUsernameByPeerID(PeerID peerID)
{
    if(m_knownUsers.contains(peerID))
    {
        return m_knownUsers[peerID];
    }
    else
    {
        return "Unknown User";
    }
}

std::string QModelManager::GetRoomnameByRoomID(RoomID roomID)
{
    return m_joinedChatRoomsModel.GetChatRoomInfo(roomID)->m_roomname;
}

void QModelManager::NetSlot_ListChatRoomsResponse(std::shared_ptr<std::vector<ChatRoomInfo>> messages, QPrivateSignal)
{
    emit Event_ListChatRoomsResponse(messages);
}

void QModelManager::NetSlot_RegisterResponse(PacketResponseReason reason, PeerID peerID, std::string username, QPrivateSignal)
{
    qDebug() << "NetSlot_RegisterResponse - emitting Event_RegisterReponse";

    if(reason == PacketResponseReason::Success)
    {
        AddKnownUser(peerID, username);
        SetLocalPeerID(peerID);
    }

    emit Event_RegisterResponse(reason, peerID, username);
}

void QModelManager::NetSlot_JoinRoomResponse(PacketResponseReason reason, RoomID newRoomID, std::string newChatRoomName, QPrivateSignal)
{
    if(reason == PacketResponseReason::Success)
    {
        AddJoinedChatRoom(newRoomID, newChatRoomName);
        AddUserToRoom(GetLocalPeerID(), newRoomID);
    }

    emit Event_JoinRoomResponse(reason, newRoomID, newChatRoomName);
}

void QModelManager::NetSlot_CreateRoomResponse(PacketResponseReason reason, RoomID newRoomID, std::string newChatRoomName, QPrivateSignal)
{
    if(reason == PacketResponseReason::Success)
    {
        Log::Get()->ConditionalWriteLine(LOG_NETWORK_EVENTS, "Created Room: %s (ID=%u)", newChatRoomName, newRoomID);
        AddJoinedChatRoom(newRoomID, newChatRoomName);
        AddUserToRoom(GetLocalPeerID(), newRoomID);
    }

    emit Event_CreateRoomResponse(reason, newRoomID, newChatRoomName);
}

void QModelManager::NetSlot_RoomUpdate_Message(RoomID roomID, PeerID peerID, std::string message, QPrivateSignal _)
{
    AddMessageToRoom(roomID, peerID, message);
    emit Event_RoomUpdate_Message(roomID, peerID, message);
}

void QModelManager::NetSlot_RoomUpdate_FULL(RoomID roomID, std::shared_ptr<std::vector<ChatMessage>> messages, QPrivateSignal)
{
    std::vector<ChatMessage>* pMessages = messages.get();
    for(int i = 0; i < pMessages->size(); i++)
    {
        const ChatMessage& message = pMessages->at(i);
        AddMessageToRoom(roomID, message.m_senderID, message.m_message);
    }

    emit Event_RoomUpdate_FULL(roomID, messages);
}

void QModelManager::NetSlot_RoomUpdate_UserJoined(RoomID roomID, PeerID newPeerID, std::string newName, QPrivateSignal)
{
    AddKnownUser(newPeerID, newName);
    AddUserToRoom(newPeerID, roomID);

    emit Event_RoomUpdate_UserJoined(roomID, newPeerID, newName);
}

void QModelManager::NetSlot_RoomUpdate_UserLeft(RoomID roomID, PeerID peerID, QPrivateSignal)
{
    emit Event_RoomUpdate_UserAboutToLeave(roomID, peerID);

    RemoveUserFromRoom(peerID, roomID);
    if(peerID == GetLocalPeerID())
    {
        RemoveJoinedChatRoom(roomID);
    }
}
