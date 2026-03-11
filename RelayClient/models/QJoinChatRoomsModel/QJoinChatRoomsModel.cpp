#include "QJoinChatRoomsModel.h"

QJoinChatRoomsModel::QJoinChatRoomsModel(QObject *parent)
    : QAbstractListModel(parent)
{}

QJoinChatRoomsModel::~QJoinChatRoomsModel()
{}

int QJoinChatRoomsModel::rowCount(const QModelIndex& parent) const
{
    return m_chatRooms.size();
}

QVariant QJoinChatRoomsModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid()) { return QVariant(); }

    int row = index.row();
    ChatRoomInfo info = m_chatRooms[row];

    switch(role)
    {
        case Role::RoomIDRole:
        {
            return info.m_roomID;
        }
        case Role::RoomnameRole:
        {
            return QString::fromStdString(info.m_roomname);
        }
        case Qt::DisplayRole:
        {
            return QString::fromStdString(info.m_roomname + std::string("(id ") + std::to_string(info.m_roomID) + std::string(")"));
        }
        default:
        {
            return QVariant();
        }
    }
}

QHash<int, QByteArray> QJoinChatRoomsModel::roleNames() const
{
    QHash<int, QByteArray> names = QAbstractListModel::roleNames();
    names[RoomIDRole] = "roomid";
    names[RoomnameRole] = "roomname";
    return names;
}

void QJoinChatRoomsModel::ReplaceAll(std::shared_ptr<std::vector<ChatRoomInfo>> newData)
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
