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

void QModelManager::InitializeClientCallbacks()
{
    m_client.OnLocalRegistered = [this](PeerID peerID, std::string username)
    {
        QMutexLocker lock(&m_mutex);
        m_localPeerID = peerID;
        m_knownUsers[peerID] = username;
    };

    m_client.OnListChatRoomsReceived = [this](std::shared_ptr<std::vector<ChatRoomInfo>> chatRooms)
    {
        emit ChatRoomsListReceived(chatRooms);
    };

    m_client.OnNewClientDiscovered = [this](PeerID newPeerID, RoomID roomID, std::string newName)
    {
        QMutexLocker lock(&m_mutex);
        m_knownUsers[newPeerID] = newName;
        m_userRooms[newPeerID].insert(roomID);
    };

    m_client.OnLocalChatRoomJoined = [this](RoomID newRoomID, std::string newChatRoomName)
    {
        {
            QMutexLocker lock(&m_mutex);
            m_userRooms[m_localPeerID].insert(newRoomID);
            m_localRoomID = newRoomID;
        }

        m_chatRoomsModel->AddChatRoom(newRoomID, QString::fromStdString(newChatRoomName));
    };

    m_client.OnClientLeftChatRoom = [this](PeerID peerID, RoomID roomID)
    {
        QMutexLocker lock(&m_mutex);
        m_userRooms[peerID].remove(roomID);
    };

    m_client.OnMessageReceived = [this](PeerID peerID, RoomID roomID, std::string message)
    {
        QMutexLocker lock(&m_mutex);

        if (!m_chatRoomMessagesModels.contains(roomID))
        {
            m_chatRoomMessagesModels[roomID] = std::make_shared<QChatRoomMessagesModel>();
            m_chatRoomMessagesModels[roomID]->Initialize(&m_knownUsers, m_chatRoomsModel->GetChatRoomInfo(roomID));
        }
        
        m_chatRoomMessagesModels[roomID]->AddMessage(peerID, QString::fromStdString(message));
    };

    m_client.OnRoomFullUpdate = [this](RoomID roomID, std::unique_ptr<std::vector<ChatMessage>> messages)
    {
        QMutexLocker lock(&m_mutex);

        if (!m_chatRoomMessagesModels.contains(roomID))
        {
            m_chatRoomMessagesModels[roomID] = std::make_shared<QChatRoomMessagesModel>();
            m_chatRoomMessagesModels[roomID]->Initialize(&m_knownUsers, m_chatRoomsModel->GetChatRoomInfo(roomID));
        }

        std::vector<ChatMessage>* pMessages = messages.get();
        for (int i = 0; i < pMessages->size(); i++)
        {
            m_chatRoomMessagesModels[roomID]->AddMessage(pMessages->at(i).m_senderID, QString::fromStdString(pMessages->at(i).m_message));
        }
    };

    m_client.OnResponseFailed = [this](PacketResponseReason reason)
    {
        // TODO(Salads): What do here?
    };
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
    QMutexLocker lock(&m_mutex);
    if (!m_chatRoomMessagesModels.contains(roomID))
    {
        return nullptr;
    }

    return m_chatRoomMessagesModels[roomID];
}

std::shared_ptr<QChatRoomsModel> QModelManager::GetModelForRooms()
{
    QMutexLocker lock(&m_mutex);
    return m_chatRoomsModel;
}
