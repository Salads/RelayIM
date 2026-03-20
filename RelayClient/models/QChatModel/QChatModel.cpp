#include "QChatModel.h"
#include "widgets/QChatView/QChatView.h"

QChatModel::QChatModel(QChatView* view, QModelManager* manager, QWidget* parent)
    : m_chatView(view), m_manager(manager), QWidget(parent)
{
    m_messagePositionsStartY.reserve(UINT16_MAX);

    m_vBar = view->verticalScrollBar();

    connect(m_manager, &QModelManager::Event_RoomUpdate_Message, this, &QChatModel::NetSlot_RoomUpdate_Message);
    connect(m_manager, &QModelManager::Event_RoomUpdate_FULL, this, &QChatModel::NetSlot_RoomUpdate_FULL);

    connect(m_vBar, &QScrollBar::valueChanged, this, &QChatModel::RenderObjects);
}

void QChatModel::RenderObjects()
{
    float min = m_vBar->minimum();
    float max = m_vBar->maximum();
    float val = m_vBar->value();
    float percentVal = (val - min) / (max - min);
    uint64_t pixStartY = m_totalHeight * percentVal;
    uint64_t pixEndY = pixStartY + m_vBar->pageStep();
    std::vector<QMessagePosition> positions = GetMessagesForRender(pixStartY, pixEndY);

    // Make sure we have the exact amount of QMessage objects
    qsizetype nObjectsNeeded = positions.size();
    qsizetype nObjects = m_messageObjects.size();

    for(int i = nObjects; i < nObjectsNeeded; i++)
    {
        QMessage* newMessageObj = new QMessage(this);
        m_messageObjects.push_back(newMessageObj);
    }

    for(int i = 0; i < (nObjectsNeeded - nObjects); i++)
    {
        QMessage* messageObj = m_messageObjects.back();
        m_messageObjects.pop_back();
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

        obj->SetContents(username, message->m_message);
        obj->move(0, pos->m_startY);
        obj->show();
    }
}

void QChatModel::SetRoom(RoomID roomID)
{
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
}

void QChatModel::NetSlot_RoomUpdate_Message(RoomID roomID, PeerID peerID, std::string message)
{
    PrecalculateMessagePositions();
    m_vBar->setValue(m_vBar->maximum());
}

void QChatModel::NetSlot_RoomUpdate_FULL(RoomID roomID, std::shared_ptr<std::vector<ChatMessage>> messages)
{
    ClearMessagePositions();
    PrecalculateMessagePositions();
    m_vBar->setValue(m_vBar->maximum());
}

uint32_t QChatModel::GetMessageHeight(const std::string& message)
{
    QFontMetrics fm(QMessage::Font);
    int lineHeight = fm.height();
    int message1DWidth = fm.horizontalAdvance(QString::fromStdString(message));
    int nLines = (message1DWidth / QMessage::MessageWidth) + 1;
    int totalMessageHeight = nLines * lineHeight;

    return totalMessageHeight;
}

void QChatModel::PrecalculateMessagePositions()
{
    if(m_roomID == INVALID_ROOM_ID)
    {
        return;
    }

    size_t startIdx = m_messagePositionsStartY.size();
    uint64_t pixelPosY = QMessage::Margin;

    if(startIdx > 0)
    {
        pixelPosY = m_messagePositionsEndY[startIdx - 1] + QMessage::Margin;
    }

    for(int i = startIdx; i < m_messages->size(); i++)
    {
        uint64_t newMessagePixelY = 0;
        const ChatMessage* message = &m_messages->at(i);

        uint64_t totalMessageHeight = GetMessageHeight(message->m_message);
        m_messagePositionsStartY.emplace_back(pixelPosY);
        m_messagePositionsEndY.emplace_back(pixelPosY + totalMessageHeight);
        int heightDiff = totalMessageHeight + QMessage::Margin;

        pixelPosY += heightDiff;
        m_totalHeight += heightDiff;
    }

    setFixedHeight(m_totalHeight);
}

void QChatModel::ClearMessagePositions()
{
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

std::vector<QMessagePosition> QChatModel::GetMessagesForRender(uint64_t viewportStartY, uint64_t viewportEndY)
{
    std::vector<QMessagePosition> result;

    // Binary Search, using viewportStartY
    size_t leftIdx = 0;
    size_t rightIdx = m_messagePositionsStartY.size() - 1;
    size_t midIdx = 0;

    if(m_messagePositionsStartY.empty())
    {
        return result;
    }

    while(leftIdx < rightIdx)
    {
        midIdx = leftIdx + ((rightIdx - leftIdx) / 2);
        uint64_t midStartY = m_messagePositionsStartY[midIdx];
        uint64_t midEndY = m_messagePositionsEndY[midIdx];

        /*
            Searching Notes

            I think we want to have the start and end Y positions of the messages, and see if the
            viewportY interesects these "ranges". However, there are some edge cases.

            1. viewY < midStartY
                - midIdx completely out of view (should partition left side)
                - midIdx is first message VALID!
            2. viewY intersects midObj 
                - Valid! We found it
        */

        if(viewportStartY >= midStartY && viewportStartY <= midEndY)
        {
            break;
        }
        else if(viewportStartY < midStartY)
        {
            rightIdx = midIdx - 1;
        }
        else if(viewportStartY > midStartY)
        {
            leftIdx = midIdx + 1;
        }
    }

    // If we haven't found an intersection yet, this means we have a "clean" viewport, as in the first message is entirely visible and isn't partially shown.
    // This means we just get the next one after the viewport's start y

    uint64_t midStartY = m_messagePositionsStartY[midIdx];
    uint64_t midEndY = m_messagePositionsEndY[midIdx];
    if(viewportStartY > midEndY)
    {
        if(midIdx < m_messagePositionsStartY.size() - 1)
        {
            midIdx++;
        }
    }

    for(uint64_t posStartY = m_messagePositionsStartY[midIdx]; posStartY < viewportEndY;)
    {
        result.emplace_back(posStartY, midIdx);
        posStartY = m_messagePositionsStartY[++midIdx];
    }

    return result;
}

QMessagePosition::QMessagePosition(uint64_t startY, qsizetype index)
{
    m_startY = startY;
    m_index = index;
}
