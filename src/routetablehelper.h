#ifndef ROUTETABLEHELPER_H
#define ROUTETABLEHELPER_H

#include <QObject>
#include <QThread>

class RouteTableHelper : public QObject
{
    Q_OBJECT

public:
    RouteTableHelper(QString serverAddress);
    ~RouteTableHelper();

    QString getDefaultGateWay();

    void resetRouteTable();

public slots:
    void setRouteTable();

private:
    QString serverAddress;
    QString gateWay;
    QThread *thread;
};

#endif // ROUTETABLEHELPER_H
