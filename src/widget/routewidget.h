#ifndef ROUTEWIDGET_H
#define ROUTEWIDGET_H

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
    QString getText(QJsonObject object, QString level1, QString level2);
    QJsonObject setText(QJsonObject object, QString input, QString level1, QString level2);
    void setConfig(QJsonObject r);
    QJsonObject getConfig();

private slots:
    void on_importRulesBtn_clicked();

    void on_exportRulesBtn_clicked();

private:
    Ui::RouteWidget *ui;
    QJsonObject route;
    QTextEdit *directDomainTxt;
    QTextEdit *proxyDomainTxt;
    QTextEdit *blockDomainTxt;
    QTextEdit *directIPTxt;
    QTextEdit *blockIPTxt;
    QTextEdit *proxyIPTxt;
};

#endif // ROUTEWIDGET_H
