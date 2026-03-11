#pragma once

#include <QMainWindow>
#include <QListView>
#include <QPushButton>
#include "ui_RelayClient.h"

#include "widgets/QChatWidget/QChatWidget.h"
#include "models/QModelManager/QModelManager.h"
#include "widgets/QConnectionStatus/QConnectionStatus.h"

class RelayClient : public QMainWindow
{
    Q_OBJECT

public:
    RelayClient(QWidget *parent = nullptr);
    ~RelayClient();

private:
    Ui::RelayClientClass ui;

    QModelManager m_manager;

    QListView* m_roomsListView;
    QChatWidget* m_chatWidget;

    QPushButton* m_createOrJoinChatRoomButton;

    QConnectionStatus* m_connectionStatus;
};

