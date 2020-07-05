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

VmessSettings StreamWidget::getSettings()
{
    return settings;
}

void StreamWidget::setSettings(const VmessSettings &st)
{
    settings = st;

    ui->transportCombo->setCurrentText(st.network);
    // tcp settings
    ui->tcpHeaderTypeCB->setCurrentText(st.tcp.type);
    ui->tcpRequestTxt->setPlainText(st.tcp.request);
    ui->tcpRespTxt->setPlainText(st.tcp.response);
    // http settings
    QString httpHosts;
    foreach (const QString& host, st.http.host)
    {
        httpHosts = httpHosts % host % "\r\n";
    }
    ui->httpHostTxt->setPlainText(httpHosts);
    ui->httpPathTxt->setText(st.http.path);
    // websocket settings
    QString wsHeaders;
    foreach (const WsHeader& header, st.ws.header)
    {
        wsHeaders = wsHeaders % header.key % "|" % header.value % "\r\n";
    }
    ui->wsHeadersTxt->setPlainText(wsHeaders);
    ui->wsPathTxt->setText(st.ws.path);
    // kcp settings
    ui->kcpMTU->setValue(st.kcp.mtu);
    ui->kcpTTI->setValue(st.kcp.tti);
    ui->kcpUploadCapacSB->setValue(st.kcp.uplinkCapacity);
    ui->kcpCongestionCB->setChecked(st.kcp.congestion);
    ui->kcpDownCapacitySB->setValue(st.kcp.downlinkCapacity);
    ui->kcpReadBufferSB->setValue(st.kcp.readBufferSize);
    ui->kcpWriteBufferSB->setValue(st.kcp.writeBufferSize);
    ui->kcpHeaderType->setCurrentText(st.kcp.type);
    // quic settings
    ui->quicSecurityCB->setCurrentText(st.quic.security);
    ui->quicKeyTxt->setText(st.quic.key);
    ui->quicHeaderTypeCB->setCurrentText(st.quic.type);
    // tls settings
    ui->tlsCB->setChecked(st.tls.enable);
    ui->allowInsecureCB->setChecked(st.tls.allowInsecure);
    ui->allowInsecureCiphersCB->setChecked(st.tls.allowInsecureCiphers);
    ui->serverNameTxt->setText(st.tls.serverName);
    QString alpns;
    foreach (const QString& alpn, st.tls.alpn)
    {
        alpns = alpns % alpn % "\r\n";
    }
    ui->alpnTxt->setPlainText(alpns);
    // mux settings
    ui->muxCB->setChecked(st.mux.enable);
    ui->muxConcurrencySB->setValue(st.mux.muxConcurrency);
}

void StreamWidget::on_transportCombo_currentIndexChanged(int index)
{
    ui->v2rayStackView->setCurrentIndex(index);
}

void StreamWidget::on_transportCombo_currentIndexChanged(const QString &arg1)
{
    settings.network = arg1;
}

void StreamWidget::on_tcpHeaderTypeCB_currentIndexChanged(const QString &arg1)
{
    settings.tcp.type = arg1;
}

void StreamWidget::on_tcpRequestTxt_textChanged()
{
    settings.tcp.request = ui->tcpRequestTxt->toPlainText();
}

void StreamWidget::on_tcpRespTxt_textChanged()
{
    settings.tcp.response = ui->tcpRespTxt->toPlainText();
}

void StreamWidget::on_httpHostTxt_textChanged()
{
    QStringList hosts = ui->httpHostTxt->toPlainText().replace("\r", "").split("\n");
    QStringList httpHost;
    for (auto host : hosts)
    {
        if (!host.trimmed().isEmpty())
        {
            httpHost.push_back(host);
         }
    }
    settings.http.host = httpHost;
}

void StreamWidget::on_httpPathTxt_textEdited(const QString &arg1)
{
    settings.http.path = arg1;
}

void StreamWidget::on_wsHeadersTxt_textChanged()
{
    QList<WsHeader> WsHeaders;
    QStringList headers = Utils::splitLines(ui->wsHeadersTxt->toPlainText());

    for (auto header : headers)
    {
        if (header.isEmpty())
            continue;

        auto index = header.indexOf("|");

        QString key = header.left(index);
        QString value = header.right(header.length() - index - 1);
        WsHeader wsHeader;
        wsHeader.key = key;
        wsHeader.value = value;
        WsHeaders.append(wsHeader);
    }
    settings.ws.header = WsHeaders;
}

void StreamWidget::on_wsPathTxt_textEdited(const QString &arg1)
{
    settings.ws.path = arg1;
}

void StreamWidget::on_kcpMTU_valueChanged(int arg1)
{
    settings.kcp.mtu = arg1;
}

void StreamWidget::on_kcpTTI_valueChanged(int arg1)
{
    settings.kcp.tti = arg1;
}

void StreamWidget::on_kcpUploadCapacSB_valueChanged(int arg1)
{
    settings.kcp.uplinkCapacity = arg1;
}

void StreamWidget::on_kcpCongestionCB_stateChanged(int arg1)
{
    settings.kcp.congestion = arg1;
}

void StreamWidget::on_kcpDownCapacitySB_valueChanged(int arg1)
{
    settings.kcp.downlinkCapacity = arg1;
}

void StreamWidget::on_kcpReadBufferSB_valueChanged(int arg1)
{
    settings.kcp.readBufferSize = arg1;
}

void StreamWidget::on_kcpWriteBufferSB_valueChanged(int arg1)
{
    settings.kcp.writeBufferSize = arg1;
}

void StreamWidget::on_kcpHeaderType_currentTextChanged(const QString &arg1)
{
    settings.kcp.type = arg1;
}

void StreamWidget::on_seedTxt_textEdited(const QString &arg1)
{
    settings.kcp.seed = arg1;
}

void StreamWidget::on_quicSecurityCB_currentTextChanged(const QString &arg1)
{
    settings.quic.security = arg1;
}

void StreamWidget::on_quicKeyTxt_textEdited(const QString &arg1)
{
    settings.quic.key = arg1;
}

void StreamWidget::on_quicHeaderTypeCB_currentIndexChanged(const QString &arg1)
{
    settings.quic.type = arg1;
}

void StreamWidget::on_tlsCB_stateChanged(int arg1)
{
    settings.tls.enable = arg1 == Qt::Checked;
}

void StreamWidget::on_allowInsecureCB_stateChanged(int arg1)
{
    settings.tls.allowInsecure = arg1 == Qt::Checked;
}

void StreamWidget::on_allowInsecureCiphersCB_stateChanged(int arg1)
{
    settings.tls.allowInsecureCiphers = arg1 == Qt::Checked;
}

void StreamWidget::on_serverNameTxt_textEdited(const QString &arg1)
{
    settings.tls.serverName = arg1;
}

void StreamWidget::on_alpnTxt_textChanged()
{
    QStringList alpnList = Utils::splitLines(ui->alpnTxt->toPlainText());
    QStringList alpnArray;
    for (auto alpn : alpnList)
    {
        if (!alpn.trimmed().isEmpty())
        {
            alpnArray.push_back(alpn);
         }
    }
    settings.tls.alpn = alpnArray;
}

void StreamWidget::on_muxCB_stateChanged(int arg1)
{
    settings.mux.enable = arg1 == Qt::Checked;
}

void StreamWidget::on_muxConcurrencySB_valueChanged(int arg1)
{
    settings.mux.muxConcurrency = arg1;
}
