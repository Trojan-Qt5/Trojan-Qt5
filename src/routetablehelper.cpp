#include "routetablehelper.h"

#include <stdlib.h>
#include <QProcess>
#include <QHostInfo>

RouteTableHelper::RouteTableHelper(QString serverAddress) : serverAddress(serverAddress)
{
    gateWay = getDefaultGateWay();
    ip = "";
    thread = new QThread(this);
    this->moveToThread(thread);
    connect(thread, SIGNAL(started()), this, SLOT(setRouteTable()));
    connect(thread, SIGNAL(finished()), this, SLOT(resetRouteTable()));
}

RouteTableHelper::~RouteTableHelper()
{}

QString RouteTableHelper::getDefaultGateWay()
{
    QProcess *task = new QProcess;
    QStringList param;
#if defined (Q_OS_WIN)
    task->start("cmd /c \"FOR /F \"tokens=13\" %x IN ('\"ipconfig /renew * > nul & ipconfig | findstr \"Default Gateway\" | findstr \"[0-9]*\\.[0-9]*\\.[0-9]*\\.[0-9]*\"\"') DO echo %x\"");
    task->waitForFinished();
    QString gateway = task->readAllStandardOutput();
    gateway = gateway.remove("\n");
    return gateway;
#elif defined (Q_OS_MAC)
    param << "-c" << "route get default | grep gateway | awk '{print $2}'";
    task->start("bash", param);
    task->waitForFinished();
    QString gateway = task->readAllStandardOutput();
    gateway = gateway.remove("\n");
    return gateway;
#elif defined (Q_OS_LINUX)
    param << "-c" << "route -n | awk '{print $2}' | awk 'NR == 3 {print}'";
    task->start("bash", param);
    task->waitForFinished();
    QString gateway = task->readAllStandardOutput();
    gateway = gateway.remove("\n");
    return gateway;
#endif
}

void RouteTableHelper::set()
{
    thread->start();
}

void RouteTableHelper::reset()
{
    thread->exit();
}

void RouteTableHelper::setRouteTable()
{
    // let's process domain & ip address
    QRegExp pattern("^([a-zA-Z0-9-]+.)+([a-zA-Z])+$");
    if (pattern.exactMatch(serverAddress)) {
        QList<QHostAddress> ipAddress = QHostInfo::fromName(serverAddress).addresses();
        if (!ipAddress.isEmpty())
            ip = ipAddress.first().toString();
        else
            return;
    }
    else
        ip = serverAddress;
#if defined (Q_OS_WIN)
    QProcess::execute("route add 0.0.0.0 mask 0.0.0.0 10.0.0.1 metric 6");
    QProcess::execute(QString("route add %1 %2 metric 5").arg(ip).arg(gateWay));
#elif defined (Q_OS_MAC)
    QProcess::execute("route delete default");
    QThread::msleep(200); // wait for tun2socks to be up
    QProcess::execute("route add default 240.0.0.1");
    QProcess::execute(QString("route add default %1 -ifscope en0").arg(gateWay));
    QProcess::execute(QString("route add 10.0.0.0/8 %1").arg(gateWay));
    QProcess::execute(QString("route add 172.16.0.0/12 %1").arg(gateWay));
    QProcess::execute(QString("route add 192.168.0.0/16 %1").arg(gateWay));
    QProcess::execute(QString("route add %1/32 %2").arg(ip).arg(gateWay));
#elif defined (Q_OS_LINUX)
    QProcess::execute("ip route del default");
    QProcess::execute("ip route add default via 240.0.0.1");
    QProcess::execute(QString("ip route add %1/32 via %2").arg(ip).arg(gateWay));
#endif

}

void RouteTableHelper::resetRouteTable()
{
#if defined (Q_OS_MAC)
    QProcess::execute("route delete default");
    QProcess::execute("route delete 10.0.0.0/8");
    QProcess::execute("route delete 172.16.0.0/12");
    QProcess::execute("route delete 192.168.0.0/16");
    QProcess::execute(QString("route delete %1/32 %2").arg(ip).arg(gateWay));
    QProcess::execute(QString("route add default %1").arg(gateWay).toUtf8().data());
#elif defined (Q_OS_LINUX)
    QProcess::execute("ip route delete default");
    QProcess::execute(QString("ip route delete %1/32").arg(ip));
    QProcess::execute(QString("ip route add default via %1").arg(gateWay).toUtf8().data());
#endif
}
