#include "advancesettingsdialog.h"
#include "ui_advancesettingsdialog.h"
#include <QPushButton>

AdvanceSettingsDialog::AdvanceSettingsDialog(ConfigHelper *ch, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AdvanceSettingsDialog),
    helper(ch)
{
    ui->setupUi(this);


    ui->logLevelComboBox->setCurrentIndex(helper->getLogLevel());
    ui->httpModeCheckBox->setChecked(helper->isEnableHttpMode());
    ui->enableIpv6SupportCheckBox->setChecked(helper->isEnableIpv6Support());
    ui->shareOverLanCheckBox->setChecked(helper->isShareOverLan());
    ui->socks5Port->setText(QString::number(helper->getSocks5Port()));
    ui->httpPort->setText(QString::number(helper->getHttpPort()));
    ui->pacPort->setText(QString::number(helper->getPACPort()));
    ui->haproxyPort->setText(QString::number(helper->getHaproxyPort()));
    ui->haproxyStatusPort->setText(QString::number(helper->getHaproxyStatusPort()));

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &AdvanceSettingsDialog::onAccepted);
    connect(ui->logLevelComboBox, &QComboBox::currentTextChanged, this, &AdvanceSettingsDialog::onChanged);
    connect(ui->httpModeCheckBox, &QCheckBox::stateChanged, this, &AdvanceSettingsDialog::onChanged);
    connect(ui->enableIpv6SupportCheckBox, &QCheckBox::stateChanged, this, &AdvanceSettingsDialog::onChanged);
    connect(ui->shareOverLanCheckBox, &QCheckBox::stateChanged, this, &AdvanceSettingsDialog::onChanged);
    connect(ui->socks5Port, &QLineEdit::textChanged, this, &AdvanceSettingsDialog::onChanged);
    connect(ui->httpPort, &QLineEdit::textChanged, this, &AdvanceSettingsDialog::onChanged);
    connect(ui->pacPort, &QLineEdit::textChanged, this, &AdvanceSettingsDialog::onChanged);
    connect(ui->haproxyPort, &QLineEdit::textChanged, this, &AdvanceSettingsDialog::onChanged);
    connect(ui->haproxyStatusPort, &QLineEdit::textChanged, this, &AdvanceSettingsDialog::onChanged);

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

    this->adjustSize();
}

AdvanceSettingsDialog::~AdvanceSettingsDialog()
{
    delete ui;
}

void AdvanceSettingsDialog::onAccepted()
{
    helper->setAdvanceSettings(ui->logLevelComboBox->currentIndex(),
                               ui->httpModeCheckBox->isChecked(),
                               ui->enableIpv6SupportCheckBox->isChecked(),
                               ui->shareOverLanCheckBox->isChecked(),
                               ui->socks5Port->text().toInt(),
                               ui->httpPort->text().toInt(),
                               ui->pacPort->text().toInt(),
                               ui->haproxyPort->text().toInt(),
                               ui->haproxyStatusPort->text().toInt());
    this->accept();
}

void AdvanceSettingsDialog::onChanged()
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}
