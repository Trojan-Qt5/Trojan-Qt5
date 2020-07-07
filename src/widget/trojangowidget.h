#ifndef TROJANGOWIDGET_H
#define TROJANGOWIDGET_H

#include <QWidget>
#include "trojangostruct.h"

namespace Ui {
class TrojanGoWidget;
}

class TrojanGoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TrojanGoWidget(QWidget *parent = nullptr);
    ~TrojanGoWidget();

    TrojanGoSettings getSettings();
    void setSettings(const TrojanGoSettings &st);

private slots:
    void on_muxCB_stateChanged(int arg1);

    void on_muxConcurrencySB_valueChanged(int arg1);

    void on_muxIdleTimeoutSB_valueChanged(int arg1);

    void on_websocketCB_stateChanged(int arg1);

    void on_websocketPathEdit_textEdited(const QString &arg1);

    void on_websocketHostnameEdit_textEdited(const QString &arg1);

    void on_shadowsocksCB_stateChanged(int arg1);

    void on_methodCB_currentIndexChanged(const QString &arg1);

    void on_passwordEdit_textEdited(const QString &arg1);

    void on_transportPluginCB_stateChanged(int arg1);

    void on_typeCB_currentIndexChanged(const QString &arg1);

    void on_commandEdit_textEdited(const QString &arg1);

    void on_argPTEdit_textChanged();

    void on_envPTEdit_textChanged();

    void on_optionEdit_textEdited(const QString &arg1);

private:
    Ui::TrojanGoWidget *ui;
    TrojanGoSettings settings;
};

#endif // TROJANGOWIDGET_H
