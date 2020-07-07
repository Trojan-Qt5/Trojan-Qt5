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
    ui->pwdEdit->setText(connection->profile.password);
    ui->sniEdit->setText(connection->profile.sni);
    ui->reuseSessionCheckBox->setChecked(connection->profile.reuseSession);
    ui->sessionTicketCheckBox->setChecked(connection->profile.sessionTicket);
    ui->resetDateEdit->setDate(connection->profile.nextResetDate);
    ui->resetDateEdit->setMinimumDate(QDate::currentDate());
    ui->autoStartCheckBox->setChecked(connection->profile.autoStart);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &TrojanEditDialog::save);

    trojanGoWidget = new TrojanGoWidget(this);
    trojanGoWidget->setSettings(connection->profile.trojanGoSettings);
    ui->trojangoFrame->addWidget(trojanGoWidget);
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
    connection->profile.reuseSession = ui->reuseSessionCheckBox->isChecked();
    connection->profile.sessionTicket = ui->sessionTicketCheckBox->isChecked();
    connection->profile.password = ui->pwdEdit->text();
    connection->profile.sni = ui->sniEdit->text();
    connection->profile.trojanGoSettings = trojanGoWidget->getSettings();
    connection->profile.nextResetDate = ui->resetDateEdit->date();
    connection->profile.autoStart = ui->autoStartCheckBox->isChecked();

    this->accept();
}
