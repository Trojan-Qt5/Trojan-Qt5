#include "snelleditdialog.h"
#include "ui_snelleditdialog.h"
#include "portvalidator.h"

SnellEditDialog::SnellEditDialog(Connection *_connection, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SnellEditDialog),
    connection(_connection)
{
    ui->setupUi(this);

    /* initialisation and validator setup */
    PortValidator *portValidator = new PortValidator(this);
    ui->serverPortEdit->setValidator(portValidator);

    ui->nameEdit->setText(connection->profile.name);
    ui->serverAddrEdit->setText(connection->profile.serverAddress);
    ui->serverPortEdit->setText(QString::number(connection->profile.serverPort));
    ui->pwdEdit->setText(connection->profile.password);
    ui->obfsComboBox->setCurrentText(connection->profile.obfs);
    ui->obfsParamEdit->setText(connection->profile.obfsParam);
    ui->tcpFastOpenCheckBox->setChecked(connection->profile.tcpFastOpen);
    ui->resetDateEdit->setDate(connection->profile.nextResetDate);
    ui->resetDateEdit->setMinimumDate(QDate::currentDate());
    ui->autoStartCheckBox->setChecked(connection->profile.autoStart);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &SnellEditDialog::save);

    this->adjustSize();
}

SnellEditDialog::~SnellEditDialog()
{
    delete ui;
}

void SnellEditDialog::save()
{
    connection->profile.type = "snell";
    connection->profile.name = ui->nameEdit->text();
    connection->profile.serverAddress = ui->serverAddrEdit->text().trimmed();
    connection->profile.serverPort = ui->serverPortEdit->text().toUShort();
    connection->profile.password = ui->pwdEdit->text();
    connection->profile.obfs = ui->obfsComboBox->currentText();
    connection->profile.obfsParam = ui->obfsParamEdit->text();
    connection->profile.tcpFastOpen = ui->tcpFastOpenCheckBox->isChecked();
    connection->profile.nextResetDate = ui->resetDateEdit->date();
    connection->profile.autoStart = ui->autoStartCheckBox->isChecked();

    this->accept();
}
