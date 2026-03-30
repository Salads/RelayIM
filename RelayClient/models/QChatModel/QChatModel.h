#ifndef QCHATMODEL_H
#define QCHATMODEL_H

#include <QWidget>
#include <QScrollBar>
#include <QVector>

#include "models/QModelManager/QModelManager.h"
#include "components/QMessage/QMessage.h"
#include "widgets/QChatView/QChatView.h"

#include "ChatMessage.h"
#include "RoomID.h"
#include "PeerID.h"

#include <memory>
#include <vector>
#include <cstdint>

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

    void setRoom(RoomID roomID);

    void refresh();
    void handleResize(QSize oldSize, QSize newSize);
    void renderObjects();

public slots:
    void slotRoomUpdateMessage(RoomID roomID, PeerID peerID, std::string message);
    void slotRoomUpdateFull(RoomID roomID, std::shared_ptr<std::vector<ChatMessage>> messages);
    void slotScrollValueChanged(int value);
    void slotScrollRangeChanged(int min, int max);

private:
    /// <summary>
    /// Updates the positioning information for new messages that come in. 
    /// 
    /// Does not clear positions already calculated, just makes sure we have a position for every message.
    /// </summary>
    void precalculateMessagePositions(bool fromFirstMultiline);

    uint32_t getMessageHeight(const std::string& username, const std::string& message);
    bool getMessageMultiline(const std::string& username, const std::string& message);

    void clearMessagePositions();
    void clearRenderObjects();
    QVector<QMessagePosition> getMessagesForRender(uint64_t viewportStartY, uint64_t viewportEndY);

private:

    QChatView* m_chatView;
    QModelManager* m_manager;

    QScrollBar* m_vBar;

    RoomID m_roomId;

    QVector<uint64_t> m_messagePositionsStartY;
    QVector<uint64_t> m_messagePositionsEndY;
    QList<ChatMessage>* m_messages;
    QList<QMessage*> m_messageObjects;

    int64_t m_firstMultilineMessageIdx = -1;

    uint64_t m_totalHeight = 0;
    
    const int c_chatWindowMargin = 10;
};

#endif // QCHATMODEL_H

