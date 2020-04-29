#include "aboutdialog.h"
#include "ui_aboutdialog.h"

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
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
