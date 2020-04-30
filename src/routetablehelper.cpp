#include "routetablehelper.h"

#include <stdlib.h>
#include <QProcess>
#include <QHostInfo>

RouteTableHelper::RouteTableHelper(QString serverAddress) : serverAddress(serverAddress)
{
    gateWay = getDefaultGateWay();
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
#if defined (Q_OS_MAC) || defined (Q_OS_LINUX)
    param << "-c" << "route get default | grep gateway | awk '{print $2}'";
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
#if defined (Q_OS_WIM)
    QProcess::execute("route add 0.0.0.0 mask 0.0.0.0 10.0.0.1 metric 6");
    // let's process domain & ip address
    QRegExp pattern("^([a-zA-Z0-9-]+.)+([a-zA-Z])+$");
    QString ip = "";
    if (pattern.exactMatch(serverAddress))
        ip = QHostInfo::fromName(serverAddress).addresses().first().toString();
    else
        ip = serverAddress;
    QProcess::execute(QString("route add %1 %2 metric 5").arg(ip).arg(gateWay));
#elif defined (Q_OS_MAC)
    QProcess::execute("route delete default");
    QThread::msleep(200); // wait for tun2socks to be up
    QProcess::execute("route add default 240.0.0.1");
    QProcess::execute(QString("route add default %1 -ifscope en0").arg(gateWay));
    QProcess::execute(QString("route add 10.0.0.0/8 %1").arg(gateWay));
    QProcess::execute(QString("route add 172.16.0.0/12 %1").arg(gateWay));
    QProcess::execute(QString("route add 192.168.0.0/16 %1").arg(gateWay));
    // let's process domain & ip address
    QRegExp pattern("^([a-zA-Z0-9-]+.)+([a-zA-Z])+$");
    QString ip = "";
    if (pattern.exactMatch(serverAddress))
        ip = QHostInfo::fromName(serverAddress).addresses().first().toString();
    else
        ip = serverAddress;
    QProcess::execute(QString("route add %1/32 %2").arg(ip).arg(gateWay));
#elif defined (Q_OS_LINUX)
    QProcess::execute("ip tuntap add mode tun dev tun1");
    QProcess::execute("ip addr add 240.0.0.1 dev tun1");
    QProcess::execute("ip link set dev tun1 up");
    QProcess::execute("ip route del default");
    QProcess::execute(QString("ip route add default via %1").arg(gateWay));
    // let's process domain & ip address
    QRegExp pattern("^([a-zA-Z0-9-]+.)+([a-zA-Z])+$");
    QString ip = "";
    if (pattern.exactMatch(serverAddress))
        ip = QHostInfo::fromName(serverAddress).addresses().first().toString();
    else
        ip = serverAddress;
    QProcess::execute(QString("ip route add %1/32 via %2").arg(ip).arg(gateWay));
#endif

}

void RouteTableHelper::resetRouteTable()
{
    for (int i = 0; i < 8; i++)
        QProcess::execute("route -n flush");
    QProcess::execute(QString("route add default %1").arg(gateWay).toUtf8().data());
}
