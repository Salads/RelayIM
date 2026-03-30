#ifndef QCHATROOM_H
#define QCHATROOM_H

#include <QPushButton>
#include <QLabel>
#include <QBoxLayout>

// UI for representing a chat room
class QChatRoom : public QPushButton
{
    Q_OBJECT

public:
    QChatRoom(QWidget *parent = nullptr);
    ~QChatRoom();

    void initialize(int roomId, const std::string roomName);

private:
    QLabel* m_roomNameLabel;
};

#endif // QCHATROOM_H

