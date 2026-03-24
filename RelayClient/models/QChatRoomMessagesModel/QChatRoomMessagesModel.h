#pragma once

#include "ChatMessage.h"
#include "ChatRoomInfo.h"
#include "RoomID.h"
#include "PeerID.h"

#include <QObject>
#include <QAbstractListModel>
#include <QVector>
#include <QList>
#include <QMutex>
#include <QHash>

class QChatRoomMessagesModel  : public QAbstractListModel
{
    Q_OBJECT

public:

    enum Role
    {
        PeerIDRole = Qt::UserRole,
        MessageRole
    };

    QChatRoomMessagesModel(QObject* parent = nullptr);
    ~QChatRoomMessagesModel();

    void Initialize(QHash<PeerID, std::string>* knownUsers, std::shared_ptr<ChatRoomInfo> info);
    void Initialize(QHash<PeerID, std::string>* knownUsers, RoomID roomID, std::string chatRoomName);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void AddMessage(PeerID peerID, const std::string& message);
    RoomID GetRoomID();
    std::string GetRoomname();

private:

    ChatRoomInfo m_info;

    QList<ChatMessage> m_messages;
    QHash<PeerID, std::string> *m_knownUsers;
};

