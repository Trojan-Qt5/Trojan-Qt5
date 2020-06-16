#include "ssreditdialog.h"
#include "ui_ssreditdialog.h"
#include "portvalidator.h"

SSREditDialog::SSREditDialog(Connection *_connection, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SSREditDialog),
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
    ui->protocolComboBox->setCurrentText(connection->profile.protocol);
    ui->protocolParamEdit->setText(connection->profile.protocolParam);
    ui->obfsComboBox->setCurrentText(connection->profile.obfs);
    ui->obfsParamEdit->setText(connection->profile.obfsParam);
    ui->tcpFastOpenCheckBox->setChecked(connection->profile.tcpFastOpen);
    ui->resetDateEdit->setDate(connection->profile.nextResetDate);
    ui->resetDateEdit->setMinimumDate(QDate::currentDate());
    ui->autoStartCheckBox->setChecked(connection->profile.autoStart);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &SSREditDialog::save);

    this->adjustSize();
}

SSREditDialog::~SSREditDialog()
{
    delete ui;
}

void SSREditDialog::save()
{
    connection->profile.type = "ssr";
    connection->profile.name = ui->nameEdit->text();
    connection->profile.serverAddress = ui->serverAddrEdit->text().trimmed();
    connection->profile.serverPort = ui->serverPortEdit->text().toUShort();
    connection->profile.method = ui->methodComboBox->currentText();
    connection->profile.password = ui->pwdEdit->text();
    connection->profile.protocol = ui->protocolComboBox->currentText();
    connection->profile.protocolParam = ui->protocolParamEdit->text();
    connection->profile.obfs = ui->obfsComboBox->currentText();
    connection->profile.obfsParam = ui->obfsParamEdit->text();
    connection->profile.tcpFastOpen = ui->tcpFastOpenCheckBox->isChecked();
    connection->profile.nextResetDate = ui->resetDateEdit->date();
    connection->profile.autoStart = ui->autoStartCheckBox->isChecked();

    this->accept();
}
