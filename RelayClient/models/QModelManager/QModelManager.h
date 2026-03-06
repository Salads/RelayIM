#pragma once

#include <QObject>

#include "RelayIMClient.h"
#include "models/QChatRoomsModel/QChatRoomsModel.h"

class QModelManager : public QObject
{
    Q_OBJECT  // Required for signals/slots

public:
    QModelManager(QObject* parent = nullptr) : QObject(parent) {}

    bool Initialize();
    bool Connect();
    void Shutdown();

signals:
    void Connected();

public slots:
    void OnConnect();

private:
    void InitializeClientCallbacks();

private:
    RelayIMClient m_client;

    std::shared_ptr<QChatRoomsModel> m_chatRoomsModel;
};