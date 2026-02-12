#pragma once

#include <string>
#include <QWidget>
#include "ui_QMessage.h"
#include <qlineedit.h>

class QMessage : public QWidget
{
    Q_OBJECT

public:
    QMessage(QWidget *parent = nullptr);
    ~QMessage();

    void setMessage(const std::string& msg);
    std::string getMessage() const;

private:
    Ui::QMessageClass ui;
    QLineEdit* lineEditMessage;
};

