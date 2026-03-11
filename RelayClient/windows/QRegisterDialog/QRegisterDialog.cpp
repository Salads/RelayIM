#include "QRegisterDialog.h"

QRegisterDialog::QRegisterDialog(QModelManager* manager, QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("Register Username");
    setMinimumSize(300, 100);
    
    m_modelManager = manager;

    QVBoxLayout* mainVLayout = new QVBoxLayout(this);
    QFormLayout* formLayout = new QFormLayout();
    QHBoxLayout* buttonLayout = new QHBoxLayout();

    mainVLayout->addLayout(formLayout);
    mainVLayout->addLayout(buttonLayout);

    QLabel* usernameLabel = new QLabel("Username:");
    m_usernameLineEdit = new QLineEdit();
    m_usernameResultLabel = new QLabel();

    formLayout->addRow(usernameLabel, m_usernameLineEdit);
    formLayout->addRow(nullptr, m_usernameResultLabel);

    m_OKButton = new QPushButton("OK");
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_OKButton);

    connect(m_modelManager, &QModelManager::RegisterResult, this, [this](bool success)
    {
        if(success)
        {
            close();
        }
        else
        {
            m_usernameResultLabel->setStyleSheet("QLabel { color: red }");
            m_usernameResultLabel->setText("Username Taken");
            m_OKButton->setDisabled(true);
        }

    }, Qt::QueuedConnection);

    connect(m_OKButton, &QPushButton::clicked, this, [this](bool checked)
    {
        m_modelManager->GetClient()->SendConnect(m_usernameLineEdit->text().toStdString());
        m_OKButton->setDisabled(true);
    });
}

QRegisterDialog::~QRegisterDialog()
{}
