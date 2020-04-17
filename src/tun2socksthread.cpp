#include "confighelper.h"
#include "tun2socksthread.h"

/*
#include "3rd/tun2socks/tun2socks.h"

Tun2socksThread::Tun2socksThread()
{

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
    QString proxyServer = QString("%1:%2").arg(conf->getSocks5Address()).arg(conf->getSocks5Port());

#if defined(Q_OS_MAC)
    tunName = "utun1";
    run_tun2socks(tunName.toUtf8().data(), tunAddr.toUtf8().data(), tunGw.toUtf8().data(), tunDns.toUtf8().data(), proxyServer.toUtf8().data());
#endif
}

void Tun2socksThread::stop()
{

}
*/
