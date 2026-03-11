#pragma once

#include <QAbstractListModel>

#include "ChatRoomInfo.h"

class QJoinChatRoomsModel  : public QAbstractListModel
{
    Q_OBJECT

public:

    enum Role
    {
        RoomIDRole = Qt::UserRole,
        RoomnameRole
    };

    QJoinChatRoomsModel(QObject *parent = nullptr);
    ~QJoinChatRoomsModel();

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void ReplaceAll(std::shared_ptr<std::vector<ChatRoomInfo>> newData);

private:
    QVector<ChatRoomInfo> m_chatRooms;
};

