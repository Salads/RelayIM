#include "QChatRoomsModel.h"

QChatRoomsModel::QChatRoomsModel(QObject *parent)
    : QAbstractListModel(parent)
{}

QChatRoomsModel::~QChatRoomsModel()
{}

int QChatRoomsModel::rowCount(const QModelIndex& parent) const
{
    return m_chatRooms.size();
}

QVariant QChatRoomsModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()){ return QVariant(); }

    int row = index.row();
    const ChatRoomInfo* info = &m_chatRooms[row];

    switch (role)
    {
        case Role::RoomIDRole:
        {
            return info->m_roomID;
        }
        case Role::RoomnameRole:
        {
            return QString::fromStdString(info->m_roomname);
        }
        default:
        {
            return QVariant();
        }
    }
}

QHash<int, QByteArray> QChatRoomsModel::roleNames() const
{
    QHash<int, QByteArray> names = QAbstractListModel::roleNames();
    names[RoomIDRole] = "roomid";
    names[RoomnameRole] = "roomname";
    return names;
}

void QChatRoomsModel::AddChatRoom(RoomID roomID, QString roomname)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_chatRooms.emplaceBack(roomID, roomname);
    endInsertRows();
}

void QChatRoomsModel::RemoveChatRoom(RoomID roomID)
{
    // Find the index of the room we want to remove.
    qsizetype idx = -1;
    for (int i = 0; i < m_chatRooms.size(); i++)
    {
        ChatRoomInfo& room = m_chatRooms[i];
        if (room.m_roomID == roomID)
        {
            idx = i;
            break;
        }
    }

    if (idx == -1)
    {
        return;
    }

    beginRemoveRows(QModelIndex(), idx, idx);
    m_chatRooms.removeAt(idx);
    endRemoveRows();
}