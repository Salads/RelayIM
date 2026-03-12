#pragma once

#include <QMainWindow>
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
    RelayClient(QWidget *parent = nullptr);
    ~RelayClient();

    void TryConnect();

private:
    Ui::RelayClientClass ui;

    QModelManager m_manager;

    QListView* m_roomsListView;
    QChatWidget* m_chatWidget;

    QPushButton* m_createOrJoinChatRoomButton;
    QPushButton* m_leaveChatRoomButton;

    QConnectionStatus* m_connectionStatus;

    QRegisterDialog* m_registerDialog;
};

