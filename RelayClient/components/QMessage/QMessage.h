#pragma once

#include <string>
#include <QWidget>
#include <QLabel>
#include <QBoxLayout>
#include <qlineedit.h>

struct QMessageTextConstraints
{
    QSize m_usernameSize;
    QSize m_messageSize;
};

class QMessage : public QWidget
{
    Q_OBJECT

public:
    QMessage(QWidget *parent = nullptr);
    ~QMessage();

    void SetContents(std::string username, std::string message, int viewportWidth);

    static QMessageTextConstraints GetTextConstraints(std::string username, std::string message, int viewportWidth);

public:
    inline static const QFont Font = QFont("Arial");

    inline static const int LeftPadding = 15;
    inline static const int Margin = 0;
    inline static const int Padding = 0;
    inline static const int Spacing = 0;

private:
    QLabel* m_usernameLabel;
    QLabel* m_messageLabel;
};

