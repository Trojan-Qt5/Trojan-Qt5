#include "confighelper.h"
#include "tun2socksthread.h"
#include <QProcess>

#include "3rd/trojan-qt5-libs/trojan-qt5-libs.h"

Tun2socksThread::Tun2socksThread()
{}

Tun2socksThread::~Tun2socksThread()
{
    stop();
}

void Tun2socksThread::run()
{
#ifdef Q_OS_WIN
    QString configFile = QCoreApplication::applicationDirPath() + "/config.ini";
#else
    QString configFile = QDir::homePath() + "/.config/trojan-qt5/config.ini";
#endif
    ConfigHelper *conf = new ConfigHelper(configFile);
    QString tunName = "tun1";
    QString tunAddr = "240.0.0.2";
    QString tunGw = "240.0.0.1";
    QString tunDns = "8.8.4.4,8.8.8.8";
    QString proxyServer = QString("%1:%2").arg("127.0.0.1").arg(conf->getSocks5Port());

#if defined (Q_OS_WIN)
    tunAddr = "10.0.0.2";
    tunGw = "10.0.0.1";
#elif defined (Q_OS_MAC)
    tunName = "utun1";
#endif

    run_tun2socks(tunName.toUtf8().data(), tunAddr.toUtf8().data(), tunGw.toUtf8().data(), tunDns.toUtf8().data(), proxyServer.toUtf8().data());
}

void Tun2socksThread::stop()
{
    stop_tun2socks();
}
