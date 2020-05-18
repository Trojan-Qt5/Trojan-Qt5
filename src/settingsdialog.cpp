#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include <QPushButton>
#include <QMessageBox>
#include <QStyleFactory>
#include <QApplication>

SettingsDialog::SettingsDialog(ConfigHelper *ch, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog),
    helper(ch)
{
    ui->setupUi(this);

    helper->readGeneralSettings();

    ui->toolbarStyleComboBox->setCurrentIndex(helper->getToolbarStyle());
    ui->logLevelComboBox->setCurrentIndex(helper->getLogLevel());
    //ui->haproxyModeComboBox->setCurrentIndex();
    ui->themeComboBox->addItems(QStyleFactory::keys());
    ui->themeComboBox->setCurrentText(helper->getTheme());
    ui->hideCheckBox->setChecked(helper->isHideWindowOnStartup());
    ui->startAtLoginCheckbox->setChecked(helper->isStartAtLogin());
    ui->oneInstanceCheckBox->setChecked(helper->isOnlyOneInstance());
    ui->availabilityCheckBox->setChecked(helper->isCheckPortAvailability());
    ui->enableNotificationCheckBox->setChecked(helper->isEnableNotification());
    ui->hideDockIconCheckBox->setChecked(helper->isHideDockIcon());
    ui->nativeMenuBarCheckBox->setChecked(helper->isNativeMenuBar());
    ui->enableHttpProxyCheckBox->setChecked(helper->isEnableHttpMode());
    ui->enableIPV6SupportCheckBox->setChecked(helper->isEnableIpv6Support());
    ui->shareOverLANCheckBox->setChecked(helper->isShareOverLan());
    ui->socks5PortLineEdit->setText(QString::number(helper->getSocks5Port()));
    ui->httpPortLineEdit->setText(QString::number(helper->getHttpPort()));
    ui->pacPortLineEdit->setText(QString::number(helper->getPACPort()));
    ui->haproxyPortLineEdit->setText(QString::number(helper->getHaproxyPort()));
    ui->haproxyStatusPortLineEdit->setText(QString::number(helper->getHaproxyStatusPort()));
    ui->forwardProxyCheckBox->setChecked(helper->isEnableForwardProxy());
    ui->forwardProxyTypeComboBox->setCurrentIndex(helper->getForwardProxyType());
    ui->forwardProxyIpAddressLineEdit->setText(helper->getForwardProxyAddress());
    ui->forwardProxyPortLineEdit->setText(QString::number(helper->getForwardProxyPort()));
    ui->forwardProxyAuthenticationCheckBox->setChecked(helper->isEnableForwardProxyAuthentication());
    ui->forwardProxyUsername->setText(helper->getForwardProxyUsername());
    ui->forwardProxyPassword->setText(helper->getForwardProxyPassword());
    ui->gfwlistUpdateUrlComboBox->setCurrentIndex(helper->getGfwlistUrl());
    ui->updateUserAgentLineEdit->setText(helper->getUpdateUserAgent());
    ui->filterKeywordLineEdit->setText(helper->getFilterKeyword());
    ui->maximumSubscribe->setText(QString::number(helper->getMaximumSubscribe()));
    ui->tlsFingerprintComboBox->setCurrentIndex(helper->getFLSFingerPrint());
    ui->enableAPICheckBox->setChecked(helper->isEnableTrojanAPI());
    ui->enableRouterCheckBox->setChecked(helper->isEnableTrojanRouter());
    ui->apiPortLineEdit->setText(QString::number(helper->getTrojanAPIPort()));
    ui->certLineEdit->setText(helper->getTrojanCertPath());
    ui->cipherLineEdit->setText(helper->getTrojanCipher());
    ui->cipherTLS13LineEdit->setText(helper->getTrojanCipherTLS13());
    ui->bufferSizeLineEdit->setText(QString::number(helper->getBufferSize()));

    routeWidget = new RouteWidget();
    routeWidget->setConfig(helper->getRoute());
    ui->routeSettingsLayout->addWidget(routeWidget);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &SettingsDialog::onAccepted);
    this->adjustSize();
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::onAccepted()
{
#if defined (Q_OS_MAC)
    // this will result in no title bar
    if (ui->hideDockIconCheckBox->isChecked() && ui->nativeMenuBarCheckBox->isChecked()) {
        QMessageBox::critical(this, tr("Invalid"),
                              tr("You can not hide dock Icon and use native menu bar at the same time"));
        return;
    }
#endif

    helper->setGeneralSettings(ui->toolbarStyleComboBox->currentIndex(),
                               ui->hideCheckBox->isChecked(),
                               ui->themeComboBox->currentText(),
                               ui->startAtLoginCheckbox->isChecked(),
                               ui->oneInstanceCheckBox->isChecked(),
                               ui->availabilityCheckBox->isChecked(),
                               ui->enableNotificationCheckBox->isChecked(),
                               ui->hideDockIconCheckBox->isChecked(),
                               ui->nativeMenuBarCheckBox->isChecked(),
                               ui->logLevelComboBox->currentIndex(),
                               ui->enableHttpProxyCheckBox->isChecked(),
                               ui->enableIPV6SupportCheckBox->isChecked(),
                               ui->shareOverLANCheckBox->isChecked(),
                               ui->socks5PortLineEdit->text().toInt(),
                               ui->httpPortLineEdit->text().toInt(),
                               ui->pacPortLineEdit->text().toInt(),
                               ui->haproxyPortLineEdit->text().toInt(),
                               ui->haproxyStatusPortLineEdit->text().toInt(),
                               ui->forwardProxyCheckBox->isChecked(),
                               ui->forwardProxyTypeComboBox->currentIndex(),
                               ui->forwardProxyIpAddressLineEdit->text(),
                               ui->forwardProxyPortLineEdit->text().toInt(),
                               ui->forwardProxyAuthenticationCheckBox->isChecked(),
                               ui->forwardProxyUsername->text(),
                               ui->forwardProxyPassword->text(),
                               ui->gfwlistUpdateUrlComboBox->currentIndex(),
                               ui->updateUserAgentLineEdit->text(),
                               ui->filterKeywordLineEdit->text(),
                               ui->maximumSubscribe->text().toInt(),
                               ui->tlsFingerprintComboBox->currentIndex(),
                               ui->enableAPICheckBox->isChecked(),
                               ui->enableRouterCheckBox->isChecked(),
                               ui->apiPortLineEdit->text().toInt(),
                               ui->certLineEdit->text(),
                               ui->cipherLineEdit->text(),
                               ui->cipherTLS13LineEdit->text(),
                               ui->bufferSizeLineEdit->text().toInt());

    helper->setRoute(routeWidget->getConfig());

    QApplication::setStyle(ui->themeComboBox->currentText());

    this->accept();
}

void SettingsDialog::onChanged()
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}
