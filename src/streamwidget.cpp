#include "streamwidget.h"
#include "ui_streamwidget.h"
#include "utils.h"
#include <QJsonArray>
#include <QStringBuilder>

StreamWidget::StreamWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StreamWidget)
{
    ui->setupUi(this);
}

StreamWidget::~StreamWidget()
{
    delete ui;
}

QJsonObject StreamWidget::getSettings()
{
    return settings;
}

void StreamWidget::setSettings(const QJsonObject &st)
{
    settings = st;

    ui->transportCombo->setCurrentText(settings["network"].toString());
    // tcp settings
    ui->tcpHeaderTypeCB->setCurrentText(settings["tcp"].toObject()["header"].toObject()["type"].toString());
    ui->tcpRequestTxt->setPlainText(settings["tcp"].toObject()["header"].toObject()["request"].toString());
    ui->tcpRespTxt->setPlainText(settings["tcp"].toObject()["header"].toObject()["response"].toString());
    // http settings
    QString httpHosts;
    foreach (const QJsonValue& value, settings["http"].toObject()["host"].toArray())
    {
        QString host = value.toString();
        httpHosts = httpHosts % host % "\r\n";
    }
    ui->httpHostTxt->setPlainText(httpHosts);
    ui->httpPathTxt->setText(settings["http"].toObject()["path"].toString());
    // websocket settings
    QString wsHeaders;
    foreach (const QString& key, settings["ws"].toObject()["header"].toObject().keys())
    {
        QJsonValue value = settings["ws"].toObject()["header"].toObject().value(key);
        wsHeaders = wsHeaders % key % "|" % value.toString() % "\r\n";
    }
    ui->wsHeadersTxt->setPlainText(wsHeaders);
    ui->wsPathTxt->setText(settings["ws"].toObject()["path"].toString());
    // kcp settings
    ui->kcpMTU->setValue(settings["kcp"].toObject()["mtu"].toInt());
    ui->kcpTTI->setValue(settings["kcp"].toObject()["tti"].toInt());
    ui->kcpUploadCapacSB->setValue(settings["kcp"].toObject()["uplinkCapacity"].toInt());
    ui->kcpCongestionCB->setChecked(settings["kcp"].toObject()["congestion"].toBool());
    ui->kcpDownCapacitySB->setValue(settings["kcp"].toObject()["downlinkCapacity"].toInt());
    ui->kcpReadBufferSB->setValue(settings["kcp"].toObject()["readBufferSize"].toInt());
    ui->kcpWriteBufferSB->setValue(settings["kcp"].toObject()["writeBufferSize"].toInt());
    ui->kcpHeaderType->setCurrentText(settings["kcp"].toObject()["header"].toObject()["type"].toString());
    // quic settings
    ui->quicSecurityCB->setCurrentText(settings["quic"].toObject()["security"].toString());
    ui->quicKeyTxt->setText(settings["quic"].toObject()["key"].toString());
    ui->quicHeaderTypeCB->setCurrentText(settings["quic"].toObject()["header"].toObject()["type"].toString());
    // tls settings
    ui->tlsCB->setChecked(settings["tls"].toObject()["enable"].toBool());
    ui->allowInsecureCB->setChecked(settings["tls"].toObject()["allowInsecure"].toBool());
    ui->allowInsecureCiphersCB->setChecked(settings["tls"].toObject()["allowInsecureCiphers"].toBool());
    ui->serverNameTxt->setText(settings["tls"].toObject()["serverName"].toString());
}

void StreamWidget::on_transportCombo_currentIndexChanged(int index)
{
    ui->v2rayStackView->setCurrentIndex(index);
}

void StreamWidget::on_transportCombo_currentIndexChanged(const QString &arg1)
{
    settings["network"] = arg1;
}

void StreamWidget::on_tcpHeaderTypeCB_currentIndexChanged(const QString &arg1)
{
    QJsonObject tcp = settings["tcp"].toObject();
    QJsonObject tcpHeader = tcp["header"].toObject();
    tcpHeader["type"] = arg1;
    tcp["header"] = tcpHeader;
    settings["tcp"] = tcp;
}

void StreamWidget::on_tcpRequestTxt_textChanged()
{
    QJsonObject tcp = settings["tcp"].toObject();
    QJsonObject tcpHeader = tcp["header"].toObject();
    tcpHeader["request"] = ui->tcpRequestTxt->toPlainText();
    tcp["header"] = tcpHeader;
    settings["tcp"] = tcp;
}

void StreamWidget::on_tcpRespTxt_textChanged()
{
    QJsonObject tcp = settings["tcp"].toObject();
    QJsonObject tcpHeader = tcp["header"].toObject();
    tcpHeader["response"] = ui->tcpRespTxt->toPlainText();
    tcp["header"] = tcpHeader;
    settings["tcp"] = tcp;
}

void StreamWidget::on_httpPathTxt_textEdited(const QString &arg1)
{
    QJsonObject http = settings["http"].toObject();
    http["path"] = arg1;
    settings["http"] = http;
}

void StreamWidget::on_httpHostTxt_textChanged()
{
    QJsonObject http = settings["http"].toObject();
    QStringList hosts = ui->httpHostTxt->toPlainText().replace("\r", "").split("\n");
    http["host"] = "";
    QJsonArray httpHost;
    for (auto host : hosts)
    {
        if (!host.trimmed().isEmpty())
        {
            httpHost.push_back(host);
         }
    }
    http["host"] = httpHost;
    settings["http"] = http;
}

void StreamWidget::on_wsHeadersTxt_textChanged()
{
    QJsonObject ws = settings["ws"].toObject();
    QJsonObject wsHeader = QJsonObject();
    QStringList headers = Utils::splitLines(ui->wsHeadersTxt->toPlainText());

    for (auto header : headers)
    {
        if (header.isEmpty())
            continue;

        auto index = header.indexOf("|");

        auto key = header.left(index);
        auto value = header.right(header.length() - index - 1);
        wsHeader[key] = value;
    }
    ws["header"] = wsHeader;
    settings["ws"] = ws;
}

void StreamWidget::on_wsPathTxt_textEdited(const QString &arg1)
{
    QJsonObject ws = settings["ws"].toObject();
    ws["path"] = arg1;
    settings["ws"] = ws;
}

void StreamWidget::on_kcpMTU_valueChanged(int arg1)
{
    QJsonObject kcp = settings["kcp"].toObject();
    kcp["mtu"] = arg1;
    settings["kcp"] = kcp;
}

void StreamWidget::on_kcpTTI_valueChanged(int arg1)
{
    QJsonObject kcp = settings["kcp"].toObject();
    kcp["tti"] = arg1;
    settings["kcp"] = kcp;
}

void StreamWidget::on_kcpUploadCapacSB_valueChanged(int arg1)
{
    QJsonObject kcp = settings["kcp"].toObject();
    kcp["uplinkCapacity"] = arg1;
    settings["kcp"] = kcp;
}

void StreamWidget::on_kcpCongestionCB_stateChanged(int arg1)
{
    QJsonObject kcp = settings["kcp"].toObject();
    kcp["congestion"] = arg1 == Qt::Checked;
    settings["kcp"] = kcp;
}

void StreamWidget::on_kcpDownCapacitySB_valueChanged(int arg1)
{
    QJsonObject kcp = settings["kcp"].toObject();
    kcp["downlinkCapacity"] = arg1;
    settings["kcp"] = kcp;
}

void StreamWidget::on_kcpReadBufferSB_valueChanged(int arg1)
{
    QJsonObject kcp = settings["kcp"].toObject();
    kcp["readBufferSize"] = arg1;
    settings["kcp"] = kcp;
}

void StreamWidget::on_kcpWriteBufferSB_valueChanged(int arg1)
{
    QJsonObject kcp = settings["kcp"].toObject();
    kcp["writeBufferSize"] = arg1;
    settings["kcp"] = kcp;
}


void StreamWidget::on_kcpHeaderType_currentTextChanged(const QString &arg1)
{
    QJsonObject kcp = settings["kcp"].toObject();
    QJsonObject kcpHeader = kcp["header"].toObject();
    kcpHeader["type"] = arg1;
    kcp["header"] = kcpHeader;
    settings["kcp"] = kcp;
}


void StreamWidget::on_quicSecurityCB_currentTextChanged(const QString &arg1)
{
    QJsonObject quic = settings["quic"].toObject();
    quic["security"] = arg1;
    settings["quic"] = quic;
}

void StreamWidget::on_quicKeyTxt_textEdited(const QString &arg1)
{
    QJsonObject quic = settings["quic"].toObject();
    quic["key"] = arg1;
    settings["quic"] = quic;
}

void StreamWidget::on_quicHeaderTypeCB_currentIndexChanged(const QString &arg1)
{
    QJsonObject quic = settings["quic"].toObject();
    QJsonObject quicHeader = quic["header"].toObject();
    quicHeader["type"] = arg1;
    quic["header"] = quicHeader;
    settings["quic"] = quic;
}

void StreamWidget::on_serverNameTxt_textEdited(const QString &arg1)
{
    QJsonObject tls = settings["tls"].toObject();
    tls["serverName"] = arg1;
    settings["tls"] = tls;
}

void StreamWidget::on_tlsCB_stateChanged(int arg1)
{
    QJsonObject tls = settings["tls"].toObject();
    tls["enable"] = arg1 == Qt::Checked;
    settings["tls"] = tls;
}

void StreamWidget::on_allowInsecureCB_stateChanged(int arg1)
{
    QJsonObject tls = settings["tls"].toObject();
    tls["allowInsecure"] = arg1 == Qt::Checked;
    settings["tls"] = tls;
}

void StreamWidget::on_allowInsecureCiphersCB_stateChanged(int arg1)
{
    QJsonObject tls = settings["tls"].toObject();
    tls["allowInsecureCiphers"] = arg1 == Qt::Checked;
    settings["tls"] = tls;
}
