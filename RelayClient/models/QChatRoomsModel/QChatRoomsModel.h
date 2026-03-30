#ifndef QCHATROOMSMODEL_H
#define QCHATROOMSMODEL_H

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

    void addJoinedChatRoom(RoomID roomID, QString roomname);
    void removeJoinedChatRoom(RoomID roomID);

    qsizetype findRoom(RoomID roomID);
    bool roomExists(const std::string& roomname);
    void replaceAll(std::shared_ptr<std::vector<ChatRoomInfo>> newData);

    std::shared_ptr<ChatRoomInfo> getChatRoomInfo(RoomID roomID);

private:
    QVector<ChatRoomInfo> m_chatRooms;
};

#endif // QCHATROOMSMODEL_H
