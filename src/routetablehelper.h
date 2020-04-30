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

    void set();
    void reset();

public slots:
    void setRouteTable();
    void resetRouteTable();

private:
    QString serverAddress;
    QString gateWay;
    QThread *thread;
};

#endif // ROUTETABLEHELPER_H
