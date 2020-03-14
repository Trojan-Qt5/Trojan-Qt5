#include "confighelper.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>

#include "logger.h"

ConfigHelper::ConfigHelper(const QString &configuration, QObject *parent) :
    QObject(parent),
    configFile(configuration)
{
    settings = new QSettings(configFile, QSettings::IniFormat, this);
    readGeneralSettings();
    readAdvanceSettings();
}

const QString ConfigHelper::profilePrefix = "Profile";

const QString ConfigHelper::subscribePrefix = "Subscribe";

void ConfigHelper::save(const ConnectionTableModel &model)
{
    int size = model.rowCount();
    settings->beginWriteArray(profilePrefix);
    for (int i = 0; i < size; ++i) {
        settings->setArrayIndex(i);
        Connection *con = model.getItem(i)->getConnection();
        QVariant value = QVariant::fromValue<TQProfile>(con->getProfile());
        settings->setValue("TQProfile", value);
    }
    settings->endArray();

    settings->setValue("EnableHttpMode", QVariant(enableHttpMode));
    settings->setValue("Socks5LocalAddress", QVariant(socks5LocalAddress));
    settings->setValue("Socks5LocalPort", QVariant(socks5LocalPort));
    settings->setValue("HttpLocalAddress", QVariant(httpLocalAddress));
    settings->setValue("HttpLocalPort", QVariant(httpLocalPort));
    settings->setValue("PACLocalAddress", QVariant(pacLocalAddress));
    settings->setValue("PACLocalPort", QVariant(pacLocalPort));
    settings->setValue("ToolbarStyle", QVariant(toolbarStyle));
    settings->setValue("AutoSetSystemProxy", QVariant(autoSetSystemProxy));
    settings->setValue("EnablePACMode", QVariant(enablePACMode));
    settings->setValue("HideWindowOnStartup", QVariant(hideWindowOnStartup));
    settings->setValue("StartAtLogin", QVariant(startAtLogin));
    settings->setValue("OnlyOneInstance", QVariant(onlyOneInstace));
    settings->setValue("CheckPortAvailability", QVariant(checkPortAvailability));
    settings->setValue("ShowToolbar", QVariant(showToolbar));
    settings->setValue("ShowFilterBar", QVariant(showFilterBar));
    settings->setValue("NativeMenuBar", QVariant(nativeMenuBar));
    settings->setValue("ConfigVersion", QVariant(2.6));
}

void ConfigHelper::saveSubscribes(QList<TQSubscribe> subscribes)
{
    int size = subscribes.size();
    settings->beginWriteArray(subscribePrefix);
    for (int i = 0; i < size; ++i) {
        settings->setArrayIndex(i);
        QVariant value = QVariant::fromValue<TQSubscribe>(subscribes[i]);
        settings->setValue("TQSubscribe", value);
    }
    settings->endArray();
}

void ConfigHelper::importGuiConfigJson(ConnectionTableModel *model, const QString &file)
{
    QFile JSONFile(file);
    JSONFile.open(QIODevice::ReadOnly | QIODevice::Text);
    if (!JSONFile.isOpen()) {
        qCritical() << "Error: cannot open " << file;
        Logger::error(QString("cannot open %1").arg(file));
        return;
    }
    if(!JSONFile.isReadable()) {
        qCritical() << "Error: cannot read " << file;
        Logger::error(QString("cannot read %1").arg(file));
        return;
    }

    QJsonParseError pe;
    QJsonDocument JSONDoc = QJsonDocument::fromJson(JSONFile.readAll(), &pe);
    JSONFile.close();
    if (pe.error != QJsonParseError::NoError) {
        qCritical() << pe.errorString();
        Logger::error(pe.errorString());
    }
    if (JSONDoc.isEmpty()) {
        qCritical() << "JSON Document" << file << "is empty!";
        Logger::error(QString("JSON Document %1 is empty!").arg(file));
        return;
    }
    QJsonObject JSONObj = JSONDoc.object();
    QJsonArray CONFArray = JSONObj["configs"].toArray();
    if (CONFArray.isEmpty()) {
        qWarning() << "configs in " << file << " is empty.";
        Logger::error(QString("configs in %1 is empty!").arg(file));
        return;
    }

    for (QJsonArray::iterator it = CONFArray.begin(); it != CONFArray.end(); ++it) {
        QJsonObject json = (*it).toObject();
        TQProfile p;
        p.name = json["remarks"].toString();
        p.serverPort = json["server_port"].toInt();
        p.serverAddress = json["server"].toString();
        p.verifyCertificate = json["verify_certificate"].toBool();
        p.verifyHostname = json["verify_hostname"].toBool();
        p.password = json["password"].toString();
        p.reuseSession = json["reuse_session"].toBool();
        p.sessionTicket = json["session_ticket"].toBool();
        p.reusePort = json["reuse_port"].toBool();
        p.tcpFastOpen = json["tcp_fast_open"].toBool();
        Connection *con = new Connection(p, this);
        model->appendConnection(con);
    }

}

void ConfigHelper::exportGuiConfigJson(const ConnectionTableModel &model, const QString &file)
{
    QJsonArray confArray;
    int size = model.rowCount();
    for (int i = 0; i < size; ++i) {
        Connection *con = model.getItem(i)->getConnection();
        QJsonObject json;
        json["remarks"] = QJsonValue(con->profile.name);
        json["server"] = QJsonValue(con->profile.serverAddress);
        json["server_port"] = QJsonValue(con->profile.serverPort);
        json["verify_certificate"] = QJsonValue(con->profile.verifyCertificate);
        json["verify_hostname"] = QJsonValue(con->profile.verifyHostname);
        json["password"] = QJsonValue(con->profile.password);
        json["reuse_session"] = QJsonValue(con->profile.reuseSession);
        json["session_ticket"] = QJsonValue(con->profile.sessionTicket);
        json["reuse_port"] = QJsonValue(con->profile.reusePort);
        json["tcp_fast_open"] = QJsonValue(con->profile.tcpFastOpen);
        confArray.append(QJsonValue(json));
    }

    QJsonObject JSONObj;
    JSONObj["configs"] = QJsonValue(confArray);

    QJsonDocument JSONDoc(JSONObj);

    QFile JSONFile(file);
    JSONFile.open(QIODevice::WriteOnly | QIODevice::Text);
    if (!JSONFile.isOpen()) {
        qCritical() << "Error: cannot open " << file;
        Logger::error(QString("cannot open %1").arg(file));
        return;
    }
    if(!JSONFile.isWritable()) {
        qCritical() << "Error: cannot write into " << file;
        Logger::error(QString("cannot write into %1").arg(file));
        return;
    }

    JSONFile.write(JSONDoc.toJson());
    JSONFile.close();
}

void ConfigHelper::importShadowrocketJson(ConnectionTableModel *model, const QString &file)
{
    QFile JSONFile(file);
    JSONFile.open(QIODevice::ReadOnly | QIODevice::Text);
    if (!JSONFile.isOpen()) {
        qCritical() << "Error: cannot open " << file;
        Logger::error(QString("cannot open %1").arg(file));
        return;
    }
    if(!JSONFile.isReadable()) {
        qCritical() << "Error: cannot read " << file;
        Logger::error(QString("cannot read %1").arg(file));
        return;
    }

    QJsonParseError pe;
    QJsonDocument JSONDoc = QJsonDocument::fromJson(JSONFile.readAll(), &pe);
    JSONFile.close();
    if (pe.error != QJsonParseError::NoError) {
        qCritical() << pe.errorString();
        Logger::error(pe.errorString());
    }
    if (JSONDoc.isEmpty()) {
        qCritical() << "JSON Document" << file << "is empty!";
        Logger::error(QString("JSON Document %1 is empty!").arg(file));
        return;
    }

    QJsonArray CONFArray = JSONDoc.array();
    for (QJsonArray::iterator it = CONFArray.begin(); it != CONFArray.end(); ++it) {
        QJsonObject json = (*it).toObject();
        TQProfile p;
        if (json["type"].toString() == "Trojan") {
            p.serverAddress = json["host"].toString();
            p.serverPort = json["port"].toInt();
            p.password = json["password"].toString();
            p.name = json["title"].toString();
        }
        Connection *con = new Connection(p, this);
        model->appendConnection(con);
    }

}

void ConfigHelper::exportShadowrocketJson(const ConnectionTableModel &model, const QString &file)
{
    QJsonArray confArray;
    int size = model.rowCount();
    for (int i = 0; i < size; ++i) {
        Connection *con = model.getItem(i)->getConnection();
        QJsonObject json;
        json["type"] = "Trojan";
        json["title"] = QJsonValue(con->profile.name);
        json["host"] = QJsonValue(con->profile.serverAddress);
        json["port"] = QJsonValue(con->profile.serverPort);
        json["password"] = QJsonValue(con->profile.password);
        confArray.append(QJsonValue(json));
    }

    QJsonDocument JSONDoc(confArray);

    QFile JSONFile(file);
    JSONFile.open(QIODevice::WriteOnly | QIODevice::Text);
    if (!JSONFile.isOpen()) {
        qCritical() << "Error: cannot open " << file;
        Logger::error(QString("cannot open %1").arg(file));
        return;
    }
    if(!JSONFile.isWritable()) {
        qCritical() << "Error: cannot write into " << file;
        Logger::error(QString("cannot write into %1").arg(file));
        return;
    }

    JSONFile.write(JSONDoc.toJson());
    JSONFile.close();
}

Connection* ConfigHelper::configJsonToConnection(const QString &file)
{
    QFile JSONFile(file);
    JSONFile.open(QIODevice::ReadOnly | QIODevice::Text);
    if (!JSONFile.isOpen()) {
        qCritical() << "Error: cannot open " << file;
        Logger::error(QString("cannot open %1").arg(file));
    }
    if(!JSONFile.isReadable()) {
        qCritical() << "Error: cannot read " << file;
        Logger::error(QString("cannot read %1").arg(file));
    }

    QJsonParseError pe;
    QJsonDocument JSONDoc = QJsonDocument::fromJson(JSONFile.readAll(), &pe);
    JSONFile.close();
    if (pe.error != QJsonParseError::NoError) {
        qCritical() << pe.errorString();
    }
    if (JSONDoc.isEmpty()) {
        qCritical() << "JSON Document" << file << "is empty!";
        Logger::error(QString("JSON Document %1 is empty!").arg(file));
        return nullptr;
    }
    QJsonObject configObj = JSONDoc.object();
    TQProfile p;
    p.serverAddress = configObj["server"].toString();
    p.serverPort = configObj["server_port"].toInt();
    p.password = configObj["password"].toString();
    Connection *con = new Connection(p, this);
    return con;
}

void ConfigHelper::connectionToJson(TQProfile &profile)
{
    QJsonObject configObj;
    configObj["run_type"] = "client";
    configObj["local_addr"] = socks5LocalAddress;
    configObj["local_port"] = socks5LocalPort;
    configObj["remote_addr"] = profile.serverAddress;
    configObj["remote_port"] = profile.serverPort;
    QJsonArray passwordArray;
    passwordArray.append(profile.password);
    configObj["password"] = QJsonValue(passwordArray);
    configObj["log_level"] = "1";
    QJsonObject ssl;
    ssl["verify"] = profile.verifyCertificate;
    ssl["verify_hostname"] = profile.verifyHostname;
    ssl["cert"] = "";
    ssl["cipher"] = "ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES128-GCM-SHA256:ECDHE-ECDSA-CHACHA20-POLY1305:ECDHE-RSA-CHACHA20-POLY1305:ECDHE-ECDSA-AES256-GCM-SHA384:ECDHE-RSA-AES256-GCM-SHA384:ECDHE-ECDSA-AES256-SHA:ECDHE-ECDSA-AES128-SHA:ECDHE-RSA-AES128-SHA:ECDHE-RSA-AES256-SHA:DHE-RSA-AES128-SHA:DHE-RSA-AES256-SHA:AES128-SHA:AES256-SHA:DES-CBC3-SHA";
    ssl["cipher_tls13"] = "TLS_AES_128_GCM_SHA256:TLS_CHACHA20_POLY1305_SHA256:TLS_AES_256_GCM_SHA384";
    ssl["sni"] = "";
    QJsonArray alpnArray;
    alpnArray.append("h2");
    alpnArray.append("http/1.1");
    ssl["alpn"] = QJsonValue(alpnArray);
    ssl["reuse_session"] = profile.reuseSession;
    ssl["session_ticket"] = profile.sessionTicket;
    ssl["curves"] = "";
    configObj["ssl"] = QJsonValue(ssl);
    QJsonObject tcp;
    tcp["no_delay"] = true;
    tcp["keep_alive"] = true;
    tcp["reuse_port"] = profile.reusePort;
    tcp["fast_open"] = profile.tcpFastOpen;
    tcp["fast_open_qlen"] = 20;
    configObj["tcp"] = QJsonValue(tcp);

    QJsonDocument JSONDoc(configObj);

#ifdef Q_OS_WIN
        QString file = QCoreApplication::applicationDirPath() + "/config.json";
#else
        QDir configDir = QDir::homePath() + "/.config/trojan-qt5";
        QString file = configDir.absolutePath() + "/config.json";
#endif

    QFile JSONFile(file);
    JSONFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
    if (!JSONFile.isOpen()) {
        qCritical() << "Error: cannot open " << file;
        Logger::error(QString("cannot open %1").arg(file));
        return;
    }
    if(!JSONFile.isWritable()) {
        qCritical() << "Error: cannot write into " << file;
        Logger::error(QString("cannot write into %1").arg(file));
        return;
    }

    JSONFile.write(JSONDoc.toJson());
    JSONFile.close();

}

void ConfigHelper::generatePrivoxyConf()
{
    QString filecontent = QString("listen-address %1:%2\n"
                                  "toggle 0\n"
                                  "show-on-task-bar 0\n"
                                  "activity-animation 0\n"
                                  "forward-socks5 / %3:%4 .\n"
                                  "hide-console\n")
                                  .arg(httpLocalAddress)
                                  .arg(QString::number(httpLocalPort))
                                  .arg(socks5LocalAddress)
                                  .arg(QString::number(socks5LocalPort));
#ifdef Q_OS_WIN
        QString file = QCoreApplication::applicationDirPath() + "/privoxy/privoxy.conf";
#else
        QDir configDir = QDir::homePath() + "/.config/trojan-qt5";
        QString file = configDir.absolutePath() + "/privoxy.conf";
#endif
    QFile privoxyConf(file);
    privoxyConf.open(QIODevice::WriteOnly | QIODevice::Text |QIODevice::Truncate);
    if (!privoxyConf.isOpen()) {
        qCritical() << "Error: cannot open " << file;
        Logger::error(QString("cannot open %1").arg(file));
        return;
    }
    if(!privoxyConf.isWritable()) {
        qCritical() << "Error: cannot write into " << file;
        Logger::error(QString("cannot write into %1").arg(file));
        return;
    }
    privoxyConf.write(filecontent.toUtf8());
    privoxyConf.close();
}

int ConfigHelper::getToolbarStyle() const
{
    return toolbarStyle;
}

QString ConfigHelper::getSocks5Address() const
{
    return socks5LocalAddress;
}

int ConfigHelper::getSocks5Port() const
{
    return socks5LocalPort;
}

QString ConfigHelper::getHttpAddress() const
{
    return httpLocalAddress;
}

int ConfigHelper::getHttpPort() const
{
    return httpLocalPort;
}

QString ConfigHelper::getPACAddress() const
{
    return pacLocalAddress;
}

int ConfigHelper::getPACPort() const
{
    return pacLocalPort;
}

bool ConfigHelper::isAutoSetSystemProxy() const
{
    return autoSetSystemProxy;
}

bool ConfigHelper::isEnableHttpMode() const
{
    return enableHttpMode;
}

bool ConfigHelper::isEnablePACMode() const
{
    return enablePACMode;
}

bool ConfigHelper::isHideWindowOnStartup() const
{
    return hideWindowOnStartup;
}

bool ConfigHelper::isStartAtLogin() const
{
    return startAtLogin;
}

bool ConfigHelper::isOnlyOneInstance() const
{
    return onlyOneInstace;
}

bool ConfigHelper::isCheckPortAvailability() const
{
    return checkPortAvailability;
}

bool ConfigHelper::isShowToolbar() const
{
    return showToolbar;
}

bool ConfigHelper::isShowFilterBar() const
{
    return showFilterBar;
}

bool ConfigHelper::isNativeMenuBar() const
{
    return nativeMenuBar;
}

void ConfigHelper::setGeneralSettings(int ts, bool assp, bool pac, bool hide, bool sal, bool oneInstance, bool cpa, bool nativeMB)
{
    if (toolbarStyle != ts) {
        emit toolbarStyleChanged(static_cast<Qt::ToolButtonStyle>(ts));
    }
    toolbarStyle = ts;
    autoSetSystemProxy = assp;
    enablePACMode = pac;
    hideWindowOnStartup = hide;
    startAtLogin = sal;
    onlyOneInstace = oneInstance;
    checkPortAvailability = cpa;
    nativeMenuBar = nativeMB;
}

void ConfigHelper::setAdvanceSettings(bool hm, QString sa, int sp, QString ha, int hp, QString pa, int pp)
{
    enableHttpMode = hm;
    socks5LocalAddress = sa;
    socks5LocalPort = sp;
    httpLocalAddress = ha;
    httpLocalPort = hp;
    pacLocalAddress = pa;
    pacLocalPort = pp;
}

void ConfigHelper::setSystemProxySettings(bool pac, bool enable)
{
   enablePACMode = pac;
   autoSetSystemProxy = enable;
   settings->setValue("AutoSetSystemProxy", QVariant(autoSetSystemProxy));
   settings->setValue("EnablePACMode", QVariant(enablePACMode));
}

void ConfigHelper::setShowToolbar(bool show)
{
    showToolbar = show;
}

void ConfigHelper::setShowFilterBar(bool show)
{
    showFilterBar = show;
}

void ConfigHelper::read(ConnectionTableModel *model)
{
    qreal configVer = settings->value("ConfigVersion", QVariant(2.4)).toReal();
    int size = settings->beginReadArray(profilePrefix);
    for (int i = 0; i < size; ++i) {
        settings->setArrayIndex(i);
        QVariant value = settings->value("TQProfile");
        TQProfile profile = value.value<TQProfile>();
        checkProfileDataUsageReset(profile);
        Connection *con = new Connection(profile, this);
        model->appendConnection(con);
    }
    settings->endArray();
    readGeneralSettings();
    readAdvanceSettings();
}

QList<TQSubscribe> ConfigHelper::readSubscribes()
{
    int size = settings->beginReadArray(subscribePrefix);
    QList<TQSubscribe> subscribes;
    for (int i = 0; i < size; ++i) {
        settings->setArrayIndex(i);
        QVariant value = settings->value("TQSubscribe");
        TQSubscribe subscribe = value.value<TQSubscribe>();
        subscribes.append(subscribe);
    }
    settings->endArray();
    return subscribes;
}

void ConfigHelper::readGeneralSettings()
{
    toolbarStyle = settings->value("ToolbarStyle", QVariant(3)).toInt();
    startAtLogin = settings->value("StartAtLogin").toBool();
    autoSetSystemProxy = settings->value("AutoSetSystemProxy", QVariant(true)).toBool();
    enablePACMode = settings->value("EnablePACMode", QVariant(true)).toBool();
    hideWindowOnStartup = settings->value("HideWindowOnStartup").toBool();
    onlyOneInstace = settings->value("OnlyOneInstance", QVariant(true)).toBool();
    checkPortAvailability = settings->value("CheckPortAvailability", QVariant(true)).toBool();
    showToolbar = settings->value("ShowToolbar", QVariant(true)).toBool();
    showFilterBar = settings->value("ShowFilterBar", QVariant(true)).toBool();
    nativeMenuBar = settings->value("NativeMenuBar", QVariant(false)).toBool();
}

void ConfigHelper::readAdvanceSettings()
{
    enableHttpMode = settings->value("EnableHttpMode", QVariant(true)).toBool();
    socks5LocalAddress = settings->value("Socks5LocalAddress", QVariant("127.0.0.1")).toString();
    socks5LocalPort = settings->value("Socks5LocalPort", QVariant(1080)).toInt();
    httpLocalAddress = settings->value("HttpLocalAddress", QVariant("127.0.0.1")).toString();
    httpLocalPort = settings->value("HttpLocalPort", QVariant(1081)).toInt();
    pacLocalAddress = settings->value("PACLocalAddress", QVariant("127.0.0.1")).toString();
    pacLocalPort = settings->value("PACLocalPort", QVariant(8070)).toInt();
}

void ConfigHelper::checkProfileDataUsageReset(TQProfile &profile)
{
    QDate currentDate = QDate::currentDate();
    if (profile.nextResetDate.isNull()){//invalid if the config.ini is old
        //the default reset day is 1 of every month
        profile.nextResetDate = QDate(currentDate.year(), currentDate.month(), 1);
        qDebug() << "config.ini upgraded from old version";
        profile.totalUsage += profile.currentUsage;//we used to use sent and received
    }

    if (profile.nextResetDate < currentDate) {//not <= because that'd casue multiple reset on this day
        profile.currentUsage = 0;
        while (profile.nextResetDate <= currentDate) {
            profile.nextResetDate = profile.nextResetDate.addMonths(1);
        }
    }
}

void ConfigHelper::startAllAutoStart(const ConnectionTableModel& model)
{
    int size = model.rowCount();
    for (int i = 0; i < size; ++i) {
        Connection *con = model.getItem(i)->getConnection();
        if (con->profile.autoStart) {
            con->start();
        }
    }
}

void ConfigHelper::setStartAtLogin()
{
    QString applicationName = "Trojan-Qt5";
    QString applicationFilePath = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
#if defined(Q_OS_WIN)
    QSettings settings("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
#elif defined(Q_OS_LINUX)
    QFile file(QDir::homePath() + "/.config/autostart/trojan-qt5.desktop");
    QString fileContent(
            "[Desktop Entry]\n"
            "Name=%1\n"
            "Exec=%2\n"
            "Type=Application\n"
            "Terminal=false\n"
            "X-GNOME-Autostart-enabled=true\n");
#elif defined(Q_OS_MAC)
    QFile file(QDir::homePath() + "/Library/LaunchAgents/org.trojan.trojan-qt5.launcher.plist");
    QString fileContent(
            "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n"
            "<plist version=\"1.0\">\n"
            "<dict>\n"
            "  <key>Label</key>\n"
            "  <string>org.trojan.trojan-qt5.launcher</string>\n"
            "  <key>LimitLoadToSessionType</key>\n"
            "  <string>Aqua</string>\n"
            "  <key>ProgramArguments</key>\n"
            "  <array>\n"
            "    <string>%2</string>\n"
            "  </array>\n"
            "  <key>RunAtLoad</key>\n"
            "  <true/>\n"
            "  <key>StandardErrorPath</key>\n"
            "  <string>/dev/null</string>\n"
            "  <key>StandardOutPath</key>\n"
            "  <string>/dev/null</string>\n"
            "</dict>\n"
            "</plist>\n");
#else
    QFile file;
    QString fileContent;
#endif

    if (this->isStartAtLogin()) {
        // Create start up item
    #if defined(Q_OS_WIN)
        settings.setValue(applicationName, applicationFilePath);
    #else
        fileContent.replace("%1", applicationName);
        fileContent.replace("%2", applicationFilePath);
        if ( file.open(QIODevice::WriteOnly) ) {
            file.write(fileContent.toUtf8());
            file.close();
        }
    #endif
    } else {
        // Delete start up item
        #if defined(Q_OS_WIN)
            settings.remove(applicationName);
        #else
            if ( file.exists() ) {
                file.remove();
            }
        #endif
    }
}

QByteArray ConfigHelper::getMainWindowGeometry() const
{
    return settings->value("MainWindowGeometry").toByteArray();
}

QByteArray ConfigHelper::getMainWindowState() const
{
    return settings->value("MainWindowState").toByteArray();
}

QByteArray ConfigHelper::getTableGeometry() const
{
    return settings->value("MainTableGeometry").toByteArray();
}

QByteArray ConfigHelper::getTableState() const
{
    return settings->value("MainTableState").toByteArray();
}

void ConfigHelper::setMainWindowGeometry(const QByteArray &geometry)
{
    settings->setValue("MainWindowGeometry", QVariant(geometry));
}

void ConfigHelper::setMainWindowState(const QByteArray &state)
{
    settings->setValue("MainWindowState", QVariant(state));
}

void ConfigHelper::setTableGeometry(const QByteArray &geometry)
{
    settings->setValue("MainTableGeometry", QVariant(geometry));
}

void ConfigHelper::setTableState(const QByteArray &state)
{
    settings->setValue("MainTableState", QVariant(state));
}
