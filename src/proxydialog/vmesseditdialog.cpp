#include "vmesseditdialog.h"
#include "ui_vmesseditdialog.h"

VmessEditDialog::VmessEditDialog(Connection *_connection, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VmessEditDialog),
    connection(_connection)
{
    ui->setupUi(this);

    ui->nameEdit->setText(connection->profile.name);
    ui->serverAddrEdit->setText(connection->profile.serverAddress);
    ui->serverPortEdit->setText(QString::number(connection->profile.serverPort));
    ui->uuidEdit->setText(connection->profile.uuid);
    ui->alterIDEdit->setText(QString::number(connection->profile.alterID));
    ui->testsCB->setCurrentText(connection->profile.testsEnabled);
    ui->securityComboBox->setCurrentText(connection->profile.security);
    ui->tcpFastOpenCheckBox->setChecked(connection->profile.tcpFastOpen);
    ui->resetDateEdit->setDate(connection->profile.nextResetDate);
    ui->resetDateEdit->setMinimumDate(QDate::currentDate());
    ui->autoStartCheckBox->setChecked(connection->profile.autoStart);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &VmessEditDialog::save);

    streamWidget = new StreamWidget(this);
    streamWidget->setSettings(connection->profile.vmessSettings);
    ui->transportFrame->addWidget(streamWidget);
    this->adjustSize();
}

VmessEditDialog::~VmessEditDialog()
{
    delete ui;
}

void VmessEditDialog::save()
{
    connection->profile.type = "vmess";
    connection->profile.name = ui->nameEdit->text();
    connection->profile.serverAddress = ui->serverAddrEdit->text().trimmed();
    connection->profile.serverPort = ui->serverPortEdit->text().toUShort();
    connection->profile.security = ui->securityComboBox->currentText();
    connection->profile.uuid = ui->uuidEdit->text();
    connection->profile.alterID = ui->alterIDEdit->text().toInt();
    connection->profile.testsEnabled = ui->testsCB->currentText();
    connection->profile.tcpFastOpen = ui->tcpFastOpenCheckBox->isChecked();
    connection->profile.vmessSettings = streamWidget->getSettings();
    connection->profile.nextResetDate = ui->resetDateEdit->date();
    connection->profile.autoStart = ui->autoStartCheckBox->isChecked();

    this->accept();
}
