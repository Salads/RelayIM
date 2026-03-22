#pragma once

#include <stdint.h>
#include <memory>
#include <vector>
#include <cstdint>

#include <QWidget>
#include <QList>
#include <QFontMetrics>
#include <QScrollBar>
#include <QVBoxLayout>

#include "ChatMessage.h"
#include "models/QModelManager/QModelManager.h"
#include "components/QMessage/QMessage.h"

class QChatView;
//#include "widgets/QChatView/QChatView.h"

using std::vector;

/// <summary>
/// For use with QChatModel, holds result for a single chat message to be rendered.
/// </summary>
struct QMessagePosition
{
    QMessagePosition(uint64_t startY, uint64_t endY, qsizetype index);
    uint64_t m_startY; // Pixel position relative to parent
    uint64_t m_endY;
    qsizetype m_index; // Index of message data in m_messages
};

/// <summary>
/// Handles the search for positioning of UI data for QChatView.
/// </summary>
class QChatModel : public QWidget
{
    Q_OBJECT

public:
    QChatModel(QChatView* view, QModelManager* manager, QWidget* parent = nullptr);

    void SetRoom(RoomID roomID);

    void Refresh();

public slots:
    void Slot_RoomUpdate_Message(RoomID roomID, PeerID peerID, std::string message);
    void Slot_RoomUpdate_FULL(RoomID roomID, std::shared_ptr<std::vector<ChatMessage>> messages);
    void Slot_ScrollValueChanged(int value);
    void Slot_ScrollRangeChanged(int min, int max);

private:
    /// <summary>
    /// Updates the positioning information for new messages that come in. 
    /// 
    /// Does not clear positions already calculated, just makes sure we have a position for every message.
    /// </summary>
    void PrecalculateMessagePositions();

    uint32_t GetMessageHeight(const std::string& username, const std::string& message);

    void RenderObjects();

    void ClearMessagePositions();
    void ClearRenderObjects();
    std::vector<QMessagePosition> GetMessagesForRender(uint64_t viewportStartY, uint64_t viewportEndY);

private:

    QChatView* m_chatView;
    QModelManager* m_manager;

    QScrollBar* m_vBar;

    RoomID m_roomID = INVALID_ROOM_ID;

    vector<uint64_t> m_messagePositionsStartY;
    vector<uint64_t> m_messagePositionsEndY;
    QList<ChatMessage>* m_messages;
    QList<QMessage*> m_messageObjects;

    uint64_t m_totalHeight = 0;
    
    const int c_chatWindowMargin = 10;
};

