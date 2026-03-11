#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QFormLayout>

#include "models/QModelManager/QModelManager.h"

// Used to register a username with the server
class QRegisterDialog  : public QDialog
{
    Q_OBJECT

public:
    QRegisterDialog(QModelManager* manager, QWidget *parent = nullptr);
    ~QRegisterDialog();

private:
    QLineEdit* m_usernameLineEdit;

    QModelManager* m_modelManager;
    QPushButton* m_OKButton;

    QLabel* m_usernameResultLabel;
};

