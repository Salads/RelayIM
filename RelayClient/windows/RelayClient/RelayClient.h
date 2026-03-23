#pragma once

#include <QMainWindow>
#include <QResizeEvent>
#include <QListView>
#include <QPushButton>
#include <QBoxLayout>
#include <QErrorMessage>
#include "ui_RelayClient.h"

#include "models/QModelManager/QModelManager.h"
#include "models/QChatRoomsModel/QChatRoomsModel.h"
#include "widgets/QChatWidget/QChatWidget.h"
#include "widgets/QConnectionStatus/QConnectionStatus.h"
#include "windows/QRegisterDialog/QRegisterDialog.h"
#include "windows/ChatRoomsDialog/QChatRoomsDialog.h"

class RelayClient : public QMainWindow
{
    Q_OBJECT

public:
    RelayClient(QModelManager* manager, QWidget *parent = nullptr);
    ~RelayClient();

    /// <summary>
    /// Purely a socket-level connect, without registering a username
    /// </summary>
    /// <returns>true if Successful</returns>
    bool TryConnect();

    void SetStatusUI(QConnectionStatus::Status status);

    void UpdateWindowTitle();

private:
    void InitializeSignalConnections();
    void SetCurrentRoom(RoomID roomID);

public slots:
    void Slot_JoinRoomResponse(PacketResponseReason reason, RoomID newRoomID, std::string newChatRoomName);
    void Slot_CreateRoomResponse(PacketResponseReason reason, RoomID newRoomID, std::string newChatRoomName);
    void Slot_RoomUpdate_UserLeft(RoomID roomID, PeerID peerID);

private:
    Ui::RelayClientClass ui;

    const QSize c_minSize = QSize(650, 393);

    QModelManager* m_manager;

    QListView* m_roomsListView;
    QChatWidget* m_chatWidget;

    QPushButton* m_createOrJoinChatRoomButton;
    QPushButton* m_leaveChatRoomButton;

    QConnectionStatus* m_connectionStatus;
};

