#include "addresstester.h"
#include "connection.h"
#include "confighelper.h"
#include <QCoreApplication>
#include <QDir>
#include <QHostInfo>
#include <QHostAddress>

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
    if (profile.serverAddress.isEmpty() || profile.password.isEmpty() || profile.localAddress.isEmpty()) {
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

    // MUST initial there otherwise privoxy will not listen port
    privoxy = new PrivoxyThread(this);

    ConfigHelper *conf = new ConfigHelper(configFile);

    // Generate Config File that trojan and privoxy will use
    ConfigHelper::connectionToJson(profile);
    ConfigHelper::generatePrivoxyConf(profile);

#ifdef Q_OS_WIN
    QString file = QCoreApplication::applicationDirPath() + "/config.json";
#else
    QDir configDir = QDir::homePath() + "/.config/trojan-qt5";
    QString file = configDir.absolutePath() + "/config.json";
#endif
    service->config().load(file.toStdString());

    running = true;
    service->start();
    if (profile.dualMode) {
        privoxy->start();
    }
    emit stateChanged(running);
    if (conf->isAutoSetSystemProxy()) {
        SystemProxyHelper::setSystemProxy(profile, running);
    }
}

void Connection::stop()
{
    ConfigHelper *conf = new ConfigHelper(configFile);
    if (running) {
        running = false;
        service->stop();
        if (profile.dualMode) {
            privoxy->stop();
        }
        emit stateChanged(running);
        if (conf->isAutoSetSystemProxy()) {
            SystemProxyHelper::setSystemProxy(profile, running);
        }
    }
}

void Connection::onStartFailed()
{
    running = false;
    emit stateChanged(running);
    emit startFailed();
    SystemProxyHelper::setSystemProxy(profile, running);
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
