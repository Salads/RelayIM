#include "QChatRoomsModel.h"

#include <QMutexLocker>
#include <QMutex>

QChatRoomsModel::QChatRoomsModel(QObject *parent)
    : QAbstractListModel(parent)
{}

QChatRoomsModel::~QChatRoomsModel()
{}

int QChatRoomsModel::rowCount(const QModelIndex& parent) const
{
    QMutexLocker lock(&m_mutex);
    return m_chatRooms.size();
}

QVariant QChatRoomsModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()){ return QVariant(); }

    QMutexLocker lock(&m_mutex);

    int row = index.row();
    std::shared_ptr<ChatRoomInfo> info = m_chatRooms[row];

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
        case Qt::DisplayRole:
        {
            return QString::fromStdString(info->m_roomname + std::string("(id ") + std::to_string(info->m_roomID) + std::string(")"));
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

qsizetype QChatRoomsModel::FindRoom(RoomID roomID)
{
    QMutexLocker lock(&m_mutex);

    for (qsizetype i = 0; i < m_chatRooms.size(); i++)
    {
        if (m_chatRooms[i]->m_roomID == roomID)
        {
            return i;
            break;
        }
    }

    return -1;
}

void QChatRoomsModel::AddChatRoom(RoomID roomID, QString roomname)
{
    QMutexLocker lock(&m_mutex);

    if (FindRoom(roomID) != -1) { return; }

    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_chatRooms.emplaceBack(std::make_shared<ChatRoomInfo>(roomID, roomname.toStdString()));
    endInsertRows();
}

void QChatRoomsModel::RemoveChatRoom(RoomID roomID)
{
    QMutexLocker lock(&m_mutex);

    // Find the index of the room we want to remove.
    qsizetype idx = -1;
    for (int i = 0; i < m_chatRooms.size(); i++)
    {
        std::shared_ptr<ChatRoomInfo> room = m_chatRooms[i];
        if (room->m_roomID == roomID)
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

std::shared_ptr<ChatRoomInfo> QChatRoomsModel::GetChatRoomInfo(RoomID roomID)
{
    qsizetype idx = FindRoom(roomID);
    if (idx != -1)
    {
        return m_chatRooms[idx];
    }

    return nullptr;
}
