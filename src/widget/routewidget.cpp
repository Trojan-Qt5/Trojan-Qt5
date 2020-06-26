#include "routewidget.h"
#include "ui_routewidget.h"

#include <QJsonArray>
#include <QStringBuilder>
#include <QFile>
#include <QJsonDocument>
#include <QFileDialog>
#include <QDebug>

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

QStringList RouteWidget::jsonArraytoStringlist(const QJsonArray &array)
{
    QStringList list;
    for (const QJsonValue &val: array) {
        QString data = val.toString();
        list.append(data);
    }
    return list;
}

RouterSettings RouteWidget::parseRouterSettings(const QJsonObject &object)
{
    RouterSettings routerSettings;
    routerSettings.domainStrategy = object["domainStrategy"].toString();
    routerSettings.domainDirect = jsonArraytoStringlist(object["domain"].toObject()["direct"].toArray());
    routerSettings.domainProxy = jsonArraytoStringlist(object["domain"].toObject()["proxy"].toArray());
    routerSettings.domainBlock = jsonArraytoStringlist(object["domain"].toObject()["block"].toArray());
    routerSettings.ipDirect = jsonArraytoStringlist(object["ip"].toObject()["direct"].toArray());
    routerSettings.ipProxy = jsonArraytoStringlist(object["ip"].toObject()["proxy"].toArray());
    routerSettings.ipBlock = jsonArraytoStringlist(object["ip"].toObject()["block"].toArray());
    return routerSettings;
}

QJsonObject RouteWidget::exportRouterSettings(const RouterSettings &settings)
{
    QJsonObject object;
    object["domainStrategy"] = settings.domainStrategy;
    QJsonObject domain;
    domain["direct"] = QJsonArray::fromStringList(settings.domainDirect);
    domain["proxy"] = QJsonArray::fromStringList(settings.domainProxy);
    domain["block"] = QJsonArray::fromStringList(settings.domainBlock);
    QJsonObject ip;
    ip["direct"] = QJsonArray::fromStringList(settings.ipDirect);
    ip["proxy"] = QJsonArray::fromStringList(settings.ipProxy);
    ip["block"] = QJsonArray::fromStringList(settings.ipBlock);
    object["domain"] = domain;
    object["ip"] = ip;
    return object;
}

QStringList RouteWidget::setText(QString input)
{
    QStringList datas = input.replace("\r", "").split("\n");
    QStringList result;
    for (auto data : datas)
    {
        if (!data.trimmed().isEmpty())
        {
            result.push_back(data);
         }
    }
    return result;
}

QString RouteWidget::getText(QStringList input)
{
    QString datas;
    foreach(const QString &data, input) {
        datas = datas % data % "\r\n";
    }
    return datas;
}

void RouteWidget::setConfig(RouterSettings r)
{
    route = r;

    ui->domainStrategyCombo->setCurrentText(route.domainStrategy);
    // domain
    directDomainTxt->setPlainText(getText(route.domainDirect));
    proxyDomainTxt->setPlainText(getText(route.domainProxy));
    blockDomainTxt->setPlainText(getText(route.domainBlock));
    // ip
    directIPTxt->setPlainText(getText(route.ipDirect));
    proxyIPTxt->setPlainText(getText(route.ipProxy));
    blockIPTxt->setPlainText(getText(route.ipBlock));
}

RouterSettings RouteWidget::getConfig()
{
    RouterSettings routerSettings;
    routerSettings.domainStrategy = ui->domainStrategyCombo->currentText();
    routerSettings.domainDirect = setText(directDomainTxt->toPlainText());
    routerSettings.domainProxy = setText(proxyDomainTxt->toPlainText());
    routerSettings.domainBlock = setText(blockDomainTxt->toPlainText());
    routerSettings.ipDirect = setText(directIPTxt->toPlainText());
    routerSettings.ipProxy = setText(proxyIPTxt->toPlainText());
    routerSettings.ipBlock = setText(blockIPTxt->toPlainText());
    return routerSettings;
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

    RouterSettings routerSettings = parseRouterSettings(object);

    ui->domainStrategyCombo->setCurrentText(routerSettings.domainStrategy);

    directDomainTxt->setText(getText(routerSettings.domainDirect));
    proxyDomainTxt->setText(getText(routerSettings.domainProxy));
    blockDomainTxt->setText(getText(routerSettings.domainBlock));

    directIPTxt->setText(getText(routerSettings.ipDirect));
    proxyIPTxt->setText(getText(routerSettings.ipProxy));
    blockIPTxt->setText(getText(routerSettings.ipBlock));
}

void RouteWidget::on_exportRulesBtn_clicked()
{
    QString file = QFileDialog::getSaveFileName(
        this,
        tr("Exports Rules to rules.json"),
         QString(),
        "Trojan-Qt5 Route Rules Configuration (rules.json)");

    QFile JSONFile(file);
    JSONFile.open(QIODevice::WriteOnly | QIODevice::Text);

    QJsonObject object;

    RouterSettings routerSettings;

    routerSettings.domainStrategy = ui->domainStrategyCombo->currentText();

    routerSettings.domainDirect = setText(directDomainTxt->toPlainText());
    routerSettings.domainProxy = setText(proxyDomainTxt->toPlainText());
    routerSettings.domainBlock = setText(blockDomainTxt->toPlainText());

    routerSettings.ipDirect = setText(directIPTxt->toPlainText());
    routerSettings.ipProxy = setText(proxyIPTxt->toPlainText());
    routerSettings.ipBlock = setText(blockIPTxt->toPlainText());

    QJsonDocument doc = QJsonDocument(exportRouterSettings(routerSettings));

    JSONFile.write(doc.toJson());
    JSONFile.close();
}
