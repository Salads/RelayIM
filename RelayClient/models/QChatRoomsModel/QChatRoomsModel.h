#pragma once

#include <QObject>
#include <QAbstractListModel>
#include <qvector.h>

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

    QChatRoomsModel(QObject *parent);
    ~QChatRoomsModel();

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void AddChatRoom(RoomID roomID, QString roomname);
    void RemoveChatRoom(RoomID roomID);

private:
    QVector<ChatRoomInfo> m_chatRooms;
};
