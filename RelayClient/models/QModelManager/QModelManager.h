#pragma once

#include <QObject>
#include <QMutexLocker>

#include "RelayIMClient.h"
#include "models/QChatRoomsModel/QChatRoomsModel.h"
#include "models/QChatRoomMessagesModel/QChatRoomMessagesModel.h"
#include "Logging.h"

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

    RelayIMClient* GetClient();

signals:

    // NOTE(Salads): UI Related Events
    void Event_RegisterResponse(PacketResponseReason result, PeerID peerID, std::string username);
    void Event_JoinRoomResponse(PacketResponseReason reason, RoomID newRoomID, std::string newChatRoomName);
    void Event_CreateRoomResponse(PacketResponseReason reason, RoomID newRoomID, std::string newChatRoomName);
    void Event_ListChatRoomsResponse(std::shared_ptr<std::vector<ChatRoomInfo>>);

    // NOTE(Salads): Data related events (models)
    void Event_RoomUpdate_Message(RoomID roomID, PeerID peerID, std::string message);
    void Event_RoomUpdate_FULL(RoomID roomID, std::shared_ptr<std::vector<ChatMessage>> messages);
    void Event_RoomUpdate_UserJoined(RoomID roomID, PeerID newPeerID, std::string newName);
    void Event_RoomUpdate_UserLeft(RoomID roomID, PeerID peerID);

public slots:

    // NOTE(Salads): All of these slots exist to bring the incoming data to the main thread, since QModels NEED to operate on the main thread.
    void Slot_RegisterResponse(PacketResponseReason result, PeerID peerID, std::string username);
    void Slot_JoinRoomResponse(PacketResponseReason reason, RoomID newRoomID, std::string newChatRoomName);
    void Slot_CreateRoomResponse(PacketResponseReason reason, RoomID newRoomID, std::string newChatRoomName);
    
    void Slot_RoomUpdate_Message(RoomID roomID, PeerID peerID, std::string message);
    void Slot_RoomUpdate_FULL(RoomID roomID, std::shared_ptr<std::vector<ChatMessage>> messages);
    void Slot_RoomUpdate_UserJoined(RoomID roomID, PeerID newPeerID, std::string newName);
    void Slot_RoomUpdate_UserLeft(RoomID roomID, PeerID peerID);

private:
    void InitializeClientCallbacks();

private:
    RelayIMClient m_client;

    bool m_callbacksInitialized = false;

    PeerID m_localPeerID = INVALID_PEER_ID; 
    RoomID m_localRoomID = INVALID_ROOM_ID; // Current room

    QMap<PeerID, std::string> m_knownUsers;
    QMap<PeerID, QSet<RoomID>> m_userRooms;

    std::shared_ptr<QChatRoomsModel> m_chatRoomsModel; // Joined Chat Rooms ListView data
    QMap<RoomID, std::shared_ptr<QChatRoomMessagesModel>> m_chatRoomMessagesModels;
};