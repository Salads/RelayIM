#pragma once

#include <string>
#include <QWidget>
#include <QLabel>
#include <QBoxLayout>
#include <qlineedit.h>

class QMessage : public QWidget
{
    Q_OBJECT

public:
    QMessage(QWidget *parent = nullptr);
    ~QMessage();

    void SetContents(std::string username, std::string message);

public:
    inline static const QFont Font = QFont("Arial");
    inline static const qsizetype UsernameWidth = 100;
    inline static const qsizetype MessageWidth = 300;
    inline static const qsizetype Margin = 11;
    inline static const qsizetype TotalWidth = UsernameWidth + MessageWidth + (Margin * 2);

private:
    QLabel* m_usernameLabel;
    QLabel* m_messageLabel;
};

