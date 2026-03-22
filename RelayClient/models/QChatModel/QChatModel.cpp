#include "QChatModel.h"
#include "widgets/QChatView/QChatView.h"

QChatModel::QChatModel(QChatView* view, QModelManager* manager, QWidget* parent)
    : m_chatView(view), m_manager(manager), QWidget(parent)
{
    m_messagePositionsStartY.reserve(UINT16_MAX);

    m_vBar = view->verticalScrollBar();

    connect(m_manager, &QModelManager::Event_RoomUpdate_Message, this, &QChatModel::Slot_RoomUpdate_Message);
    connect(m_manager, &QModelManager::Event_RoomUpdate_FULL, this, &QChatModel::Slot_RoomUpdate_FULL);
    connect(m_vBar, &QScrollBar::rangeChanged, this, &QChatModel::Slot_ScrollRangeChanged);
    connect(m_vBar, &QScrollBar::valueChanged, this, &QChatModel::Slot_ScrollValueChanged);
}

void QChatModel::Slot_ScrollRangeChanged(int min, int max)
{
    RenderObjects();
}

void QChatModel::Slot_ScrollValueChanged(int value)
{
    RenderObjects();
}

void QChatModel::RenderObjects()
{
    if(m_roomID == INVALID_ROOM_ID)
    {
        return;
    }

    float min = m_vBar->minimum();
    float max = m_vBar->maximum();
    float val = m_vBar->value();

    uint64_t pixStartY = 0;
    uint64_t pixEndY = m_totalHeight;

    // Scrollbar has content larger than viewport
    if(max > 0)
    {
        pixStartY = val;
        pixEndY = pixStartY + m_vBar->pageStep();
    }

    std::vector<QMessagePosition> positions = GetMessagesForRender(pixStartY, pixEndY);

    // Make sure we have the exact amount of QMessage objects
    qsizetype nObjectsNeeded = positions.size();
    qsizetype nObjects = m_messageObjects.size();

    for(int i = nObjects; i < nObjectsNeeded; i++)
    {
        QMessage* newMessageObj = new QMessage(this);
        m_messageObjects.push_back(newMessageObj);
    }

    nObjects = m_messageObjects.size();
    for(int i = nObjects; i > nObjectsNeeded; i--)
    {
        QMessage* messageObj = m_messageObjects.takeLast();

        QPalette pal = QPalette();
        pal.setColor(QPalette::Window, Qt::red);
        messageObj->setAutoFillBackground(true);
        messageObj->setPalette(pal);

        delete messageObj;
    }

    // Now we have the exact amount needed.
    Q_ASSERT(positions.size() == m_messageObjects.size());

    for(int i = 0; i < positions.size(); i++)
    {
        QMessagePosition* pos = &positions[i];
        ChatMessage* message = &(*m_messages)[pos->m_index];
        QMessage* obj = m_messageObjects[i];

        std::string username = m_manager->GetUsernameByPeerID(message->m_senderID);

        obj->SetContents(username, message->m_message, m_chatView->GetViewportWidth());
        obj->move(0, pos->m_startY);
        obj->show();
    }
}

void QChatModel::SetRoom(RoomID roomID)
{
    ClearMessagePositions();
    ClearRenderObjects();

    m_roomID = roomID;

    if(roomID != INVALID_ROOM_ID)
    {
        m_messages = m_manager->GetMessagesForRoom(roomID);
    }
    else
    {
        m_messages = nullptr;
    }
    
    PrecalculateMessagePositions();
    RenderObjects();
}

void QChatModel::Slot_RoomUpdate_Message(RoomID roomID, PeerID peerID, std::string message)
{
    PrecalculateMessagePositions();
    m_vBar->setValue(m_vBar->maximum());
    RenderObjects();
}

void QChatModel::Slot_RoomUpdate_FULL(RoomID roomID, std::shared_ptr<std::vector<ChatMessage>> messages)
{
    Refresh();
}

uint32_t QChatModel::GetMessageHeight(const std::string& username, const std::string& message)
{
    QMessageTextConstraints constraints =  QMessage::GetTextConstraints(username, message, m_chatView->GetViewportWidth());
    return constraints.m_messageSize.height();
}

void QChatModel::PrecalculateMessagePositions()
{
    if(m_roomID == INVALID_ROOM_ID)
    {
        return;
    }

    size_t startIdx = m_messagePositionsStartY.size();
    int spaceBetweenMessages = QMessage::Margin + QMessage::Spacing;
    uint64_t pixelPosY = c_chatWindowMargin;

    if(startIdx > 0)
    {
        pixelPosY = m_messagePositionsEndY[startIdx - 1] + spaceBetweenMessages;
    }

    for(int i = startIdx; i < m_messages->size(); i++)
    {
        const ChatMessage* message = &m_messages->at(i);

        uint64_t totalObjectHeight = GetMessageHeight(m_manager->GetUsernameByPeerID(message->m_senderID), message->m_message);
        uint64_t nextYPos = pixelPosY + totalObjectHeight + spaceBetweenMessages;

        m_messagePositionsStartY.emplace_back(pixelPosY);
        m_messagePositionsEndY.emplace_back(pixelPosY + totalObjectHeight);

        pixelPosY = nextYPos;
        m_totalHeight = nextYPos + c_chatWindowMargin;
    }

    setFixedHeight(m_totalHeight);
    setMinimumSize(m_chatView->GetViewportWidth(), m_totalHeight);
}

void QChatModel::ClearMessagePositions()
{
    ClearRenderObjects();
    m_messagePositionsStartY.clear();
    m_messagePositionsEndY.clear();
    m_totalHeight = 0;
}

void QChatModel::ClearRenderObjects()
{
    for(int i = 0; i < m_messageObjects.size(); i++)
    {
        delete m_messageObjects[i];
    }

    m_messageObjects.clear();
}

void QChatModel::Refresh()
{
    ClearMessagePositions();
    PrecalculateMessagePositions();
    m_vBar->setValue(m_vBar->maximum());
    RenderObjects();
}

std::vector<QMessagePosition> QChatModel::GetMessagesForRender(uint64_t viewportStartY, uint64_t viewportEndY)
{
    std::vector<QMessagePosition> result;

    // Binary Search, using viewportStartY
    qsizetype leftIdx = 0;
    qsizetype rightIdx = m_messagePositionsStartY.size() - 1;
    qsizetype midIdx = 0;

    if(m_messagePositionsStartY.empty())
    {
        return result;
    }

    while(leftIdx <= rightIdx)
    {
        midIdx = leftIdx + ((rightIdx - leftIdx) / 2);
        uint64_t midStartY = m_messagePositionsStartY[midIdx];
        uint64_t midEndY = m_messagePositionsEndY[midIdx];

        if(viewportStartY >= midStartY && viewportStartY <= midEndY)
        {
            break;
        }
        else if(viewportStartY < midStartY)
        {
            if(midIdx == 0)
            {
                break;
            }

            rightIdx = midIdx - 1;
        }
        else if(viewportStartY > midStartY)
        {
            if(midIdx == m_messagePositionsStartY.size() - 1)
            {
                break;
            }

            leftIdx = midIdx + 1;
        }
    }

    // If the scrollbar value is in between messages, we either got the correct one, or the message invisible above the top.
    // So this is fine.

    uint64_t posStartY = m_messagePositionsStartY[midIdx];
    while(posStartY <= viewportEndY && midIdx < m_messagePositionsStartY.size())
    {
        QMessagePosition newPos(posStartY, m_messagePositionsEndY[midIdx], midIdx);
        result.push_back(newPos);

        if(++midIdx < m_messagePositionsStartY.size())
        {
            posStartY = m_messagePositionsStartY[midIdx];
        }
    }

    return result;
}

QMessagePosition::QMessagePosition(uint64_t startY, uint64_t endY, qsizetype index)
{
    m_startY = startY;
    m_endY = endY;
    m_index = index;
}
