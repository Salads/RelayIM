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
    inline static const qsizetype MaxUsernameWidth = 100;
    inline static const qsizetype FixedMessageWidth = 300;
    inline static const qsizetype TotalWidth = MaxUsernameWidth + FixedMessageWidth;

    inline static const int Margin = 0;
    inline static const int Padding = 0;
    inline static const int Spacing = 5;

private:
    QLabel* m_usernameLabel;
    QLabel* m_messageLabel;
};

