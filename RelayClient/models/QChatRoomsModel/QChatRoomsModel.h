#pragma once

#include <QObject>
#include <QAbstractListModel>
#include <QVector>
#include <QMutexLocker>
#include <QMutex>

#include "ChatRoomInfo.h"

// Data model to hold a list of chat rooms, but not the data inside (messages)
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

    void AddJoinedChatRoom(RoomID roomID, QString roomname);
    void RemoveJoinedChatRoom(RoomID roomID);

    qsizetype FindRoom(RoomID roomID);
    bool RoomExists(const std::string& roomname);
    void ReplaceAll(std::shared_ptr<std::vector<ChatRoomInfo>> newData);

    std::shared_ptr<ChatRoomInfo> GetChatRoomInfo(RoomID roomID);

private:
    QVector<ChatRoomInfo> m_chatRooms;
};
