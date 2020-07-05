#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "themehelper.h"

#include <QPushButton>
#include <QMessageBox>
#include <QStyleFactory>
#include <QApplication>
#include <QTranslator>

SettingsDialog::SettingsDialog(ConfigHelper *ch, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog),
    helper(ch)
{
    ui->setupUi(this);

    helper->readGeneralSettings();

    // general settings
    ui->toolbarStyleComboBox->setCurrentIndex(helper->getGeneralSettings().toolBarStyle);
    ui->logLevelComboBox->setCurrentIndex(helper->getGeneralSettings().logLevel);
    //ui->haproxyModeComboBox->setCurrentIndex();
    ui->themeComboBox->addItems(QStyleFactory::keys());
    ui->themeComboBox->setCurrentText(helper->getGeneralSettings().theme);
    ui->systemThemeCB->setCurrentIndex(helper->getGeneralSettings().systemTheme);
    if (ui->systemThemeCB->currentIndex() == 1 || ui->systemThemeCB->currentIndex() == 2) {
        ui->themeComboBox->setDisabled(true);
        ui->themeComboBox->setCurrentText("Fusion");
    }
    ui->languageCB->setCurrentText(helper->getGeneralSettings().language);
    ui->systemTrayMaximumServerEdit->setText(QString::number(helper->getGeneralSettings().systemTrayMaximumServer));
    ui->hideCheckBox->setChecked(helper->getGeneralSettings().hideWindowOnStartup);
    ui->startAtLoginCheckbox->setChecked(helper->getGeneralSettings().startAtLogin);
    ui->oneInstanceCheckBox->setChecked(helper->getGeneralSettings().onlyOneInstace);
    ui->availabilityCheckBox->setChecked(helper->getGeneralSettings().checkPortAvailability);
    ui->enableNotificationCheckBox->setChecked(helper->getGeneralSettings().enableNotification);
    ui->hideDockIconCheckBox->setChecked(helper->getGeneralSettings().hideDockIcon);
    ui->nativeMenuBarCheckBox->setChecked(helper->getGeneralSettings().nativeMenuBar);
    ui->showAirportAndDonationCB->setChecked(helper->getGeneralSettings().showAirportAndDonation);
    // inbound settings
    ui->enableHttpProxyCheckBox->setChecked(helper->getInboundSettings().enableHttpMode);
    ui->enableIPV6SupportCheckBox->setChecked(helper->getInboundSettings().enableIpv6Support);
    ui->shareOverLANCheckBox->setChecked(helper->getInboundSettings().shareOverLan);
    ui->inboundSniffingCB->setChecked(helper->getInboundSettings().inboundSniffing);
    ui->socks5PortLineEdit->setText(QString::number(helper->getInboundSettings().socks5LocalPort));
    ui->httpPortLineEdit->setText(QString::number(helper->getInboundSettings().httpLocalPort));
    ui->pacPortLineEdit->setText(QString::number(helper->getInboundSettings().pacLocalPort));
    ui->haproxyPortLineEdit->setText(QString::number(helper->getInboundSettings().haproxyPort));
    ui->haproxyStatusPortLineEdit->setText(QString::number(helper->getInboundSettings().haproxyStatusPort));
    // outbound settings
    ui->bypassBittorrentCB->setChecked(helper->getOutboundSettings().bypassBittorrent);
    ui->bypassPrivateAddressCB->setChecked(helper->getOutboundSettings().bypassPrivateAddress);
    ui->bypassChinaMainlandCB->setChecked(helper->getOutboundSettings().bypassChinaMainland);
    ui->forwardProxyCheckBox->setChecked(helper->getOutboundSettings().forwardProxy);
    ui->forwardProxyTypeComboBox->setCurrentIndex(helper->getOutboundSettings().forwardProxyType);
    ui->forwardProxyIpAddressLineEdit->setText(helper->getOutboundSettings().forwardProxyAddress);
    ui->forwardProxyPortLineEdit->setText(QString::number(helper->getOutboundSettings().forwardProxyPort));
    ui->forwardProxyAuthenticationCheckBox->setChecked(helper->getOutboundSettings().forwardProxyAuthentication);
    ui->forwardProxyUsername->setText(helper->getOutboundSettings().forwardProxyUsername);
    ui->forwardProxyPassword->setText(helper->getOutboundSettings().forwardProxyPassword);
    // test settings
    ui->latencyTestMethodCB->setCurrentIndex(helper->getTestSettings().method);
    ui->realLatencyTestUrlEdit->setText(helper->getTestSettings().latencyTestUrl);
    ui->speedTestUrlEdit->setText(helper->getTestSettings().speedTestUrl);
    // subscribe settings
    ui->gfwlistUpdateUrlComboBox->setCurrentIndex(helper->getSubscribeSettings().gfwListUrl);
    ui->maximumSubscribe->setText(QString::number(helper->getSubscribeSettings().maximumSubscribe));
    ui->updateUserAgentLineEdit->setText(helper->getSubscribeSettings().updateUserAgent);
    ui->filterKeywordLineEdit->setText(helper->getSubscribeSettings().filterKeyword);
    ui->autoFetchGroupnameCB->setChecked(helper->getSubscribeSettings().autoFetchGroupName);
    ui->overwriteAllowInsecureCB->setChecked(helper->getSubscribeSettings().overwriteAllowInsecure);
    ui->overwriteAllowInsecureCiphersCB->setChecked(helper->getSubscribeSettings().overwriteAllowInsecureCiphers);
    ui->overwriteTcpFastOpenCB->setChecked(helper->getSubscribeSettings().overwriteTcpFastOpen);
    // graph settings
    ui->downloadColorPicker->setCurrentColor(QColor(helper->getGraphSettings().downloadSpeedColor));
    ui->uploadColorPicker->setCurrentColor(QColor(helper->getGraphSettings().uploadSpeedColor));
    // trojan settings
    ui->tlsFingerprintComboBox->setCurrentIndex(helper->getTrojanSettings().fingerprint);
    ui->enableAPICheckBox->setChecked(helper->getTrojanSettings().enableTrojanAPI);
    ui->enableRouterCheckBox->setChecked(helper->getTrojanSettings().enableTrojanRouter);
    ui->apiPortLineEdit->setText(QString::number(helper->getTrojanSettings().trojanAPIPort));
    ui->certLineEdit->setText(helper->getTrojanSettings().trojanCertPath);
    ui->cipherLineEdit->setText(helper->getTrojanSettings().trojanCipher);
    ui->cipherTLS13LineEdit->setText(helper->getTrojanSettings().trojanCipherTLS13);
    ui->bufferSizeLineEdit->setText(QString::number(helper->getTrojanSettings().bufferSize));
    ui->geoPathEdit->setText(helper->getTrojanSettings().geoPath);

    routeWidget = new RouteWidget();
    routeWidget->setConfig(helper->getRouterSettings());
    ui->routeSettingsLayout->addWidget(routeWidget);

    connect(ui->systemThemeCB, &QComboBox::currentTextChanged, this, &SettingsDialog::onThemeChanged);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &SettingsDialog::onAccepted);
    this->adjustSize();
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::onThemeChanged()
{
    if (ui->systemThemeCB->currentIndex() == 1 || ui->systemThemeCB->currentIndex() == 2) {
        ui->themeComboBox->setDisabled(true);
        ui->themeComboBox->setCurrentText("Fusion");
    }
    else
        ui->themeComboBox->setDisabled(false);
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

    GeneralSettings generalSettings = helper->getGeneralSettings();
    generalSettings.theme = ui->themeComboBox->currentText();
    generalSettings.systemTheme = ui->systemThemeCB->currentIndex();
    generalSettings.language = ui->languageCB->currentText();
    generalSettings.toolBarStyle = ui->toolbarStyleComboBox->currentIndex();
    generalSettings.logLevel = ui->logLevelComboBox->currentIndex();
    generalSettings.systemTrayMaximumServer = ui->systemTrayMaximumServerEdit->text().toInt();
    generalSettings.startAtLogin = ui->startAtLoginCheckbox->isChecked();
    generalSettings.hideWindowOnStartup = ui->hideCheckBox->isChecked();
    generalSettings.onlyOneInstace = ui->oneInstanceCheckBox->isChecked();
    generalSettings.checkPortAvailability = ui->availabilityCheckBox->isChecked();
    generalSettings.enableNotification = ui->enableNotificationCheckBox->isChecked();
    generalSettings.hideDockIcon = ui->hideDockIconCheckBox->isChecked();
    generalSettings.nativeMenuBar = ui->nativeMenuBarCheckBox->isChecked();
    generalSettings.showAirportAndDonation = ui->showAirportAndDonationCB->isChecked();

    InboundSettings inboundSettings = helper->getInboundSettings();
    inboundSettings.enableHttpMode = ui->enableHttpProxyCheckBox->isChecked();
    inboundSettings.shareOverLan = ui->shareOverLANCheckBox->isChecked();
    inboundSettings.enableIpv6Support = ui->enableIPV6SupportCheckBox->isChecked();
    inboundSettings.inboundSniffing = ui->inboundSniffingCB->isChecked();
    inboundSettings.socks5LocalPort = ui->socks5PortLineEdit->text().toInt();
    inboundSettings.httpLocalPort = ui->httpPortLineEdit->text().toInt();
    inboundSettings.pacLocalPort = ui->pacPortLineEdit->text().toInt();
    inboundSettings.haproxyStatusPort = ui->haproxyStatusPortLineEdit->text().toInt();
    inboundSettings.haproxyPort = ui->haproxyPortLineEdit->text().toInt();

    OutboundSettings outboundSettings = helper->getOutboundSettings();
    outboundSettings.bypassBittorrent = ui->bypassBittorrentCB->isChecked();
    outboundSettings.bypassPrivateAddress = ui->bypassPrivateAddressCB->isChecked();
    outboundSettings.bypassChinaMainland = ui->bypassChinaMainlandCB->isChecked();
    outboundSettings.forwardProxy = ui->forwardProxyCheckBox->isChecked();
    outboundSettings.forwardProxyType = ui->forwardProxyTypeComboBox->currentIndex();
    outboundSettings.forwardProxyAddress = ui->forwardProxyIpAddressLineEdit->text();
    outboundSettings.forwardProxyPort = ui->forwardProxyPortLineEdit->text().toInt();
    outboundSettings.forwardProxyAuthentication = ui->forwardProxyAuthenticationCheckBox->isChecked();
    outboundSettings.forwardProxyUsername = ui->forwardProxyUsername->text();
    outboundSettings.forwardProxyPassword = ui->forwardProxyPassword->text();

    TestSettings testSettings = helper->getTestSettings();
    testSettings.method = ui->latencyTestMethodCB->currentIndex();
    testSettings.latencyTestUrl = ui->realLatencyTestUrlEdit->text();
    testSettings.speedTestUrl = ui->speedTestUrlEdit->text();

    SubscribeSettings subscribeSettings = helper->getSubscribeSettings();
    subscribeSettings.gfwListUrl = ui->gfwlistUpdateUrlComboBox->currentIndex();
    subscribeSettings.updateUserAgent = ui->updateUserAgentLineEdit->text();
    subscribeSettings.filterKeyword = ui->filterKeywordLineEdit->text();
    subscribeSettings.maximumSubscribe = ui->maximumSubscribe->text().toInt();
    subscribeSettings.autoFetchGroupName = ui->autoFetchGroupnameCB->isChecked();
    subscribeSettings.overwriteAllowInsecure = ui->overwriteAllowInsecureCB->isChecked();
    subscribeSettings.overwriteAllowInsecureCiphers = ui->overwriteAllowInsecureCiphersCB->isChecked();
    subscribeSettings.overwriteTcpFastOpen = ui->overwriteTcpFastOpenCB->isChecked();

    GraphSettings graphSettings = helper->getGraphSettings();
    graphSettings.downloadSpeedColor = ui->downloadColorPicker->currentColor().name();
    graphSettings.uploadSpeedColor = ui->uploadColorPicker->currentColor().name();

    RouterSettings routerSettings = routeWidget->getConfig();

    TrojanSettings trojanSettings = helper->getTrojanSettings();
    trojanSettings.fingerprint = ui->tlsFingerprintComboBox->currentIndex();
    trojanSettings.enableTrojanAPI = ui->enableAPICheckBox->isChecked();
    trojanSettings.enableTrojanRouter = ui->enableRouterCheckBox->isChecked();
    trojanSettings.trojanAPIPort = ui->apiPortLineEdit->text().toInt();
    trojanSettings.trojanCertPath = ui->certLineEdit->text();
    trojanSettings.trojanCipher = ui->cipherLineEdit->text();
    trojanSettings.trojanCipherTLS13 = ui->cipherTLS13LineEdit->text();
    trojanSettings.geoPath = ui->geoPathEdit->text();
    trojanSettings.bufferSize = ui->bufferSizeLineEdit->text().toInt();

    helper->setGeneralSettings(generalSettings, inboundSettings, outboundSettings, testSettings, subscribeSettings, graphSettings, routerSettings, trojanSettings);

    // setup style
    QApplication::setStyle(ui->themeComboBox->currentText());

    // apply light/dark theme
    ThemeHelper::setupThemeOnChange(ui->systemThemeCB->currentIndex());

    this->accept();
}

void SettingsDialog::onChanged()
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}
