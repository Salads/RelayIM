#include "QChatModel.h"
#include "widgets/QChatView/QChatView.h"

QChatModel::QChatModel(QChatView* view, QModelManager* manager, QWidget* parent)
    : m_chatView(view), m_manager(manager), QWidget(parent)
{
    m_messagePositionsStartY.reserve(UINT16_MAX);

    m_vBar = view->verticalScrollBar();

    connect(m_manager, &QModelManager::Event_RoomUpdate_Message, this, &QChatModel::Slot_RoomUpdate_Message);
    connect(m_manager, &QModelManager::Event_RoomUpdate_FULL, this, &QChatModel::Slot_RoomUpdate_FULL);

    //connect(m_vBar, &QScrollBar::valueChanged, this, &QChatModel::RenderObjects);

    QPalette pal = QPalette();
    pal.setColor(QPalette::Window, "#999999");
    setAutoFillBackground(true);
    setPalette(pal);
}

void QChatModel::RenderObjects()
{
    float min = m_vBar->minimum();
    float max = m_vBar->maximum();
    float val = m_vBar->value();

    uint64_t pixStartY = 0;
    uint64_t pixEndY = m_totalHeight;

    // Scrollbar is visible, meaning we are larger than it.
    if(min && max && val)
    {
        float percentVal = (val - min) / (max - min);
        uint64_t pixStartY = m_totalHeight * percentVal;
        uint64_t pixEndY = pixStartY + m_vBar->pageStep();
    }

    std::vector<QMessagePosition> positions = GetMessagesForRender(pixStartY, pixEndY);

    // Make sure we have the exact amount of QMessage objects
    qsizetype nObjectsNeeded = positions.size();
    qsizetype nObjects = m_messageObjects.size();

    qDebug("QChatModel::RenderObjects - nPositions=%u, nObjects=%u", nObjectsNeeded, nObjects);

    for(int i = nObjects; i < nObjectsNeeded; i++)
    {
        QMessage* newMessageObj = new QMessage(this);
        m_messageObjects.push_back(newMessageObj);
    }

    nObjects = m_messageObjects.size();
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

        qDebug("\tRendering QMessage(%d->%d) at (%u, %u)", pos->m_index, i, 0, pos->m_startY);

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

void QChatModel::Slot_RoomUpdate_Message(RoomID roomID, PeerID peerID, std::string message)
{
    PrecalculateMessagePositions();
    m_vBar->setValue(m_vBar->maximum());
    RenderObjects();
}

void QChatModel::Slot_RoomUpdate_FULL(RoomID roomID, std::shared_ptr<std::vector<ChatMessage>> messages)
{
    ClearMessagePositions();
    PrecalculateMessagePositions();
    m_vBar->setValue(m_vBar->maximum());
    RenderObjects();
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

/*
    Messages and Rendering
    ---------------------------------------
    Messages come in, we _append_ to a vector. This means that "new" messages are at the back, and order is oldest->newest via index.
    When rendering, we start from the top to the bottom, so that also means oldest->newest

    When we store message positions, we iterate forwards in the messages list. So, this means we store these oldest->newest as well.
    
    When we calculate a new position for a new message, we don't have to modify every other position, because we simply increase the size of the parent
    background widget that these messages are parented to. The oldest message will always have the same position, because origin is top-left.
*/
void QChatModel::PrecalculateMessagePositions()
{
    if(m_roomID == INVALID_ROOM_ID)
    {
        return;
    }

    size_t startIdx = m_messagePositionsStartY.size();
    uint64_t pixelPosY = QMessage::Margin;

    qDebug("QChatModel::PrecalculateMessagePositions - nPos=%u", startIdx);

    if(startIdx > 0)
    {
        pixelPosY = m_messagePositionsEndY[startIdx - 1];
    }

    for(int i = startIdx; i < m_messages->size(); i++)
    {
        const ChatMessage* message = &m_messages->at(i);

        uint64_t totalMessageHeight = GetMessageHeight(message->m_message);
        uint64_t nextYPosDiff = totalMessageHeight + QMessage::Margin;
        uint64_t nextYPos = pixelPosY + nextYPosDiff;

        qDebug("\tNew Pos: %u (height=%u)", pixelPosY, nextYPosDiff);

        m_messagePositionsStartY.emplace_back(pixelPosY);
        m_messagePositionsEndY.emplace_back(nextYPos);

        pixelPosY = nextYPos;
        m_totalHeight = nextYPos;
    }

    setFixedHeight(m_totalHeight);
    setMinimumSize(QMessage::TotalWidth, m_totalHeight);
}

void QChatModel::ClearMessagePositions()
{
    qDebug("QChatModel::ClearMessagePositions");
    ClearRenderObjects();
    m_messagePositionsStartY.clear();
    m_messagePositionsEndY.clear();
    m_totalHeight = 0;
}

void QChatModel::ClearRenderObjects()
{
    qDebug("QChatModel::ClearRenderObjects");
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
    qsizetype leftIdx = 0;
    qsizetype rightIdx = m_messagePositionsStartY.size() - 1;
    qsizetype midIdx = 0;

    qDebug("Start - LeftIdx=%u, RightIdx=%u, viewportStartY=%u, viewportEndY=%u, nPositions:%u", leftIdx, rightIdx, viewportStartY, viewportEndY, m_messagePositionsStartY.size());

    if(m_messagePositionsStartY.empty())
    {
        qDebug("End - No positions!");
        return result;
    }

    while(leftIdx < rightIdx)
    {
        midIdx = leftIdx + ((rightIdx - leftIdx) / 2);
        uint64_t midStartY = m_messagePositionsStartY[midIdx];
        uint64_t midEndY = m_messagePositionsEndY[midIdx];

        qDebug("\tIteration - MidIdx=%u [%u, %u], LeftIdx=%u, RightIdx=%u", midIdx, midStartY, midEndY, leftIdx, rightIdx);

        if(viewportStartY >= midStartY && viewportStartY <= midEndY)
        {
            qDebug("\t\tFound Intersection");
            break;
        }
        else if(viewportStartY < midStartY)
        {
            qDebug("\t\tCant go more left, we found it.");
            if(midIdx == 0)
            {
                break;
            }

            qDebug("\t\tUse Left Half");
            rightIdx = midIdx - 1;
        }
        else if(viewportStartY > midStartY)
        {
            qDebug("\t\tCant go more right, we found it.");
            if(midIdx == m_messagePositionsStartY.size() - 1)
            {
                break;
            }

            qDebug("\t\tUse Right Half");
            leftIdx = midIdx + 1;
        }
    }

    // If we haven't found an intersection yet, this means we have a "clean" viewport, as in the first message is entirely visible and isn't partially shown.
    // This means we just get the next one after the viewport's start y

    //uint64_t midStartY = m_messagePositionsStartY[midIdx];
    //uint64_t midEndY = m_messagePositionsEndY[midIdx];
    //if(viewportStartY > midEndY) // The message we got is above, and completely hidden
    //{
    //    if(midIdx < m_messagePositionsStartY.size() - 1)
    //    {
    //        qDebug("Viewport Check - Message is above, get next one");
    //        midIdx++;
    //    }
    //}

    qDebug("Gather Items from MidIdx");
    uint64_t posStartY = m_messagePositionsStartY[midIdx];
    while(posStartY <= viewportEndY && midIdx < m_messagePositionsStartY.size())
    {
        QMessagePosition newPos(posStartY, midIdx);
        result.push_back(newPos);

        if(++midIdx < m_messagePositionsStartY.size())
        {
            posStartY = m_messagePositionsStartY[midIdx];
        }
    }

    return result;
}

QMessagePosition::QMessagePosition(uint64_t startY, qsizetype index)
{
    m_startY = startY;
    m_index = index;
}
