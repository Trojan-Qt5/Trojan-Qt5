#include "snelleditdialog.h"
#include "ui_snelleditdialog.h"

SnellEditDialog::SnellEditDialog(Connection *_connection, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SnellEditDialog),
    connection(_connection)
{
    ui->setupUi(this);
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

    this->accept();
}
