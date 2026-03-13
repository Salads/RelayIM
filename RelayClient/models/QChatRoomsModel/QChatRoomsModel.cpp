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
        case Qt::DisplayRole:
        {
            return QString::fromStdString(info->m_roomname + std::string(" (id ") + std::to_string(info->m_roomID) + std::string(")"));
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
    for (qsizetype i = 0; i < m_chatRooms.size(); i++)
    {
        if (m_chatRooms[i].m_roomID == roomID)
        {
            return i;
        }
    }

    return -1;
}

void QChatRoomsModel::ReplaceAll(std::shared_ptr<std::vector<ChatRoomInfo>> newData)
{
    std::vector<ChatRoomInfo>* vec = newData.get();

    if(!m_chatRooms.isEmpty())
    {
        beginRemoveRows(QModelIndex(), 0, m_chatRooms.size() - 1);
        m_chatRooms.clear();
        endRemoveRows();
    }

    if(!vec->empty())
    {
        beginInsertRows(QModelIndex(), 0, vec->size() - 1);

        for(int i = 0; i < vec->size(); i++)
        {
            m_chatRooms.emplaceBack((*vec)[i]);
        }

        endInsertRows();
    }
}

void QChatRoomsModel::AddChatRoom(RoomID roomID, QString roomname)
{
    if(FindRoom(roomID) != -1) { return; }

    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_chatRooms.emplaceBack(roomID, roomname.toStdString());
    endInsertRows();
}

void QChatRoomsModel::RemoveChatRoom(RoomID roomID)
{
    // Find the index of the room we want to remove.
    qsizetype idx = FindRoom(roomID);
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
        return std::make_shared<ChatRoomInfo>(m_chatRooms[idx]);
    }

    return nullptr;
}
