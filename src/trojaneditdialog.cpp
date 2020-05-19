#include "trojaneditdialog.h"
#include "ui_trojaneditdialog.h"
#include "generalvalidator.h"
#include "ip4validator.h"
#include "portvalidator.h"

TrojanEditDialog::TrojanEditDialog(Connection *_connection, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TrojanEditDialog),
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
    ui->sniEdit->setText(connection->profile.sni);
    ui->reuseSessionCheckBox->setChecked(connection->profile.reuseSession);
    ui->sessionTicketCheckBox->setChecked(connection->profile.sessionTicket);
    ui->reusePortCheckBox->setChecked(connection->profile.reusePort);
    ui->tcpFastOpenCheckBox->setChecked(connection->profile.tcpFastOpen);
    ui->muxCheckBox->setChecked(connection->profile.mux);
    ui->muxConcurrencyEdit->setText(QString::number(connection->profile.muxConcurrency));
    ui->websocketCheckBox->setChecked(connection->profile.websocket);
    ui->websocketDoubleTLSCheckBox->setChecked(connection->profile.websocketDoubleTLS);
    ui->websocketPathEdit->setText(connection->profile.websocketPath);
    ui->websocketHostnameEdit->setText(connection->profile.websocketHostname);
    ui->websocketObfsPasswordEdit->setText(connection->profile.websocketObfsPassword);
    ui->resetDateEdit->setDate(connection->profile.nextResetDate);
    ui->resetDateEdit->setMinimumDate(QDate::currentDate());
    ui->autoStartCheckBox->setChecked(connection->profile.autoStart);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &TrojanEditDialog::save);

    this->adjustSize();
}

TrojanEditDialog::~TrojanEditDialog()
{
    delete ui;
}

void TrojanEditDialog::save()
{
    connection->profile.type = "trojan";
    connection->profile.name = ui->nameEdit->text();
    connection->profile.serverAddress = ui->serverAddrEdit->text().trimmed();
    connection->profile.serverPort = ui->serverPortEdit->text().toUShort();
    connection->profile.verifyCertificate = ui->verifyCertificateCheckBox->isChecked();
    connection->profile.verifyHostname = ui->verifyHostnameCheckBox->isChecked();
    connection->profile.reuseSession = ui->reuseSessionCheckBox->isChecked();
    connection->profile.sessionTicket = ui->sessionTicketCheckBox->isChecked();
    connection->profile.reusePort = ui->reusePortCheckBox->isChecked();
    connection->profile.tcpFastOpen = ui->tcpFastOpenCheckBox->isChecked();
    connection->profile.mux = ui->muxCheckBox->isChecked();
    connection->profile.muxConcurrency = ui->muxConcurrencyEdit->text().toInt();
    connection->profile.websocket = ui->websocketCheckBox->isChecked();
    connection->profile.websocketDoubleTLS = ui->websocketDoubleTLSCheckBox->isChecked();
    connection->profile.websocketPath = ui->websocketPathEdit->text();
    connection->profile.websocketHostname = ui->websocketHostnameEdit->text();
    connection->profile.websocketObfsPassword = ui->websocketObfsPasswordEdit->text();
    connection->profile.password = ui->pwdEdit->text();
    connection->profile.sni = ui->sniEdit->text();
    connection->profile.nextResetDate = ui->resetDateEdit->date();
    connection->profile.autoStart = ui->autoStartCheckBox->isChecked();

    this->accept();
}
