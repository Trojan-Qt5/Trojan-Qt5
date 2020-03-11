#include "addresstester.h"
#include "connection.h"
#include "confighelper.h"
#include "pacserver.h"
#include "portvalidator.h"
#include <QCoreApplication>
#include <QDir>
#include <QHostInfo>
#include <QHostAddress>

#include "logger.h"

#include <boost/exception/all.hpp>

Connection::Connection(QObject *parent) :
    QObject(parent),
    running(false),
    service(new ServiceThread(this))
{
#ifdef Q_OS_WIN
    configFile = QCoreApplication::applicationDirPath() + "/config.ini";
#else
    QDir configDir = QDir::homePath() + "/.config/trojan-qt5";
    configFile = configDir.absolutePath() + "/config.ini";
#endif
    connect(service, &ServiceThread::startFailed, this, &Connection::onStartFailed);
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

QByteArray Connection::getURI() const
{
    QString uri = profile.toUri();
    return QByteArray(uri.toUtf8());
}

bool Connection::isValid() const
{
    if (profile.serverAddress.isEmpty() || profile.password.isEmpty()) {
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

    //MUST initial there otherwise privoxy will not listen port
    privoxy = new PrivoxyThread();

    ConfigHelper *conf = new ConfigHelper(configFile);

    //generate Config File that trojan and privoxy will use
    conf->connectionToJson(profile);
    conf->generatePrivoxyConf();

#ifdef Q_OS_WIN
    QString file = QCoreApplication::applicationDirPath() + "/config.json";
#else
    QDir configDir = QDir::homePath() + "/.config/trojan-qt5";
    QString file = configDir.absolutePath() + "/config.json";
#endif

    //load service config first
    try {
        service->config().load(file.toLocal8Bit().data());
    } catch (boost::exception &e) {
        Logger::error(QString::fromStdString(boost::diagnostic_information(e)));
    }

    //wait, let's check if port is in use
    if (conf->isCheckPortAvailability()) {
        PortValidator *pv = new PortValidator();
        if (pv->isInUse(conf->getSocks5Port())) {
            qCritical() << QString("Socks5 port %1 is being used").arg(QString::number(conf->getSocks5Port()));
            Logger::error(QString("Socks5 port %1 is being used").arg(QString::number(conf->getSocks5Port())));
            return;
        }
        if (pv->isInUse(conf->getHttpPort())) {
            qCritical() << QString("Http port %1 is being used").arg(QString::number(conf->getHttpPort()));
            Logger::error(QString("Http port %1 is being used").arg(QString::number(conf->getHttpPort())));
            return;
        }
    }

    //set running status to true before we start trojan
    running = true;
    service->start();

    //start privoxy if profile is configured to do so
    if (profile.dualMode) {
        privoxy->start();
    }

    emit stateChanged(running);

    //set proxy settings after emit the signal
    if (conf->isAutoSetSystemProxy()) {
        if (conf->isEnablePACMode()) {
            SystemProxyHelper::setSystemProxy(2);
        } else {
            SystemProxyHelper::setSystemProxy(1);
        }
    }
}

void Connection::stop()
{
    ConfigHelper *conf = new ConfigHelper(configFile);

    if (running) {
        //set the running status to false first. */
        running = false;
        service->stop();

        //if we have started privoxy, stop it
        if (profile.dualMode) {
            privoxy->stop();
        }

        emit stateChanged(running);

        //set proxy settings after emit the signal
        if (conf->isAutoSetSystemProxy()) {
            SystemProxyHelper::setSystemProxy(0);
        }
    }
}

void Connection::onStartFailed()
{
    ConfigHelper *conf = new ConfigHelper(configFile);

    running = false;
    emit stateChanged(running);
    emit startFailed();

    //set proxy settings if the setting is configured to do so
    if (conf->isAutoSetSystemProxy()) {
        SystemProxyHelper::setSystemProxy(0);
    }
}

void Connection::testAddressLatency(const QHostAddress &addr)
{
    AddressTester *addrTester = new AddressTester(addr, profile.serverPort, this);
    connect(addrTester, &AddressTester::lagTestFinished, this, &Connection::onLatencyAvailable, Qt::QueuedConnection);
    addrTester->startLagTest();
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
