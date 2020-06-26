#ifndef ROUTEWIDGET_H
#define ROUTEWIDGET_H

#include "configstruct.h"

#include <QWidget>
#include <QTextEdit>
#include <QJsonObject>

namespace Ui {
class RouteWidget;
}

class RouteWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RouteWidget(QWidget *parent = nullptr);
    ~RouteWidget();

    QStringList jsonArraytoStringlist(const QJsonArray &array);

    RouterSettings parseRouterSettings(const QJsonObject &object);
    QJsonObject exportRouterSettings(const RouterSettings &settings);

    QString getText(QStringList input);
    QStringList setText(QString input);

    void setConfig(RouterSettings r);
    RouterSettings getConfig();

private slots:
    void on_importRulesBtn_clicked();

    void on_exportRulesBtn_clicked();

private:
    Ui::RouteWidget *ui;
    RouterSettings route;
    QTextEdit *directDomainTxt;
    QTextEdit *proxyDomainTxt;
    QTextEdit *blockDomainTxt;
    QTextEdit *directIPTxt;
    QTextEdit *blockIPTxt;
    QTextEdit *proxyIPTxt;
};

#endif // ROUTEWIDGET_H
