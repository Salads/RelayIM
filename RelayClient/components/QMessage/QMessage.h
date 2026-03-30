#ifndef QMESSAGE_H
#define QMESSAGE_H

#include <QWidget>
#include <QLabel>
#include <QTextEdit>
#include <QBoxLayout>
#include <QLineEdit>

#include <string>

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

    void setContents(std::string username, std::string message, int viewportWidth);

    static QMessageTextConstraints getTextConstraints(std::string username, std::string message, int viewportWidth);

public:
    inline static const QFont Font = QFont("Arial");

    inline static const int TextPadding = 5;
    inline static const int LeftPadding = 15;
    inline static const int Margin = 0;
    inline static const int Padding = 0;
    inline static const int Spacing = 0;

private:
    QLabel* m_usernameLabel;
    QTextEdit* m_messageEdit;
};

#endif // QMESSAGE_H

