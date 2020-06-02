#include "routewidget.h"
#include "ui_routewidget.h"
#include <QJsonArray>
#include <QStringBuilder>
#include <QFile>
#include <QJsonDocument>
#include <QFileDialog>

RouteWidget::RouteWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RouteWidget)
{
    ui->setupUi(this);

    directDomainTxt = new QTextEdit(this);
    proxyDomainTxt = new QTextEdit(this);
    blockDomainTxt = new QTextEdit(this);

    directIPTxt = new QTextEdit(this);
    proxyIPTxt = new QTextEdit(this);
    blockIPTxt = new QTextEdit(this);

    ui->directTxtLayout->addWidget(directDomainTxt);
    ui->proxyTxtLayout->addWidget(proxyDomainTxt);
    ui->blockTxtLayout->addWidget(blockDomainTxt);

    ui->directIPLayout->addWidget(directIPTxt);
    ui->proxyIPLayout->addWidget(proxyIPTxt);
    ui->blockIPLayout->addWidget(blockIPTxt);
}

RouteWidget::~RouteWidget()
{
    delete ui;
}

void RouteWidget::setText(QJsonObject object, QString input, QString level1, QString level2)
{
    QStringList datas = input.replace("\r", "").split("\n");
    QJsonArray array;
    QJsonObject ip = object["ip"].toObject();
    QJsonObject domain = object["domain"].toObject();
    for (auto data : datas)
    {
        if (!data.trimmed().isEmpty())
        {
            array.push_back(data);
         }
    }
    if (level1 == "ip") {
        ip[level2] = array;
        object[level1] = ip;
    } else {
        domain[level2] = array;
        object[level1] = domain;
    }
}

QString RouteWidget::getText(QJsonObject object, QString level1, QString level2)
{
    QString datas;
    foreach(const QJsonValue &var, object[level1].toObject()[level2].toArray()) {
        QString data = var.toString();
        datas = datas % data % "\r\n";
    }
    return datas;
}

void RouteWidget::setConfig(QJsonObject r)
{
    route = r;

    ui->domainStrategyCombo->setCurrentText(route["domainStrategy"].toString());
    // domain
    directDomainTxt->setPlainText(getText(route, "domain", "direct"));
    proxyDomainTxt->setPlainText(getText(route, "domain", "proxy"));
    blockDomainTxt->setPlainText(getText(route, "domain", "block"));
    // ip
    directIPTxt->setPlainText(getText(route, "ip", "direct"));
    proxyIPTxt->setPlainText(getText(route, "ip", "proxy"));
    blockIPTxt->setPlainText(getText(route, "ip", "block"));
}

QJsonObject RouteWidget::getConfig()
{
    route["domainStrategy"] = ui->domainStrategyCombo->currentText();
    setText(route, directDomainTxt->toPlainText(), "domain", "direct");
    setText(route, proxyDomainTxt->toPlainText(), "domain", "proxy");
    setText(route, blockDomainTxt->toPlainText(), "domain", "block");
    setText(route, directIPTxt->toPlainText(), "ip", "direct");
    setText(route, proxyIPTxt->toPlainText(), "ip", "proxy");
    setText(route, blockIPTxt->toPlainText(), "ip", "block");
    return route;
}

void RouteWidget::on_importRulesBtn_clicked()
{
    QString file = QFileDialog::getOpenFileName(
        this,
        tr("Import Rules from rules.json"),
         QString(),
        "Trojan-Qt5 Route Rules Configuration (rules.json)");

    QFile JSONFile(file);
    JSONFile.open(QIODevice::ReadOnly | QIODevice::Text);

    QJsonDocument doc = QJsonDocument::fromJson(JSONFile.readAll());
    JSONFile.close();
    QJsonObject object = doc.object();

    directDomainTxt->setText(getText(object, "domains", "direct"));
    proxyDomainTxt->setText(getText(object, "domains", "proxy"));
    blockDomainTxt->setText(getText(object, "domains", "block"));

    directIPTxt->setText(getText(object, "ips", "direct"));
    proxyIPTxt->setText(getText(object, "ips", "proxy"));
    blockIPTxt->setText(getText(object, "ips", "block"));

}

void RouteWidget::on_exportRulesBtn_clicked()
{
    QString file = QFileDialog::getSaveFileName(
        this,
        tr("Exports Rules to rules.json"),
         QString(),
        "Trojan-Qt5 Route Rules Configuration (rules.json)");

    QFile JSONFile(file);
    JSONFile.open(QIODevice::ReadOnly | QIODevice::Text);

    QJsonObject object;

    setText(object, directDomainTxt->toPlainText(), "domain", "direct");
    setText(object, proxyDomainTxt->toPlainText(), "domain", "proxy");
    setText(object, blockDomainTxt->toPlainText(), "domain", "block");

    setText(object, directIPTxt->toPlainText(), "ip", "direct");
    setText(object, proxyIPTxt->toPlainText(), "ip", "proxy");
    setText(object, blockIPTxt->toPlainText(), "ip", "block");

    QJsonDocument doc = QJsonDocument(object);

    JSONFile.write(doc.toJson());
    JSONFile.close();
}
