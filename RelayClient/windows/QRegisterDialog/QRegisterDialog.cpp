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

    m_OKButton = new QPushButton("Register");
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_OKButton);

    connect(m_modelManager, &QModelManager::Event_RegisterResponse, this, &QRegisterDialog::Slot_RegisterResponse);
    connect(m_OKButton, &QPushButton::clicked, this, &QRegisterDialog::Slot_OkButtonClicked);
}

QRegisterDialog::~QRegisterDialog()
{}

void QRegisterDialog::Slot_OkButtonClicked(bool checked)
{
    std::string desiredUsername = m_usernameLineEdit->text().toStdString();
    PacketResponseReason usernameCheckResult = m_modelManager->CheckDesiredUsername(desiredUsername);
    if(usernameCheckResult == PacketResponseReason::Success)
    {
        m_modelManager->GetClient()->SendConnect(m_usernameLineEdit->text().toStdString());
        m_OKButton->setText("Registering...");
        m_OKButton->setDisabled(true);
    }
    else
    {
        std::string reasonDesc;
        switch(usernameCheckResult)
        {
            case PacketResponseReason::UsernameInvalid:
                reasonDesc = "Username must not be empty, and not start with a space.";
                break;
            case PacketResponseReason::UsernameTaken:
                reasonDesc = "Username has been taken.";
                break;
            default:
                reasonDesc = "Unknown reason.";
                break;
        }

        QMessageBox msgBox;
        msgBox.setWindowTitle("Username error");
        msgBox.setText(QString::fromStdString(reasonDesc));
        msgBox.exec();
    }

    qDebug() << "Register button callback finished";
}

void QRegisterDialog::Slot_RegisterResponse(PacketResponseReason reason, PeerID peerID, std::string username)
{
    qDebug() << "Register Response - " << ResponseTypeToString(reason);
    if(reason == PacketResponseReason::Success)
    {
        m_registerSuccess = true;
        close();
    }
    else
    {
        m_usernameResultLabel->setStyleSheet("QLabel { color: red }");
        m_usernameResultLabel->setText("Username Taken");
        m_OKButton->setText("Register");
        m_OKButton->setDisabled(false);
    }
}

bool QRegisterDialog::GetRegistered()
{
    return m_registerSuccess;
}
