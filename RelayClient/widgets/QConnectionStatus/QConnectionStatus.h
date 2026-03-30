#ifndef QCONNECTIONSTATUS_H
#define QCONNECTIONSTATUS_H

#include <QWidget>
#include <QPainter>

class QConnectionStatus : public QWidget
{
    Q_OBJECT

public:

    enum class Status {NotConnected, Connecting, ConnectedUnregistered, ConnectedRegistered};

    QConnectionStatus(QWidget *parent = nullptr);
    ~QConnectionStatus();

    void SetStatus(QConnectionStatus::Status status);

protected:
    void paintEvent(QPaintEvent*);

private:
    Status m_status = Status::NotConnected;
    QString m_statusStr = "";
    Qt::GlobalColor m_iconColor;
};

#endif // QCONNECTIONSTATUS_H

