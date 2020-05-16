#include "addresstester.h"
#include "connection.h"
#include "confighelper.h"
#include "pachelper.h"
#include "portvalidator.h"
#include "privilegeshelper.h"
#include "ssgoapi.h"
#include "trojangoapi.h"
#include "3rd/trojan-qt5-core/trojan-qt5-core.h"
#include "SSRThread.hpp"
#include <QCoreApplication>
#include <QDir>
#include <QHostInfo>
#include <QHostAddress>

#include "logger.h"
#include "midman.h"

Connection::Connection(QObject *parent) :
    QObject(parent),
    running(false)
{
#ifdef Q_OS_WIN
    configFile = QCoreApplication::applicationDirPath() + "/config.ini";
#else
    QDir configDir = QDir::homePath() + "/.config/trojan-qt5";
    configFile = configDir.absolutePath() + "/config.ini";
#endif
}

Connection::Connection(const TQProfile &_profile, QObject *parent) :
    Connection(parent)
{
    profile = _profile;
}

Connection::Connection(QString uri, QObject *parent) :
    Connection(parent)
{
    profile = TQProfile(uri);
}

Connection::~Connection()
{
    stop();
}

const TQProfile& Connection::getProfile() const
{
    return profile;
}

const QString& Connection::getName() const
{
    return profile.name;
}

QByteArray Connection::getURI(QString type) const
{
    QString uri = "";
    if (type == "ss")
        uri = profile.toSSUri();
    else if (type == "ssr")
        uri = profile.toSSRUri();
    else if (type == "vmess")
        uri = profile.toVmessUri();
    else if (type == "trojan")
        uri = profile.toTrojanUri();
    else if (type == "snell")
        uri = profile.toSnellUri();
    return QByteArray(uri.toUtf8());
}

void Connection::setProfile(TQProfile p)
{
    profile = p;
}

bool Connection::isValid() const
{
    if (profile.serverAddress.isEmpty() || (profile.password.isEmpty() && profile.uuid.isEmpty())) {
        return false;
    }
    else {
        return true;
    }
}

const bool &Connection::isRunning() const
{
    return running;
}

void Connection::latencyTest()
{
    QHostAddress serverAddr(profile.serverAddress);
    if (serverAddr.isNull()) {
        QHostInfo::lookupHost(profile.serverAddress, this, SLOT(onServerAddressLookedUp(QHostInfo)));
    } else {
        testAddressLatency(serverAddr);
    }
}

void Connection::start()
{
    profile.lastTime = QDateTime::currentDateTime();
    //perform a latency test if the latency is unknown
    if (profile.latency == TQProfile::LATENCY_UNKNOWN) {
        latencyTest();
    }

    ConfigHelper *conf = new ConfigHelper(configFile);

    //wait, let's check if port is in use
    if (conf->isCheckPortAvailability()) {
        PortValidator *pv = new PortValidator();
        QString errorString = pv->isInUse(conf->getSocks5Port());
        if (!errorString.isEmpty()) {
            Logger::error(QString("[Connection] Can't bind socks5 port %1: %2").arg(QString::number(conf->getSocks5Port())).arg(errorString));
            return;
        }

        //don't check http mode if httpMode is not enabled
        if (conf->isEnableHttpMode()) {
            QString errorString = pv->isInUse(conf->getHttpPort());
            if (!errorString.isEmpty()) {
                Logger::error(QString("[Connection] Can't bind http port %1: %2").arg(QString::number(conf->getHttpPort())).arg(errorString));
                return;
            }
        }
    }

    if (conf->isEnableHttpMode())
        http = new HttpProxy();

    if (conf->getSystemProxySettings() == "advance") {
        //initialize tun2socks and route table helper
        tun2socks = new Tun2socksThread();
        rhelper = new RouteTableHelper(profile.serverAddress);
    }

#ifdef Q_OS_WIN
    QString file = QCoreApplication::applicationDirPath() + "/config.json";
#else
    QDir configDir = QDir::homePath() + "/.config/trojan-qt5";
    QString file = configDir.absolutePath() + "/config.json";
#endif

    QString localAddr = conf->isEnableIpv6Support() ? (conf->isShareOverLan() ? "::" : "::1") : (conf->isShareOverLan() ? "0.0.0.0" : "127.0.0.1");

    if (profile.type == "ss") {
        QString apiAddr = localAddr + ":" + QString::number(conf->getTrojanAPIPort());
        QString clientAddr = localAddr + ":" + QString::number(conf->getSocks5Port());
        QString serverAddr = profile.serverAddress + ":" + QString::number(profile.serverPort);
        ss = new SSThread(clientAddr.toUtf8().data(),
                          serverAddr.toUtf8().data(),
                          profile.method.toUtf8().data(),
                          profile.password.toUtf8().data(),
                          profile.plugin.toUtf8().data(),
                          profile.pluginParam.toUtf8().data(),
                          conf->isEnableTrojanAPI(),
                          apiAddr.toUtf8().data());
    } else if (profile.type == "ssr") {
        //initalize ssr thread
        ssr = std::make_unique<SSRThread>(conf->getSocks5Port(),
                          profile.serverPort,
                          6000,
                          1500,
                          SSRThread::SSR_WORK_MODE::TCP_AND_UDP,
                          conf->isEnableIpv6Support() ? (conf->isShareOverLan() ? "::" : "::1") : (conf->isShareOverLan() ? "0.0.0.0" : "127.0.0.1"),
                          profile.serverAddress.toStdString(),
                          profile.method.toStdString(),
                          profile.password.toStdString(),
                          profile.obfs.toStdString(),
                          profile.obfsParam.toStdString(),
                          profile.protocol.toStdString(),
                          profile.protocolParam.toStdString());
        ssr->connect(ssr.get(), &SSRThread::OnDataReady, this, &Connection::onNewBytesTransmitted);
        ssr->connect(ssr.get(), &SSRThread::onSSRThreadLog, this, &Connection::onLog);
    } else if (profile.type == "vmess") {
        conf->generateV2rayJson(profile);
        v2ray = new V2rayThread(file);
    } else if (profile.type == "trojan") {
        //generate Config File that trojan will use
        conf->connectionToJson(profile);
        trojan = new TrojanThread(file);
    }

    //set running status to true before we start proxy
    running = true;

    if (profile.type == "ss") {
        ss->start();
        if (conf->isEnableTrojanAPI()) {
            ssGoAPI = new SSGoAPI();
            ssGoAPI->start();
            connect(ssGoAPI, &SSGoAPI::OnDataReady, this, &Connection::onNewBytesTransmitted);
        }
    } else if (profile.type == "ssr") {
        ssr->start();
    } else if (profile.type == "vmess") {
       v2ray->start();
    } else if (profile.type == "trojan") {
        trojan->start();
        if (conf->isEnableTrojanAPI()) {
            trojanGoAPI = new TrojanGoAPI();
            trojanGoAPI->setPassword(profile.password);
            trojanGoAPI->start();
            connect(trojanGoAPI, &TrojanGoAPI::OnDataReady, this, &Connection::onNewBytesTransmitted);
        }
    }

    // hack so we can get the connection
    MidMan::setConnection(this);

    //start http proxy if settings is configured to do so
    if (conf->isEnableHttpMode())
        if (conf->getSystemProxySettings() != "advance")
            http->httpListen(QHostAddress(localAddr),
                           conf->getHttpPort(),
                           localAddr,
                           conf->getSocks5Port());

    //start tun2socks if settings is configured to do so
    if (conf->getSystemProxySettings() == "advance") {
        if (PrivilegesHelper::checkPrivileges()) {
            tun2socks->start();
            rhelper->set();
           }
        else {
            PrivilegesHelper::showWarning();
            onStartFailed();
            return;
        }
    }

    conf->setTrojanOn(running);

    emit stateChanged(running);

    // notify the status Connection
    connect(this, &Connection::connectionChanged, MidMan::getStatusConnection(), &Connection::onNotifyConnectionChanged);
    emit connectionChanged();

    //set proxy settings after emit the signal
    if (conf->getSystemProxySettings() == "pac")
        SystemProxyHelper::setSystemProxy(2);
    else if (conf->getSystemProxySettings() == "global")
        SystemProxyHelper::setSystemProxy(1);
}

void Connection::stop()
{
    ConfigHelper *conf = new ConfigHelper(configFile);

    if (running) {
        //set the running status to false first.
        running = false;

        if (profile.type == "ss") {
            ss->stop();
            if (conf->isEnableTrojanAPI()) {
                ssGoAPI->stop();
            }
        } else if (profile.type == "ssr") {
            ssr->stop();
        } else if (profile.type == "vmess") {
            v2ray->stop();
        } else if (profile.type == "trojan") {
            trojan->stop();
            if (conf->isEnableTrojanAPI()) {
                trojanGoAPI->stop();
            }
        }

        //if we have started http proxy, stop it
        if (conf->isEnableHttpMode()) {
            http->close();
        }

        conf->setTrojanOn(running);

        emit stateChanged(running);

        if (conf->getSystemProxySettings() == "advance") {
            tun2socks->stop();
            rhelper->reset();
            rhelper = nullptr;
        } else if (conf->getSystemProxySettings() != "direct") {
            //set proxy settings after emit the signal
            SystemProxyHelper::setSystemProxy(0);
        }
    }
}

void Connection::onStartFailed()
{
    ConfigHelper *conf = new ConfigHelper(configFile);

    running = false;

    conf->setTrojanOn(running);

    if (profile.type == "ss") {
        ss->stop();
        if (conf->isEnableTrojanAPI()) {
            ssGoAPI->stop();
        }
    } else if (profile.type == "ssr") {
        ssr->stop();
    } else if (profile.type == "vmess") {
        v2ray->stop();
    } else if (profile.type == "trojan") {
        trojan->stop();
        if (conf->isEnableTrojanAPI()) {
            trojanGoAPI->stop();
        }
    }

    //if we have started http proxy, stop it
    if (conf->isEnableHttpMode()) {
        http->close();
    }

    emit stateChanged(running);
    emit startFailed();

    //set proxy settings if the setting is configured to do so
    if (conf->getSystemProxySettings() != "direct" && conf->getSystemProxySettings() != "advance") {
        SystemProxyHelper::setSystemProxy(0);
    }
}

void Connection::onNotifyConnectionChanged()
{
    emit connectionSwitched();
}

void Connection::testAddressLatency(const QHostAddress &addr)
{
    AddressTester *addrTester = new AddressTester(addr, profile.serverPort, this);
    connect(addrTester, &AddressTester::lagTestFinished, this, &Connection::onLatencyAvailable, Qt::QueuedConnection);
    addrTester->startLagTest();
}

void Connection::onTrojanConnectionDestoryed(Connection& connection, const uint64_t &download, const uint64_t &upload)
{
    connection.onNewBytesTransmitted(upload, download);
}

void Connection::onNewBytesTransmitted(const quint64 &u, const quint64 &d)
{
    profile.currentUsage += u + d;
    profile.totalUsage += u + d;
    emit dataUsageChanged(profile.currentUsage, profile.totalUsage);
    emit dataTrafficAvailable(u, d);
}

void Connection::onServerAddressLookedUp(const QHostInfo &host)
{
    if (host.error() == QHostInfo::NoError) {
        testAddressLatency(host.addresses().first());
    } else {
        onLatencyAvailable(TQProfile::LATENCY_ERROR);
    }
}

void Connection::onLatencyAvailable(const int latency)
{
    profile.latency = latency;
    emit latencyAvailable(latency);
}

void Connection::onLog(QString string)
{
    qDebug() << string;
}
