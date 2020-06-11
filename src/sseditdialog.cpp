#include "sseditdialog.h"
#include "ui_sseditdialog.h"
#include "portvalidator.h"

SSEditDialog::SSEditDialog(Connection *_connection, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SSEditDialog),
    connection(_connection)
{
    ui->setupUi(this);

    /* initialisation and validator setup */
    PortValidator *portValidator = new PortValidator(this);
    ui->serverPortEdit->setValidator(portValidator);

    ui->nameEdit->setText(connection->profile.name);
    ui->serverAddrEdit->setText(connection->profile.serverAddress);
    ui->serverPortEdit->setText(QString::number(connection->profile.serverPort));
    ui->methodComboBox->setCurrentText(connection->profile.method);
    ui->pwdEdit->setText(connection->profile.password);
    ui->pluginEdit->setText(connection->profile.plugin);
    ui->pluginOptionsEdit->setText(connection->profile.pluginParam);
    ui->tcpFastOpenCheckBox->setChecked(connection->profile.tcpFastOpen);
    ui->resetDateEdit->setDate(connection->profile.nextResetDate);
    ui->resetDateEdit->setMinimumDate(QDate::currentDate());
    ui->autoStartCheckBox->setChecked(connection->profile.autoStart);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &SSEditDialog::save);

    this->adjustSize();
}

SSEditDialog::~SSEditDialog()
{
    delete ui;
}

void SSEditDialog::save()
{
    connection->profile.type = "ss";
    connection->profile.name = ui->nameEdit->text();
    connection->profile.serverAddress = ui->serverAddrEdit->text().trimmed();
    connection->profile.serverPort = ui->serverPortEdit->text().toUShort();
    connection->profile.method = ui->methodComboBox->currentText();
    connection->profile.password = ui->pwdEdit->text();
    connection->profile.plugin = ui->pluginEdit->text();
    connection->profile.pluginParam = ui->pluginOptionsEdit->text();
    connection->profile.tcpFastOpen = ui->tcpFastOpenCheckBox->isChecked();
    connection->profile.nextResetDate = ui->resetDateEdit->date();
    connection->profile.autoStart = ui->autoStartCheckBox->isChecked();

    this->accept();
}
