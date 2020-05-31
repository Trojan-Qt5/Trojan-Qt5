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

    ui->toolbarStyleComboBox->setCurrentIndex(helper->getGeneralSettings()["toolbarStyle"].toInt());
    ui->logLevelComboBox->setCurrentIndex(helper->getGeneralSettings()["logLevel"].toInt());
    //ui->haproxyModeComboBox->setCurrentIndex();
    ui->themeComboBox->addItems(QStyleFactory::keys());
    ui->themeComboBox->setCurrentText(helper->getGeneralSettings()["theme"].toString());
    ui->darkModeCB->setChecked(helper->getGeneralSettings()["darkTheme"].toBool());
    ui->hideCheckBox->setChecked(helper->getGeneralSettings()["hideWindowOnStartup"].toBool());
    ui->startAtLoginCheckbox->setChecked(helper->getGeneralSettings()["startAtLogin"].toBool());
    ui->oneInstanceCheckBox->setChecked(helper->getGeneralSettings()["onlyOneInstace"].toBool());
    ui->availabilityCheckBox->setChecked(helper->getGeneralSettings()["checkPortAvailability"].toBool());
    ui->enableNotificationCheckBox->setChecked(helper->getGeneralSettings()["enableNotification"].toBool());
    ui->hideDockIconCheckBox->setChecked(helper->getGeneralSettings()["hideDockIcon"].toBool());
    ui->nativeMenuBarCheckBox->setChecked(helper->getGeneralSettings()["nativeMenuBar"].toBool());
    ui->showAirportAndDonationCB->setChecked(helper->getGeneralSettings()["showAirportAndDonation"].toBool());
    ui->enableHttpProxyCheckBox->setChecked(helper->getInboundSettings()["enableHttpMode"].toBool());
    ui->enableIPV6SupportCheckBox->setChecked(helper->getInboundSettings()["enableIpv6Support"].toBool());
    ui->shareOverLANCheckBox->setChecked(helper->getInboundSettings()["shareOverLan"].toBool());
    ui->socks5PortLineEdit->setText(QString::number(helper->getInboundSettings()["socks5LocalPort"].toInt()));
    ui->httpPortLineEdit->setText(QString::number(helper->getInboundSettings()["httpLocalPort"].toInt()));
    ui->pacPortLineEdit->setText(QString::number(helper->getInboundSettings()["pacLocalPort"].toInt()));
    ui->haproxyPortLineEdit->setText(QString::number(helper->getInboundSettings()["haproxyPort"].toInt()));
    ui->haproxyStatusPortLineEdit->setText(QString::number(helper->getInboundSettings()["haproxyStatusPort"].toInt()));
    ui->forwardProxyCheckBox->setChecked(helper->getOutboundSettings()["forwardProxy"].toBool());
    ui->forwardProxyTypeComboBox->setCurrentIndex(helper->getOutboundSettings()["forwardProxyType"].toInt());
    ui->forwardProxyIpAddressLineEdit->setText(helper->getOutboundSettings()["forwardProxyAddress"].toString());
    ui->forwardProxyPortLineEdit->setText(QString::number(helper->getOutboundSettings()["forwardProxyPort"].toInt()));
    ui->forwardProxyAuthenticationCheckBox->setChecked(helper->getOutboundSettings()["forwardProxyAuthentication"].toBool());
    ui->forwardProxyUsername->setText(helper->getOutboundSettings()["forwardProxyUsername"].toString());
    ui->forwardProxyPassword->setText(helper->getOutboundSettings()["forwardProxyPassword"].toString());
    ui->gfwlistUpdateUrlComboBox->setCurrentText(helper->getSubscribeSettings()["gfwListUrl"].toString());
    ui->maximumSubscribe->setText(QString::number(helper->getSubscribeSettings()["maximumSubscribe"].toInt()));
    ui->updateUserAgentLineEdit->setText(helper->getSubscribeSettings()["updateUserAgent"].toString());
    ui->filterKeywordLineEdit->setText(helper->getSubscribeSettings()["filterKeyword"].toString());
    ui->autoFetchGroupnameCB->setChecked(helper->getSubscribeSettings()["autoFetchGroupName"].toBool());
    ui->overwriteAllowInsecureCB->setChecked(helper->getSubscribeSettings()["overwriteAllowInsecure"].toBool());
    ui->overwriteAllowInsecureCiphersCB->setChecked(helper->getSubscribeSettings()["overwriteAllowInsecureCiphers"].toBool());
    ui->overwriteTcpFastOpenCB->setChecked(helper->getSubscribeSettings()["overwriteTcpFastOpen"].toBool());
    ui->tlsFingerprintComboBox->setCurrentIndex(helper->getTrojanSettings()["fingerprint"].toInt());
    ui->enableAPICheckBox->setChecked(helper->getTrojanSettings()["enableTrojanAPI"].toBool());
    ui->enableRouterCheckBox->setChecked(helper->getTrojanSettings()["enableTrojanRouter"].toBool());
    ui->apiPortLineEdit->setText(QString::number(helper->getTrojanSettings()["trojanAPIPort"].toInt()));
    ui->certLineEdit->setText(helper->getTrojanSettings()["trojanCertPath"].toString());
    ui->cipherLineEdit->setText(helper->getTrojanSettings()["trojanCipher"].toString());
    ui->cipherTLS13LineEdit->setText(helper->getTrojanSettings()["trojanCipherTLS13"].toString());
    ui->bufferSizeLineEdit->setText(QString::number(helper->getTrojanSettings()["bufferSize"].toInt()));

    routeWidget = new RouteWidget();
    routeWidget->setConfig(helper->getRouterSettings());
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

    QJsonObject generalSettings = helper->getGeneralSettings();
    generalSettings["theme"] = ui->themeComboBox->currentText();
    generalSettings["darkTheme"] = ui->darkModeCB->isChecked();
    generalSettings["toolbarStyle"] = ui->toolbarStyleComboBox->currentIndex();
    generalSettings["logLevel"] = ui->logLevelComboBox->currentIndex();
    generalSettings["startAtLogin"] = ui->startAtLoginCheckbox->isChecked();
    generalSettings["hideWindowOnStartup"] = ui->hideCheckBox->isChecked();
    generalSettings["onlyOneInstace"] = ui->oneInstanceCheckBox->isChecked();
    generalSettings["checkPortAvailability"] = ui->availabilityCheckBox->isChecked();
    generalSettings["enableNotification"] = ui->enableNotificationCheckBox->isChecked();
    generalSettings["hideDockIcon"] = ui->hideDockIconCheckBox->isChecked();
    generalSettings["nativeMenuBar"] = ui->nativeMenuBarCheckBox->isChecked();
    generalSettings["showAirportAndDonation"] = ui->showAirportAndDonationCB->isChecked();

    QJsonObject inboundSettings = helper->getInboundSettings();
    inboundSettings["enableHttpMode"] = ui->enableHttpProxyCheckBox->isChecked();
    inboundSettings["shareOverLan"] = ui->shareOverLANCheckBox->isChecked();
    inboundSettings["enableIpv6Support"] = ui->enableIPV6SupportCheckBox->isChecked();
    inboundSettings["socks5LocalPort"] = ui->socks5PortLineEdit->text().toInt();
    inboundSettings["httpLocalPort"] = ui->httpPortLineEdit->text().toInt();
    inboundSettings["pacLocalPort"] = ui->pacPortLineEdit->text().toInt();
    inboundSettings["haproxyStatusPort"] = ui->haproxyStatusPortLineEdit->text().toInt();
    inboundSettings["haproxyPort"] = ui->haproxyPortLineEdit->text().toInt();

    QJsonObject outboundSettings = helper->getOutboundSettings();
    outboundSettings["forwardProxy"] = ui->forwardProxyCheckBox->isChecked();
    outboundSettings["forwardProxyType"] = ui->forwardProxyTypeComboBox->currentIndex();
    outboundSettings["forwardProxyAddress"] = ui->forwardProxyIpAddressLineEdit->text();
    outboundSettings["forwardProxyPort"] = ui->forwardProxyPortLineEdit->text().toInt();
    outboundSettings["forwardProxyAuthentication"] = ui->forwardProxyAuthenticationCheckBox->isChecked();
    outboundSettings["forwardProxyUsername"] = ui->forwardProxyUsername->text();
    outboundSettings["forwardProxyPassword"] = ui->forwardProxyPassword->text();

    QJsonObject subscribeSettings = helper->getSubscribeSettings();
    subscribeSettings["gfwListUrl"] = ui->gfwlistUpdateUrlComboBox->currentIndex();
    subscribeSettings["updateUserAgent"] = ui->updateUserAgentLineEdit->text();
    subscribeSettings["filterKeyword"] = ui->filterKeywordLineEdit->text();
    subscribeSettings["maximumSubscribe"] = ui->maximumSubscribe->text().toInt();
    subscribeSettings["autoFetchGroupName"] = ui->autoFetchGroupnameCB->isChecked();
    subscribeSettings["overwriteAllowInsecure"] = ui->overwriteAllowInsecureCB->isChecked();
    subscribeSettings["overwriteAllowInsecureCiphers"] = ui->overwriteAllowInsecureCiphersCB->isChecked();
    subscribeSettings["overwriteTcpFastOpen"] = ui->overwriteTcpFastOpenCB->isChecked();

    QJsonObject routerSettings = routeWidget->getConfig();

    QJsonObject trojanSettings = helper->getTrojanSettings();
    trojanSettings["fingerprint"] = ui->tlsFingerprintComboBox->currentText();
    trojanSettings["enableTrojanAPI"] = ui->enableAPICheckBox->isChecked();
    trojanSettings["enableTrojanRouter"] = ui->enableRouterCheckBox->isChecked();
    trojanSettings["trojanAPIPort"] = ui->apiPortLineEdit->text().toInt();
    trojanSettings["trojanCertPath"] = ui->certLineEdit->text();
    trojanSettings["trojanCipher"] = ui->cipherLineEdit->text();
    trojanSettings["trojanCipherTLS13"] = ui->cipherTLS13LineEdit->text();
    trojanSettings["bufferSize"] = ui->bufferSizeLineEdit->text().toInt();

    helper->setGeneralSettings(generalSettings, inboundSettings, outboundSettings, subscribeSettings, routerSettings, trojanSettings);

    QApplication::setStyle(ui->themeComboBox->currentText());

    this->accept();
}

void SettingsDialog::onChanged()
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}
