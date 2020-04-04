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
    run_tun2socks(tunName.toLocal8Bit().data(), tunAddr.toLocal8Bit().data(), tunGw.toLocal8Bit().data(), tunDns.toLocal8Bit().data(), proxyServer.toLocal8Bit().data());
#endif
}

void Tun2socksThread::stop()
{

}
*/
