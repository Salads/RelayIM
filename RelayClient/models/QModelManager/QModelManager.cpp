#include <QObject>

#include "models/QModelManager/QModelManager.h"

QModelManager::QModelManager(QObject* parent)
    : QObject(parent), m_client(this)
{}

QModelManager::~QModelManager()
{
    m_chatRoomMessages.clear();
}

bool QModelManager::initialize()
{
    initializeClientCallbacks();

    return m_client.initialize();
}

bool QModelManager::connectToServer()
{
    return m_client.connectToServer();
}

void QModelManager::shutdown()
{
    m_client.shutdownClient();
}

void QModelManager::setLocalPeerId(PeerID peerID)
{
    m_localPeerId = peerID;
}

void QModelManager::addKnownUser(PeerID peerID, const std::string& username)
{
    m_knownUsers[peerID] = username;
}

void QModelManager::addUserToRoom(PeerID peerID, RoomID roomID)
{
    m_userRooms[peerID].insert(roomID);
}

QModelIndex QModelManager::getChatRoomIdx(RoomID roomID)
{
    qsizetype rowIdx = m_joinedChatRoomsModel.findRoom(roomID);
    if(rowIdx == -1)
    {
        return QModelIndex();
    }

    return m_joinedChatRoomsModel.index(rowIdx);
}

bool QModelManager::hasJoinedRoom(RoomID roomID)
{
    return m_joinedChatRoomsModel.findRoom(roomID) != -1;
}

void QModelManager::deleteRoomMessagesModel(RoomID roomID)
{
    if(m_chatRoomMessages.contains(roomID))
    {
        m_chatRoomMessages.remove(roomID);
    }
}

void QModelManager::addJoinedChatRoom(RoomID roomID, const std::string& roomname)
{
    deleteRoomMessagesModel(roomID);
    m_joinedChatRoomsModel.addJoinedChatRoom(roomID, QString::fromStdString(roomname));
    m_chatRoomMessages[roomID] = QList<ChatMessage>();
}

void QModelManager::removeJoinedChatRoom(RoomID roomID)
{
    m_joinedChatRoomsModel.removeJoinedChatRoom(roomID);
    deleteRoomMessagesModel(roomID);
}

void QModelManager::addMessageToRoom(RoomID roomID, PeerID peerID, const std::string& message)
{
    if(m_chatRoomMessages.contains(roomID))
    {
        m_chatRoomMessages[roomID].emplaceBack(peerID, message);
    }
    else
    {
        Log::get()->conditionalWriteLine(LOG_NETWORK_EVENTS, "Tried to add message to non-existing room! (Room:%u, Peer:%u)", roomID, peerID);
    }
}

void QModelManager::removeUserFromRoom(PeerID peerID, RoomID roomID)
{
    m_userRooms[peerID].remove(roomID);
}

PacketResponseReason QModelManager::checkDesiredUsername(const std::string& desiredUsername)
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

void QModelManager::initializeClientCallbacks()
{
    if (m_callbacksInitialized) { return; }


    connect(this, &QModelManager::netEventRegisterResponse,      this, &QModelManager::netSlotRegisterResponse,      static_cast<Qt::ConnectionType>(Qt::BlockingQueuedConnection | Qt::UniqueConnection));
    connect(this, &QModelManager::netEventListChatRoomsResponse, this, &QModelManager::netSlotListChatRoomsResponse, static_cast<Qt::ConnectionType>(Qt::BlockingQueuedConnection | Qt::UniqueConnection));
    connect(this, &QModelManager::netEventJoinRoomResponse,      this, &QModelManager::netSlotJoinRoomResponse,      static_cast<Qt::ConnectionType>(Qt::BlockingQueuedConnection | Qt::UniqueConnection));
    connect(this, &QModelManager::netEventCreateRoomResponse,    this, &QModelManager::netSlotCreateRoomResponse,    static_cast<Qt::ConnectionType>(Qt::BlockingQueuedConnection | Qt::UniqueConnection));
    connect(this, &QModelManager::netEventRoomUpdateMessage,    this, &QModelManager::netSlotRoomUpdateMessage,    static_cast<Qt::ConnectionType>(Qt::BlockingQueuedConnection | Qt::UniqueConnection));
    connect(this, &QModelManager::netEventRoomUpdateFull,       this, &QModelManager::netSlotRoomUpdateFull,       static_cast<Qt::ConnectionType>(Qt::BlockingQueuedConnection | Qt::UniqueConnection));
    connect(this, &QModelManager::netEventRoomUpdateUserJoined, this, &QModelManager::netSlotRoomUpdateUserJoined, static_cast<Qt::ConnectionType>(Qt::BlockingQueuedConnection | Qt::UniqueConnection));
    connect(this, &QModelManager::netEventRoomUpdateUserLeft,   this, &QModelManager::netSlotRoomUpdateUserLeft,   static_cast<Qt::ConnectionType>(Qt::BlockingQueuedConnection | Qt::UniqueConnection));

    m_callbacksInitialized = true;
}

void QModelManager::onRegisterResponse(PacketResponseReason reason, PeerID peerID, std::string username)
{
    emit netEventRegisterResponse(reason, peerID, username, QPrivateSignal());
}

void QModelManager::onListChatRoomsResponse(std::shared_ptr<std::vector<ChatRoomInfo>> chatRooms)
{
    emit netEventListChatRoomsResponse(chatRooms, QPrivateSignal());
}

void QModelManager::onJoinRoomResponse(PacketResponseReason reason, RoomID newRoomID, std::string newChatRoomName)
{
    emit netEventJoinRoomResponse(reason, newRoomID, newChatRoomName, QPrivateSignal());
}

void QModelManager::onCreateRoomResponse(PacketResponseReason reason, RoomID newRoomID, std::string newChatRoomName)
{
    emit netEventCreateRoomResponse(reason, newRoomID, newChatRoomName, QPrivateSignal());
}

void QModelManager::onRoomUpdateNewMessage(RoomID roomID, PeerID peerID, std::string message)
{
    emit netEventRoomUpdateMessage(roomID, peerID, message, QPrivateSignal());
}

void QModelManager::onRoomUpdateFullUpdate(RoomID roomID, std::shared_ptr<std::vector<ChatMessage>> messages)
{
    emit netEventRoomUpdateFull(roomID, messages, QPrivateSignal());
}

void QModelManager::onRoomUpdateUserJoined(RoomID roomID, PeerID newPeerID, std::string newName)
{
    emit netEventRoomUpdateUserJoined(roomID, newPeerID, newName, QPrivateSignal());
}

void QModelManager::onRoomUpdateUserLeft(RoomID roomID, PeerID peerID)
{
    emit netEventRoomUpdateUserLeft(roomID, peerID, QPrivateSignal());
}

PeerID QModelManager::getLocalPeerId()
{
    return m_localPeerId;
}

QList<ChatMessage>* QModelManager::getMessagesForRoom(RoomID roomID)
{
    if (!m_chatRoomMessages.contains(roomID))
    {
        return nullptr;
    }

    return &m_chatRoomMessages[roomID];
}

QChatRoomsModel* QModelManager::getModelForRooms()
{
    return &m_joinedChatRoomsModel;
}

RelayIMClient* QModelManager::getClient()
{
    return &m_client;
}

std::string QModelManager::getUsernameByPeerId(PeerID peerID)
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

std::string QModelManager::getRoomnameByRoomId(RoomID roomID)
{
    return m_joinedChatRoomsModel.getChatRoomInfo(roomID)->m_roomname;
}

void QModelManager::netSlotListChatRoomsResponse(std::shared_ptr<std::vector<ChatRoomInfo>> messages, QPrivateSignal)
{
    emit eventListChatRoomsResponse(messages);
}

void QModelManager::netSlotRegisterResponse(PacketResponseReason reason, PeerID peerID, std::string username, QPrivateSignal)
{
    qDebug() << "netSlotRegisterResponse - emitting eventRegisterResponse";

    if(reason == PacketResponseReason::Success)
    {
        addKnownUser(peerID, username);
        setLocalPeerId(peerID);
    }

    emit eventRegisterResponse(reason, peerID, username);
}

void QModelManager::netSlotJoinRoomResponse(PacketResponseReason reason, RoomID newRoomID, std::string newChatRoomName, QPrivateSignal)
{
    if(reason == PacketResponseReason::Success)
    {
        addJoinedChatRoom(newRoomID, newChatRoomName);
        addUserToRoom(getLocalPeerId(), newRoomID);
    }

    emit eventJoinRoomResponse(reason, newRoomID, newChatRoomName);
}

void QModelManager::netSlotCreateRoomResponse(PacketResponseReason reason, RoomID newRoomID, std::string newChatRoomName, QPrivateSignal)
{
    if(reason == PacketResponseReason::Success)
    {
        Log::get()->conditionalWriteLine(LOG_NETWORK_EVENTS, "Created Room: %s (ID=%u)", newChatRoomName, newRoomID);
        addJoinedChatRoom(newRoomID, newChatRoomName);
        addUserToRoom(getLocalPeerId(), newRoomID);
    }

    emit eventCreateRoomResponse(reason, newRoomID, newChatRoomName);
}

void QModelManager::netSlotRoomUpdateMessage(RoomID roomID, PeerID peerID, std::string message, QPrivateSignal _)
{
    addMessageToRoom(roomID, peerID, message);
    emit eventRoomUpdateMessage(roomID, peerID, message);
}

void QModelManager::netSlotRoomUpdateFull(RoomID roomID, std::shared_ptr<std::vector<ChatMessage>> messages, QPrivateSignal)
{
    std::vector<ChatMessage>* pMessages = messages.get();
    for(int i = 0; i < pMessages->size(); i++)
    {
        const ChatMessage& message = pMessages->at(i);
        addMessageToRoom(roomID, message.m_senderId, message.m_message);
    }

    emit eventRoomUpdateFull(roomID, messages);
}

void QModelManager::netSlotRoomUpdateUserJoined(RoomID roomID, PeerID newPeerID, std::string newName, QPrivateSignal)
{
    addKnownUser(newPeerID, newName);
    addUserToRoom(newPeerID, roomID);

    emit eventRoomUpdateUserJoined(roomID, newPeerID, newName);
}

void QModelManager::netSlotRoomUpdateUserLeft(RoomID roomID, PeerID peerID, QPrivateSignal)
{
    emit eventRoomUpdateUserAboutToLeave(roomID, peerID);

    removeUserFromRoom(peerID, roomID);
    if(peerID == getLocalPeerId())
    {
        removeJoinedChatRoom(roomID);
    }
}
