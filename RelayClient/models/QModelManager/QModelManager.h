#pragma once

#include <QObject>
#include <QMutexLocker>

#include "RelayIMClient.h"
#include "models/QChatRoomsModel/QChatRoomsModel.h"
#include "models/QChatRoomMessagesModel/QChatRoomMessagesModel.h"

class QModelManager : public QObject
{
    Q_OBJECT  // Required for signals/slots

public:
    QModelManager(QObject* parent = nullptr);

    bool Initialize();
    bool Connect();
    void Shutdown();

    PeerID GetLocalPeerID();
    RoomID GetLocalRoomID();

    std::shared_ptr<QChatRoomsModel> GetModelForRooms();
    std::shared_ptr<QChatRoomMessagesModel> GetModelForRoom(RoomID roomID);

signals:
    void ChatRoomsListReceived(std::shared_ptr<std::vector<ChatRoomInfo>>);

public slots:

private:
    void InitializeClientCallbacks();

private:
    RelayIMClient m_client;

    QMutex m_mutex;

    PeerID m_localPeerID = INVALID_PEER_ID; 
    RoomID m_localRoomID = INVALID_ROOM_ID; // Current room

    QMap<PeerID, std::string> m_knownUsers;
    QMap<PeerID, QSet<RoomID>> m_userRooms;

    std::shared_ptr<QChatRoomsModel> m_chatRoomsModel;
    QMap<RoomID, std::shared_ptr<QChatRoomMessagesModel>> m_chatRoomMessagesModels;
};