#pragma once

#include <QDialog>
#include <QBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QLineEdit>
#include <QPushButton>
#include <QListView>
#include <QGroupBox>
#include <QMessageBox>
#include "ui_QChatRoomsDialog.h"

#include "models/QModelManager/QModelManager.h"
#include "models/QJoinChatRoomsModel/QJoinChatRoomsModel.h"

class QChatRoomsDialog : public QDialog
{
    Q_OBJECT

public:
    QChatRoomsDialog(QModelManager* manager, QWidget *parent = nullptr);
    ~QChatRoomsDialog();

private:
    Ui::QChatRoomsDialogClass ui;

    QModelManager* m_manager;

    QJoinChatRoomsModel m_model;

    QListView* m_joinableRoomsListView;

    QLineEdit* m_createRoomLineEdit;
    QLabel* m_createRoomLabel;
    QLabel* m_createRoomErrorLabel;

    QPushButton* m_createButton;
};

