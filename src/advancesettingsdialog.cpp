#include "advancesettingsdialog.h"
#include "ui_advancesettingsdialog.h"
#include <QPushButton>

AdvanceSettingsDialog::AdvanceSettingsDialog(ConfigHelper *ch, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AdvanceSettingsDialog),
    helper(ch)
{
    ui->setupUi(this);

    ui->socks5Address->setText(helper->getSocks5Address());
    ui->socks5Port->setText(QString::number(helper->getSocks5Port()));
    ui->httpAddress->setText(helper->getHttpAddress());
    ui->httpPort->setText(QString::number(helper->getHttpPort()));
    ui->pacAddress->setText(helper->getPACAddress());
    ui->pacPort->setText(QString::number(helper->getPACPort()));

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &AdvanceSettingsDialog::onAccepted);
    connect(ui->socks5Address, &QLineEdit::textChanged, this, &AdvanceSettingsDialog::onChanged);
    connect(ui->socks5Port, &QLineEdit::textChanged, this, &AdvanceSettingsDialog::onChanged);
    connect(ui->httpAddress, &QLineEdit::textChanged, this, &AdvanceSettingsDialog::onChanged);
    connect(ui->httpPort, &QLineEdit::textChanged, this, &AdvanceSettingsDialog::onChanged);
    connect(ui->pacAddress, &QLineEdit::textChanged, this, &AdvanceSettingsDialog::onChanged);
    connect(ui->pacPort, &QLineEdit::textChanged, this, &AdvanceSettingsDialog::onChanged);

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

    this->adjustSize();
}

AdvanceSettingsDialog::~AdvanceSettingsDialog()
{
    delete ui;
}

void AdvanceSettingsDialog::onAccepted()
{
    helper->setAdvanceSettings(ui->socks5Address->text(),
                               ui->socks5Port->text().toInt(),
                               ui->httpAddress->text(),
                               ui->httpPort->text().toInt(),
                               ui->pacAddress->text(),
                               ui->pacPort->text().toInt());
    this->accept();
}

void AdvanceSettingsDialog::onChanged()
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}
