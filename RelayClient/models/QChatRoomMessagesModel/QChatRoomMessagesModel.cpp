#include "QChatRoomMessagesModel.h"

QChatRoomMessagesModel::QChatRoomMessagesModel(QObject *parent)
    : QAbstractListModel(parent)
{}

QChatRoomMessagesModel::~QChatRoomMessagesModel()
{}

int QChatRoomMessagesModel::rowCount(const QModelIndex& parent) const
{
    return m_messages.size();
}

QVariant QChatRoomMessagesModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) { return QVariant(); }

    if (role == PeerIDRole)
    {
        uint32_t peerID = static_cast<uint32_t>(m_messages[index.row()].m_senderID);
        return peerID;
    }
    else if (role == MessageRole)
    {
        return QString::fromStdString(m_messages[index.row()].m_message);
    }
    else if (role == Qt::DisplayRole)
    {
        if (m_knownUsers->contains(m_messages[index.row()].m_senderID))
        {
            return QString::fromStdString((*m_knownUsers)[m_messages[index.row()].m_senderID] + std::string(": ") + m_messages[index.row()].m_message);
        }
        else
        {
            uint32_t peerID = static_cast<uint32_t>(m_messages[index.row()].m_senderID);
            return QString::fromStdString(std::to_string(peerID) + std::string(": ") + m_messages[index.row()].m_message);
        }
    }

    return QVariant();
}

void QChatRoomMessagesModel::Initialize(QHash<PeerID, std::string>* knownUsers, std::shared_ptr<ChatRoomInfo> info)
{
    m_knownUsers = knownUsers;
    m_info = *info.get();
}

void QChatRoomMessagesModel::Initialize(QHash<PeerID, std::string>* knownUsers, RoomID roomID, std::string chatRoomName)
{
    m_knownUsers = knownUsers;
    m_info.m_roomID = roomID;
    m_info.m_roomname = chatRoomName;
}

QHash<int, QByteArray> QChatRoomMessagesModel::roleNames() const
{
    QHash<int, QByteArray> names = QAbstractListModel::roleNames();
    names[PeerIDRole] = "peerid";
    names[MessageRole] = "message";
    return names;
}

void QChatRoomMessagesModel::AddMessage(PeerID peerID, const std::string& message)
{
    beginInsertRows(QModelIndex(), m_messages.size(), m_messages.size());
    m_messages.emplaceBack(peerID, message);
    endInsertRows();
}

RoomID QChatRoomMessagesModel::GetRoomID()
{
    return m_info.m_roomID;
}

std::string QChatRoomMessagesModel::GetRoomname()
{
    return m_info.m_roomname;
}
