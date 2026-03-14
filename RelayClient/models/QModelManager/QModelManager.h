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
    ~QModelManager();

    bool Initialize();
    bool Connect();
    void Shutdown();

    PeerID GetLocalPeerID();
    void SetLocalPeerID(PeerID peerID);

    void AddKnownUser(PeerID peerID, const std::string& username);
    void AddUserToRoom(PeerID peerID, RoomID roomID);
    void AddMessageToRoom(RoomID roomID, PeerID peerID, const std::string& message);

    void RemoveUserFromRoom(PeerID peerID, RoomID roomID);
    void RemoveJoinedChatRoom(RoomID roomID);

    QModelIndex GetChatRoomIdx(RoomID roomID);

    bool HasJoinedRoom(RoomID roomID);

    /// <summary>
    /// Adds a chat room to the QChatRoomsModel, and creates a new QChatRoomMessagesModel for the RoomID given.
    /// If a QChatRoomMessagesModel exists, deletes it first before adding a new one.
    /// </summary>
    /// <param name="roomID">RoomID of new chat room</param>
    /// <param name="roomname">Display roomname of new chat room</param>
    void AddJoinedChatRoom(RoomID roomID, const std::string& roomname);

    QChatRoomsModel* GetModelForRooms();
    QChatRoomMessagesModel* GetModelForRoom(RoomID roomID);

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

private:
    void InitializeClientCallbacks();
    void DeleteRoomMessagesModel(RoomID roomID);

private:
    RelayIMClient m_client;

    bool m_callbacksInitialized = false;

    PeerID m_localPeerID = INVALID_PEER_ID; 

    QMap<PeerID, std::string> m_knownUsers;
    QMap<PeerID, QSet<RoomID>> m_userRooms;

    QChatRoomsModel m_joinedChatRoomsModel;
    QMap<RoomID, QChatRoomMessagesModel*> m_chatRoomMessagesModels;
};