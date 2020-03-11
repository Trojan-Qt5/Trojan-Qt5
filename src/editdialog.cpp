#include "editdialog.h"
#include "ui_editdialog.h"
#include "trojanvalidator.h"
#include "ip4validator.h"
#include "portvalidator.h"

EditDialog::EditDialog(Connection *_connection, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditDialog),
    connection(_connection)
{
    ui->setupUi(this);

    /* initialisation and validator setup */
    PortValidator *portValidator = new PortValidator(this);
    ui->serverPortEdit->setValidator(portValidator);

    ui->nameEdit->setText(connection->profile.name);
    ui->serverAddrEdit->setText(connection->profile.serverAddress);
    ui->serverPortEdit->setText(QString::number(connection->profile.serverPort));
    ui->verifyCertificateCheckBox->setChecked(connection->profile.verifyCertificate);
    ui->verifyHostnameCheckBox->setChecked(connection->profile.verifyHostname);
    ui->pwdEdit->setText(connection->profile.password);
    ui->reuseSessionCheckBox->setChecked(connection->profile.reuseSession);
    ui->sessionTicketCheckBox->setChecked(connection->profile.sessionTicket);
    ui->reusePortCheckBox->setChecked(connection->profile.reusePort);
    ui->tcpFastOpenCheckBox->setChecked(connection->profile.tcpFastOpen);
    ui->resetDateEdit->setDate(connection->profile.nextResetDate);
    ui->resetDateEdit->setMinimumDate(QDate::currentDate());
    ui->autoStartCheckBox->setChecked(connection->profile.autoStart);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &EditDialog::save);

    this->adjustSize();
}

EditDialog::~EditDialog()
{
    delete ui;
}

void EditDialog::save()
{
    connection->profile.name = ui->nameEdit->text();
    connection->profile.serverAddress = ui->serverAddrEdit->text().trimmed();
    connection->profile.serverPort = ui->serverPortEdit->text().toUShort();
    connection->profile.verifyCertificate = ui->verifyCertificateCheckBox->isChecked();
    connection->profile.verifyHostname = ui->verifyHostnameCheckBox->isChecked();
    connection->profile.reuseSession = ui->reuseSessionCheckBox->isChecked();
    connection->profile.sessionTicket = ui->sessionTicketCheckBox->isChecked();
    connection->profile.reusePort = ui->reusePortCheckBox->isChecked();
    connection->profile.tcpFastOpen = ui->tcpFastOpenCheckBox->isChecked();
    connection->profile.password = ui->pwdEdit->text();
    connection->profile.nextResetDate = ui->resetDateEdit->date();
    connection->profile.autoStart = ui->autoStartCheckBox->isChecked();

    this->accept();
}
