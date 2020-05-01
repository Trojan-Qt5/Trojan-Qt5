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
#include "yaml-cpp/yaml.h"

using namespace std;

ConfigHelper::ConfigHelper(const QString &configuration, QObject *parent) :
    QObject(parent),
    configFile(configuration)
{
    settings = new QSettings(configFile, QSettings::IniFormat, this);
    readGeneralSettings();
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

    settings->setValue("LogLevel", QVariant(logLevel));
    settings->setValue("EnableHttpMode", QVariant(enableHttpMode));
    settings->setValue("Socks5LocalPort", QVariant(socks5Port));
    settings->setValue("HttpLocalPort", QVariant(httpPort));
    settings->setValue("PACLocalPort", QVariant(pacPort));
    settings->setValue("HaproxyStatusPort", QVariant(haproxyStatusPort));
    settings->setValue("HaproxyPort", QVariant(haproxyPort));
    settings->setValue("ToolbarStyle", QVariant(toolbarStyle));
    settings->setValue("AutoUpdateSubscribes", QVariant(autoUpdateSubscribes));
    settings->setValue("TrojanOn", QVariant(trojanOn));
    settings->setValue("SystemProxyMode", QVariant(systemProxyMode));
    settings->setValue("EnableIPV6Support", QVariant(enableIpv6Support));
    settings->setValue("ShareOverLan", QVariant(shareOverLan));
    settings->setValue("ServerLoadBalance", QVariant(serverLoadBalance));
    settings->setValue("HideWindowOnStartup", QVariant(hideWindowOnStartup));
    settings->setValue("StartAtLogin", QVariant(startAtLogin));
    settings->setValue("OnlyOneInstance", QVariant(onlyOneInstace));
    settings->setValue("CheckPortAvailability", QVariant(checkPortAvailability));
    settings->setValue("EnableNotification", QVariant(enableNotification));
    settings->setValue("HideDockIcon", QVariant(hideDockIcon));
    settings->setValue("ShowToolbar", QVariant(showToolbar));
    settings->setValue("ShowFilterBar", QVariant(showFilterBar));
    settings->setValue("NativeMenuBar", QVariant(nativeMenuBar));
    settings->setValue("ConfigVersion", QVariant(2.6));
    settings->setValue("ForwardProxy", QVariant(enableForwardProxy));
    settings->setValue("ForwardProxyType", QVariant(forwardProxyType));
    settings->setValue("ForwardProxyAddress", QVariant(forwardProxyAddress));
    settings->setValue("ForwardProxyPort", QVariant(forwardProxyPort));
    settings->setValue("ForwardProxyAuthentication", QVariant(enableForwardProxyAuthentication));
    settings->setValue("ForwardProxyUsername", QVariant(forwardProxyUsername));
    settings->setValue("ForwardProxyPassword", QVariant(forwardProxyPassword));
    settings->setValue("GFWListUrl", QVariant(gfwlistUrl));
    settings->setValue("UpdateUserAgent", QVariant(updateUserAgent));
    settings->setValue("FilterKeyword", QVariant(filterKeyword));
    settings->setValue("MaximumSubscribe", QVariant(maximumSubscribe));
    settings->setValue("Fingerprint", QVariant(fingerprint));
    settings->setValue("EnableTrojanAPI", QVariant(enableTrojanAPI));
    settings->setValue("EnableTrojanRouter", QVariant(enableTrojanRouter));
    settings->setValue("TrojanAPIPort", QVariant(trojanAPIPort));
    settings->setValue("TrojanCertPath", QVariant(trojanCertPath));
    settings->setValue("TrojanCipher", QVariant(trojanCipher));
    settings->setValue("TrojanCipherTLS13", QVariant(trojanCipherTLS13));
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
        p.sni = json["sni"].toString();
        p.reuseSession = json["reuse_session"].toBool();
        p.sessionTicket = json["session_ticket"].toBool();
        p.reusePort = json["reuse_port"].toBool();
        p.tcpFastOpen = json["tcp_fast_open"].toBool();
        p.mux = json["mux"].toBool();
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
        json["type"] = QJsonValue(con->profile.type);
        json["remarks"] = QJsonValue(con->profile.name);
        json["server"] = QJsonValue(con->profile.serverAddress);
        json["server_port"] = QJsonValue(con->profile.serverPort);
        json["verify_certificate"] = QJsonValue(con->profile.verifyCertificate);
        json["verify_hostname"] = QJsonValue(con->profile.verifyHostname);
        json["password"] = QJsonValue(con->profile.password);
        json["sni"] = QJsonValue(con->profile.sni);
        json["reuse_session"] = QJsonValue(con->profile.reuseSession);
        json["session_ticket"] = QJsonValue(con->profile.sessionTicket);
        json["reuse_port"] = QJsonValue(con->profile.reusePort);
        json["tcp_fast_open"] = QJsonValue(con->profile.tcpFastOpen);
        json["mux"] = QJsonValue(con->profile.mux);
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

void ConfigHelper::importConfigYaml(ConnectionTableModel *model, const QString &file)
{
    YAML::Node config = YAML::LoadFile(file.toStdString());
    const YAML::Node& proxies = config["Proxy"];
    for (std::size_t i = 0; i < proxies.size(); i++) {
        const YAML::Node& proxy = proxies[i];
        if (QString::fromStdString(proxy["type"].as<string>()) == "trojan") {
            TQProfile p;
            p.name = QString::fromStdString(proxy["name"].as<string>());
            p.serverAddress = QString::fromStdString(proxy["server"].as<string>());
            p.serverPort = proxy["skip-cert-verify"].as<int>();
            p.password = QString::fromStdString(proxy["password"].as<string>());
            p.verifyCertificate = !proxy["skip-cert-verify"].as<bool>();
            Connection *con = new Connection(p, this);
            model->appendConnection(con);
        }
    }
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
        } else if (json["type"].toString() == "") {

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

void ConfigHelper::exportTrojanSubscribe(const ConnectionTableModel &model, const QString &file)
{
    QString uri;
    int size = model.rowCount();
    for (int i = 0; i < size; ++i) {
        Connection *con = model.getItem(i)->getConnection();
        uri += con->getURI("");
    }
    uri = uri.toUtf8().toBase64();

    QFile Subscribe(file);
    Subscribe.open(QIODevice::WriteOnly | QIODevice::Text);
    if (!Subscribe.isOpen()) {
        qCritical() << "Error: cannot open " << file;
        Logger::error(QString("cannot open %1").arg(file));
        return;
    }
    if(!Subscribe.isWritable()) {
        qCritical() << "Error: cannot write into " << file;
        Logger::error(QString("cannot write into %1").arg(file));
        return;
    }

    Subscribe.write(uri.toUtf8().data());
    Subscribe.close();
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
    p.serverAddress = configObj["remote_addr"].toString();
    p.serverPort = configObj["remote_port"].toInt();
    p.password = configObj["password"].toArray()[0].toString(); //only the first password will be used
    p.sni = configObj["ssl"].toObject()["sni"].toString();
    p.verifyCertificate = configObj["verify"].toBool();
    p.verifyHostname = configObj["verify_hostname"].toBool();
    p.reuseSession = configObj["ssl"].toObject()["reuse_session"].toBool();
    p.sessionTicket = configObj["ssl"].toObject()["session_ticket"].toBool();
    p.reusePort = configObj["tcp"].toObject()["reuse_port"].toBool();
    p.tcpFastOpen = configObj["tcp"].toObject()["fast_open"].toBool();
    p.mux = configObj["mux"].toObject()["enabled"].toBool();
    Connection *con = new Connection(p, this);
    return con;
}

void ConfigHelper::connectionToJson(TQProfile &profile)
{
    QJsonObject configObj;
    configObj["run_type"] = "client";
    configObj["local_addr"] = isEnableIpv6Support() ? (isShareOverLan() ? "::" : "::1") : (isShareOverLan() ? "0.0.0.0" : "127.0.0.1");
    configObj["local_port"] = socks5Port;
    configObj["remote_addr"] = profile.serverAddress;
    configObj["remote_port"] = profile.serverPort;
    QJsonArray passwordArray;
    passwordArray.append(profile.password);
    configObj["password"] = QJsonValue(passwordArray);
    configObj["log_level"] = logLevel;
    QJsonObject ssl;
    ssl["verify"] = profile.verifyCertificate;
    ssl["verify_hostname"] = profile.verifyHostname;
    ssl["cert"] = trojanCertPath;
    ssl["cipher"] = trojanCipher;
    ssl["cipher_tls13"] = trojanCipherTLS13;
    ssl["sni"] = profile.sni;
    QJsonArray alpnArray;
    alpnArray.append("h2");
    alpnArray.append("http/1.1");
    ssl["alpn"] = QJsonValue(alpnArray);
    ssl["reuse_session"] = profile.reuseSession;
    ssl["session_ticket"] = profile.sessionTicket;
    ssl["curves"] = "";
    ssl["fingerprint"] = parseTLSFingerprint(fingerprint);
    configObj["ssl"] = QJsonValue(ssl);
    QJsonObject tcp;
    tcp["no_delay"] = true;
    tcp["keep_alive"] = true;
    tcp["reuse_port"] = profile.reusePort;
    tcp["fast_open"] = profile.tcpFastOpen;
    tcp["fast_open_qlen"] = 20;
    configObj["tcp"] = QJsonValue(tcp);
    QJsonObject mux;
    mux["enabled"] = profile.mux;
    configObj["mux"] = QJsonValue(mux);
    QJsonObject websocket;
    websocket["enabled"] = profile.websocket;
    websocket["path"] = profile.websocketPath;
    websocket["hostname"] = profile.websocketHostname;
    websocket["obfuscation_password"] = profile.websocketObfsPassword;
    websocket["double_tls"] = profile.websocketDoubleTLS;
    configObj["websocket"] = QJsonValue(websocket);
    QJsonObject api;
    api["enabled"] = enableTrojanAPI;
    api["api_addr"] = "127.0.0.1";
    api["api_port"] = trojanAPIPort;
    configObj["api"] = QJsonValue(api);
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
                                  .arg(isEnableIpv6Support() ? (isShareOverLan() ? "[::]" : "[::1]") : (isShareOverLan() ? "0.0.0.0" : "127.0.0.1"))
                                  .arg(QString::number(httpPort))
                                  .arg(isEnableIpv6Support() ? (isShareOverLan() ? "[::]" : "[::1]") : (isShareOverLan() ? "0.0.0.0" : "127.0.0.1"))
                                  .arg(QString::number(socks5Port));
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

void ConfigHelper::generateHaproxyConf(const ConnectionTableModel &model)
{
#ifdef Q_OS_WIN
        QString haproxyConf = QCoreApplication::applicationDirPath() + "/haproxy.conf";
#else
        QDir configDir = QDir::homePath() + "/.config/trojan-qt5";
        QString haproxyConf = configDir.absolutePath() + "/haproxy.conf";
#endif

    if (QFile::exists(haproxyConf)) {
        QFile::remove(haproxyConf);
    }
    QFile::copy(":/conf/haproxy.conf", haproxyConf);
    QFile::setPermissions(haproxyConf, QFile::WriteOwner | QFile::ReadOwner | QFile::ReadGroup | QFile::ReadOther);

    QFile file(haproxyConf);
    file.open(QIODevice::ReadWrite); // open for read and write
    QByteArray fileData = file.readAll(); // read all the data into the byte array
    QString text(fileData); // add to text string for easy string replace
    text += "\n"; // append a new line to the end
    int size = model.rowCount();
    for (int i = 0; i < size; ++i) {
        TQProfile p = model.getItem(i)->getConnection()->getProfile();
        text += QString("    server trojan%1 %2:%3 check inter 1000 weight 2\n").arg(QString::number(i)).arg(p.serverAddress).arg(i);
    }
    text.replace(QString("__STATUS__"), QString("%1:%2").arg(isEnableIpv6Support() ? (isShareOverLan() ? "::" : "::1") : (isShareOverLan() ? "0.0.0.0" : "127.0.0.1")).arg(QString::number(haproxyStatusPort)));
    text.replace(QString("__TROJAN__"), QString("%1:%2").arg(isEnableIpv6Support() ? (isShareOverLan() ? "::" : "::1") : (isShareOverLan() ? "0.0.0.0" : "127.0.0.1")).arg(QString::number(haproxyPort)));
    text.replace(QString("__MODE__"), QString("%1"));
    file.seek(0); // go to the beginning of the file
    file.write(text.toUtf8()); // write the new text back to the file
    file.close(); // close the file handle.
}

QString ConfigHelper::parseTLSFingerprint(int choice) const
{
    switch (choice) {
    case 0:
        return "";
    case 1:
        return "auto";
    case 2:
        return "firefox";
    case 3:
        return "chrome";
    case 4:
        return "ios";
    case 5:
        return "randomized";
    }
    return "";
}

int ConfigHelper::getFLSFingerPrint() const
{
    return fingerprint;
}

int ConfigHelper::getLogLevel() const
{
    return logLevel;
}

int ConfigHelper::getToolbarStyle() const
{
    return toolbarStyle;
}

int ConfigHelper::getSocks5Port() const
{
    return socks5Port;
}

int ConfigHelper::getHttpPort() const
{
    return httpPort;
}

int ConfigHelper::getPACPort() const
{
    return pacPort;
}

int ConfigHelper::getHaproxyPort() const
{
    return haproxyPort;
}

int ConfigHelper::getHaproxyStatusPort() const
{
    return haproxyStatusPort;
}

QString ConfigHelper::getSystemProxySettings() const
{
    return systemProxyMode;
}

bool ConfigHelper::isTrojanOn() const
{
    return trojanOn;
}

bool ConfigHelper::isEnableServerLoadBalance() const
{
    return serverLoadBalance;
}

bool ConfigHelper::isEnableIpv6Support() const
{
    return enableIpv6Support;
}

bool ConfigHelper::isShareOverLan() const
{
    return shareOverLan;
}

bool ConfigHelper::isEnableHttpMode() const
{
    return enableHttpMode;
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

bool ConfigHelper::isEnableNotification() const
{
    return enableNotification;
}

bool ConfigHelper::isHideDockIcon() const
{
    return hideDockIcon;
}

bool ConfigHelper::isEnableForwardProxy() const
{
    return enableForwardProxy;
}

int ConfigHelper::getForwardProxyType() const
{
    return forwardProxyType;
}

QString ConfigHelper::getForwardProxyAddress() const
{
    return forwardProxyAddress;
}

int ConfigHelper::getForwardProxyPort() const
{
    return forwardProxyPort;
}

bool ConfigHelper::isEnableForwardProxyAuthentication() const
{
    return enableForwardProxyAuthentication;
}

QString ConfigHelper::getForwardProxyUsername() const
{
    return forwardProxyUsername;
}

QString ConfigHelper::getForwardProxyPassword() const
{
    return forwardProxyPassword;
}

int ConfigHelper::getGfwlistUrl() const
{
    return gfwlistUrl;
}

QString ConfigHelper::getUpdateUserAgent() const
{
    return updateUserAgent;
}

QString ConfigHelper::getFilterKeyword() const
{
    return filterKeyword;
}

int ConfigHelper::getMaximumSubscribe() const
{
    return maximumSubscribe;
}

bool ConfigHelper::isEnableTrojanAPI() const
{
    return enableTrojanAPI;
}

bool ConfigHelper::isEnableTrojanRouter() const
{
    return enableTrojanRouter;
}

int ConfigHelper::getTrojanAPIPort() const
{
    return trojanAPIPort;
}

QString ConfigHelper::getTrojanCertPath() const
{
    return trojanCertPath;
}

QString ConfigHelper::getTrojanCipher() const
{
    return trojanCipher;
}

QString ConfigHelper::getTrojanCipherTLS13() const
{
    return trojanCipherTLS13;
}

bool ConfigHelper::isAutoUpdateSubscribes() const
{
    return autoUpdateSubscribes;
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

void ConfigHelper::setGeneralSettings(int ts, bool hide, bool sal, bool oneInstance, bool cpa, bool en, bool hdi, bool nativeMB, int ll, bool hm, bool eis, bool sol, int sp, int hp, int pp, int ap, int hsp, bool efp, int fpt, QString fpa, int fpp, bool efpa, QString fpu, QString fppa, int glu, QString uua, QString fkw, int ms, int fp, bool eta, bool etr, int tap, QString tcp, QString tc, QString tct13)
{
    if (toolbarStyle != ts) {
        emit toolbarStyleChanged(static_cast<Qt::ToolButtonStyle>(ts));
    }
    toolbarStyle = ts;
    hideWindowOnStartup = hide;
    startAtLogin = sal;
    onlyOneInstace = oneInstance;
    checkPortAvailability = cpa;
    enableNotification = en;
    hideDockIcon = hdi;
    nativeMenuBar = nativeMB;
    logLevel = ll;
    enableHttpMode = hm;
    enableIpv6Support = eis;
    shareOverLan = sol;
    socks5Port = sp;
    httpPort = hp;
    pacPort = pp;
    haproxyPort = ap;
    haproxyStatusPort = hsp;
    enableForwardProxy = efp;
    forwardProxyType = fpt;
    forwardProxyAddress = fpa;
    forwardProxyPort = fpp;
    enableForwardProxyAuthentication = efpa;
    forwardProxyUsername = fpu;
    forwardProxyPassword = fppa;
    gfwlistUrl = glu;
    updateUserAgent = uua;
    filterKeyword = fkw;
    maximumSubscribe = ms;
    fingerprint = fp;
    enableTrojanAPI = eta;
    enableTrojanRouter = etr;
    trojanAPIPort = tap;
    trojanCertPath = tcp;
    trojanCipher = tc;
    trojanCipherTLS13 = tct13;
}

void ConfigHelper::setSystemProxySettings(QString mode)
{
   systemProxyMode = mode;
   settings->setValue("SystemProxyMode", QVariant(systemProxyMode));
}

void ConfigHelper::setTrojanOn(bool on)
{
    trojanOn = on;
    settings->setValue("TrojanOn", trojanOn);
}

void ConfigHelper::setAutoUpdateSubscribes(bool update)
{
    autoUpdateSubscribes = update;
    settings->setValue("AutoUpdateSubscribes", QVariant(autoUpdateSubscribes));
}

void ConfigHelper::setServerLoadBalance(bool enable)
{
    serverLoadBalance = enable;
    settings->setValue("ServerLoadBalance", QVariant(serverLoadBalance));
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
    trojanOn = settings->value("TrojanOn", QVariant(false)).toBool();
    toolbarStyle = settings->value("ToolbarStyle", QVariant(3)).toInt();
    logLevel = settings->value("LogLevel", QVariant(1)).toInt();
    startAtLogin = settings->value("StartAtLogin").toBool();
    autoUpdateSubscribes = settings->value("AutoUpdateSubscribes", QVariant(false)).toBool();
    systemProxyMode = settings->value("SystemProxyMode", QVariant("pac")).toString();
    serverLoadBalance = settings->value("ServerLoadBalance", QVariant(false)).toBool();
    hideWindowOnStartup = settings->value("HideWindowOnStartup").toBool();
    onlyOneInstace = settings->value("OnlyOneInstance", QVariant(true)).toBool();
    checkPortAvailability = settings->value("CheckPortAvailability", QVariant(true)).toBool();
    enableNotification = settings->value("EnableNotification", QVariant(true)).toBool();
    hideDockIcon = settings->value("HideDockIcon", QVariant(false)).toBool();
    showToolbar = settings->value("ShowToolbar", QVariant(true)).toBool();
    showFilterBar = settings->value("ShowFilterBar", QVariant(true)).toBool();
    nativeMenuBar = settings->value("NativeMenuBar", QVariant(false)).toBool();
    enableHttpMode = settings->value("EnableHttpMode", QVariant(true)).toBool();
    shareOverLan = settings->value("ShareOverLan", QVariant(false)).toBool();
    enableIpv6Support = settings->value("EnableIpv6Support", QVariant(false)).toBool();
    socks5Port = settings->value("Socks5LocalPort", QVariant(1080)).toInt();
    httpPort = settings->value("HttpLocalPort", QVariant(1081)).toInt();
    pacPort = settings->value("PACLocalPort", QVariant(8070)).toInt();
    haproxyStatusPort = settings->value("HaproxyStatusPort", QVariant(2080)).toInt();
    haproxyPort = settings->value("HaproxyPort", QVariant(7777)).toInt();
    enableForwardProxy = settings->value("ForwardProxy", QVariant(false)).toBool();
    forwardProxyType = settings->value("ForwardProxyType", QVariant(0)).toInt();
    forwardProxyAddress = settings->value("ForwardProxyAddress", QVariant("127.0.0.1")).toString();
    forwardProxyPort = settings->value("ForwardProxyPort", QVariant(1086)).toInt();
    enableForwardProxyAuthentication = settings->value("ForwardProxyAuthentication", QVariant(false)).toBool();
    forwardProxyUsername = settings->value("ForwardProxyUsername", QVariant("")).toString();
    forwardProxyPassword = settings->value("ForwardProxyPassword", QVariant("")).toString();
    gfwlistUrl = settings->value("GFWListUrl", QVariant(0)).toInt();
    updateUserAgent = settings->value("UpdateUserAgent", QVariant(QString("Trojan-Qt5/%1").arg(APP_VERSION))).toString();
    filterKeyword = settings->value("FilterKeyword", QVariant("")).toString();
    maximumSubscribe = settings->value("MaximumSubscribe", QVariant(0)).toInt();
    fingerprint = settings->value("Fingerprint", QVariant(0)).toInt();
    enableTrojanAPI = settings->value("EnableTrojanAPI", QVariant(true)).toBool();
    enableTrojanRouter = settings->value("EnableTrojanRouter", QVariant(false)).toBool();
    trojanAPIPort = settings->value("TrojanAPIPort", QVariant(10000)).toInt();
    trojanCertPath = settings->value("TrojanCertPath", QVariant("")).toString();
    trojanCipher = settings->value("TrojanCipher", QVariant("ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES128-GCM-SHA256:ECDHE-ECDSA-CHACHA20-POLY1305:ECDHE-RSA-CHACHA20-POLY1305:ECDHE-ECDSA-AES256-GCM-SHA384:ECDHE-RSA-AES256-GCM-SHA384:ECDHE-ECDSA-AES256-SHA:ECDHE-ECDSA-AES128-SHA:ECDHE-RSA-AES128-SHA:ECDHE-RSA-AES256-SHA:DHE-RSA-AES128-SHA:DHE-RSA-AES256-SHA:AES128-SHA:AES256-SHA:DES-CBC3-SHA")).toString();
    trojanCipherTLS13 = settings->value("TrojanCipherTLS13", QVariant("TLS_AES_128_GCM_SHA256:TLS_CHACHA20_POLY1305_SHA256:TLS_AES_256_GCM_SHA384")).toString();
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
