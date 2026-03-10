#pragma once

#include <QObject>
#include <QAbstractListModel>
#include <QVector>
#include <QMutexLocker>
#include <QMutex>

#include "ChatRoomInfo.h"

class QChatRoomsModel  : public QAbstractListModel
{
    Q_OBJECT

public:

    enum Role
    {
        RoomIDRole = Qt::UserRole,
        RoomnameRole
    };

    QChatRoomsModel(QObject *parent = nullptr);
    ~QChatRoomsModel();

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void AddChatRoom(RoomID roomID, QString roomname);
    void RemoveChatRoom(RoomID roomID);

    qsizetype FindRoom(RoomID roomID);

    std::shared_ptr<ChatRoomInfo> GetChatRoomInfo(RoomID roomID);

private:
    QVector<std::shared_ptr<ChatRoomInfo>> m_chatRooms;
    mutable QMutex m_mutex;
};
