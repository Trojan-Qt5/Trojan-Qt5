#include "confighelper.h"
#include "tun2socksthread.h"
#include "utils.h"
#include "trojan-qt5-core.h"

#include <QProcess>

Tun2socksThread::Tun2socksThread()
{}

Tun2socksThread::~Tun2socksThread()
{
    stop();
}

void Tun2socksThread::run()
{
    ConfigHelper *conf = Utils::getConfigHelper();
    QString tunName = "tun1";
    QString tunAddr = "240.0.0.2";
    QString tunGw = "240.0.0.1";
    QString tunDns = "8.8.4.4,8.8.8.8";
    QString proxyServer = QString("%1:%2").arg("127.0.0.1").arg(conf->getInboundSettings().socks5LocalPort);

#if defined (Q_OS_WIN)
    tunAddr = "10.0.0.2";
    tunGw = "10.0.0.1";
#elif defined (Q_OS_MAC)
    tunName = "utun1";
#elif defined (Q_OS_LINUX)
    QProcess::execute("ip tuntap add mode tun dev tun1");
    QProcess::execute("ip addr add 240.0.0.1 dev tun1");
    QProcess::execute("ip link set dev tun1 up");
#endif

    run_tun2socks(tunName.toUtf8().data(), tunAddr.toUtf8().data(), tunGw.toUtf8().data(), tunDns.toUtf8().data(), proxyServer.toUtf8().data());
}

void Tun2socksThread::stop()
{
    stop_tun2socks();
}
