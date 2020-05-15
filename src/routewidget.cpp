#include "routewidget.h"
#include "ui_routewidget.h"
#include <QJsonArray>
#include <QStringBuilder>

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

void RouteWidget::setText(QString input, QString level1, QString level2)
{
    QStringList datas = input.replace("\r", "").split("\n");
    QJsonArray array;
    QJsonObject ip = route["ip"].toObject();
    QJsonObject domain = route["domain"].toObject();
    for (auto data : datas)
    {
        if (!data.trimmed().isEmpty())
        {
            array.push_back(data);
         }
    }
    if (level1 == "ip") {
        ip[level2] = array;
        route[level1] = ip;
    } else {
        domain[level2] = array;
        route[level1] = domain;
    }
}

QString RouteWidget::getText(QString level1, QString level2)
{
    QString datas;
    foreach(const QJsonValue &var, route[level1].toObject()[level2].toArray()) {
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
    directDomainTxt->setPlainText(getText("domain", "direct"));
    proxyDomainTxt->setPlainText(getText("domain", "proxy"));
    blockDomainTxt->setPlainText(getText("domain", "block"));
    // ip
    directIPTxt->setPlainText(getText("ip", "direct"));
    proxyIPTxt->setPlainText(getText("ip", "proxy"));
    blockIPTxt->setPlainText(getText("ip", "block"));
}

QJsonObject RouteWidget::getConfig()
{
    route["domainStrategy"] = ui->domainStrategyCombo->currentText();
    setText(directDomainTxt->toPlainText(), "domain", "direct");
    setText(proxyDomainTxt->toPlainText(), "domain", "proxy");
    setText(blockDomainTxt->toPlainText(), "domain", "block");
    setText(directIPTxt->toPlainText(), "ip", "direct");
    setText(proxyIPTxt->toPlainText(), "ip", "proxy");
    setText(blockIPTxt->toPlainText(), "ip", "block");
    return route;
}
