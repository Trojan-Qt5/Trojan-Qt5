#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include "confighelper.h"
#include "utils.h"

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

    ConfigHelper *conf = Utils::getConfigHelper();

    if (!conf->getGeneralSettings().showAirportAndDonation) {
        // remove them inversely to delete completely
        ui->tabWidget->removeTab(3);
        ui->tabWidget->removeTab(2);
        ui->tabWidget->removeTab(1);
    } else {
        ui->mieLinkLabel->setUrl("https://rakuten-co-jp.club/register?aff=COELWU");
        ui->westWorldSSLabel->setUrl("https://xbsj5673.xyz");
        ui->qingWanLabel->setUrl("https://www.qwyun.vip/auth/register?code=COELWU");
        ui->youyun666Label->setUrl("https://youyun666.com/auth/register?code=FDqS");
    }
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
