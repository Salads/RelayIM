#ifndef QCHATROOMMESSAGESMODEL_H
#define QCHATROOMMESSAGESMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QVector>
#include <QList>
#include <QMutex>
#include <QHash>

#include "ChatMessage.h"
#include "ChatRoomInfo.h"
#include "RoomID.h"
#include "PeerID.h"

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

    void initialize(QHash<PeerID, std::string>* knownUsers, std::shared_ptr<ChatRoomInfo> info);
    void initialize(QHash<PeerID, std::string>* knownUsers, RoomID roomID, std::string chatRoomName);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void addMessage(PeerID peerID, const std::string& message);
    RoomID getRoomId();
    std::string getRoomname();

private:

    ChatRoomInfo m_info;

    QList<ChatMessage> m_messages;
    QHash<PeerID, std::string> *m_knownUsers;
};

#endif // QCHATROOMMESSAGESMODEL_H

