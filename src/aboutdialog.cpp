#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include "confighelper.h"

#include <openssl/opensslv.h>
#include <qrencode.h>

#include <QFile>
#include <QDesktopServices>

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    QFile file(":/credits/credits.html");
    file.open(QIODevice::ReadOnly);
    ui->textBrowser->setHtml(file.readAll());
    file.close();

    ui->version->setText(QStringLiteral(APP_VERSION));
    ui->buildInfo->setText(TROJAN_QT5_BUILD_INFO);
    ui->buildExInfo->setText(TROJAN_QT5_BUILD_EXTRA_INFO);
    ui->buildTime->setText(__DATE__ " " __TIME__);

#ifdef Q_OS_WIN
    QString configFile = qApp->applicationDirPath() + "/config.ini";
#else
    QDir configDir = QDir::homePath() + "/.config/trojan-qt5";
    QString configFile = configDir.absolutePath() + "/config.ini";
#endif
    ConfigHelper *conf = new ConfigHelper(configFile);

    if (!conf->getGeneralSettings()["showAirportAndDonation"].toBool()) {
        ui->mieLinkLabel->setStyleSheet("");
        ui->westWorldSSLabel->setStyleSheet("");
        ui->donationXMR->setStyleSheet("");
    } else {
        ui->mieLinkLabel->setUrl("https://rakuten-co-jp.club/register?aff=COELWU");
        ui->westWorldSSLabel->setUrl("https://xbsj5673.xyz");
    }
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
