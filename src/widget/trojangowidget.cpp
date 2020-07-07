#include "trojangowidget.h"
#include "ui_trojangowidget.h"
#include "utils.h"

#include <QStringBuilder>

TrojanGoWidget::TrojanGoWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TrojanGoWidget)
{
    ui->setupUi(this);
}

TrojanGoWidget::~TrojanGoWidget()
{
    delete ui;
}

TrojanGoSettings TrojanGoWidget::getSettings()
{
    return settings;
}

void TrojanGoWidget::setSettings(const TrojanGoSettings &st)
{
    settings = st;
    // mux settings
    ui->muxCB->setChecked(st.mux.enable);
    ui->muxConcurrencySB->setValue(st.mux.muxConcurrency);
    ui->muxIdleTimeoutSB->setValue(st.mux.muxIdleTimeout);
    // websocket settings
    ui->websocketCB->setChecked(st.websocket.enable);
    ui->websocketPathEdit->setText(st.websocket.path);
    ui->websocketHostnameEdit->setText(st.websocket.hostname);
    // shadowsocks settings
    ui->shadowsocksCB->setChecked(st.shadowsocks.enable);
    ui->methodCB->setCurrentText(st.shadowsocks.method);
    ui->passwordEdit->setText(st.shadowsocks.password);
    // transportPlugin settings
    ui->transportPluginCB->setChecked(st.transportPlugin.enable);
    ui->typeCB->setCurrentText(st.transportPlugin.type);
    ui->commandEdit->setText(st.transportPlugin.command);
    QString args;
    foreach (const QString& arg, st.transportPlugin.arg)
    {
        args = args % arg % "\r\n";
    }
    ui->argPTEdit->setPlainText(args);
    QString envs;
    foreach (const QString& env, st.transportPlugin.env)
    {
        envs = envs % env % "\r\n";
    }
    ui->envPTEdit->setPlainText(envs);
    ui->optionEdit->setText(st.transportPlugin.option);
}

void TrojanGoWidget::on_muxCB_stateChanged(int arg1)
{
    settings.mux.enable = arg1 == Qt::Checked;
}

void TrojanGoWidget::on_muxConcurrencySB_valueChanged(int arg1)
{
    settings.mux.muxConcurrency = arg1;
}

void TrojanGoWidget::on_muxIdleTimeoutSB_valueChanged(int arg1)
{
    settings.mux.muxIdleTimeout = arg1;
}

void TrojanGoWidget::on_websocketCB_stateChanged(int arg1)
{
    settings.websocket.enable = arg1 == Qt::Checked;
}

void TrojanGoWidget::on_websocketPathEdit_textEdited(const QString &arg1)
{
    settings.websocket.path = arg1;
}

void TrojanGoWidget::on_websocketHostnameEdit_textEdited(const QString &arg1)
{
    settings.websocket.hostname = arg1;
}

void TrojanGoWidget::on_shadowsocksCB_stateChanged(int arg1)
{
    settings.shadowsocks.enable = arg1;
}

void TrojanGoWidget::on_methodCB_currentIndexChanged(const QString &arg1)
{
    settings.shadowsocks.method = arg1;
}

void TrojanGoWidget::on_passwordEdit_textEdited(const QString &arg1)
{
    settings.shadowsocks.password = arg1;
}

void TrojanGoWidget::on_transportPluginCB_stateChanged(int arg1)
{
    settings.transportPlugin.enable = arg1 == Qt::Checked;
}

void TrojanGoWidget::on_typeCB_currentIndexChanged(const QString &arg1)
{
    settings.transportPlugin.type = arg1;
}

void TrojanGoWidget::on_commandEdit_textEdited(const QString &arg1)
{
    settings.transportPlugin.command = arg1;
}

void TrojanGoWidget::on_argPTEdit_textChanged()
{
    QStringList argList = Utils::splitLines(ui->argPTEdit->toPlainText());
    QStringList argArray;
    for (auto arg : argList)
    {
        if (!arg.trimmed().isEmpty())
        {
            argArray.push_back(arg);
         }
    }
    settings.transportPlugin.arg = argArray;
}

void TrojanGoWidget::on_envPTEdit_textChanged()
{
    QStringList envList = Utils::splitLines(ui->envPTEdit->toPlainText());
    QStringList envArray;
    for (auto arg : envList)
    {
        if (!arg.trimmed().isEmpty())
        {
            envArray.push_back(arg);
         }
    }
    settings.transportPlugin.env = envArray;
}

void TrojanGoWidget::on_optionEdit_textEdited(const QString &arg1)
{
    settings.transportPlugin.option = arg1;
}
