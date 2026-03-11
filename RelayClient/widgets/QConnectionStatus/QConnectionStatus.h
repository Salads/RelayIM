#pragma once

#include <QWidget>
#include <QPainter>
#include "ui_QConnectionStatus.h"

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
    Ui::QConnectionStatusClass ui;

    Status m_status = Status::NotConnected;
    QString m_statusStr = "";
    Qt::GlobalColor m_iconColor;
};

