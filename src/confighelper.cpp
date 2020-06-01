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
#include "utils.h"

using namespace std;

ConfigHelper::ConfigHelper(const QString &configuration, QObject *parent) :
    QObject(parent),
    configFile(configuration)
{
    Utils::setPermisison(configFile);
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

    settings->setValue("AutoUpdateSubscribes", QVariant(autoUpdateSubscribes));
    settings->setValue("TrojanOn", QVariant(trojanOn));
    settings->setValue("SystemProxyMode", QVariant(systemProxyMode));
    settings->setValue("ServerLoadBalance", QVariant(serverLoadBalance));
    settings->setValue("ConfigVersion", QVariant(2.6));
    settings->setValue("ShowToolbar", QVariant(showToolbar));
    settings->setValue("ShowFilterBar", QVariant(showFilterBar));
    settings->setValue("GeneralSettings", QVariant(generalSettings));
    settings->setValue("InboundSettings", QVariant(inboundSettings));
    settings->setValue("OutboundSettings", QVariant(outboundSettings));
    settings->setValue("GraphSettings", QVariant(graphSettings));
    settings->setValue("RouterSettings", QVariant(routerSettings));
    settings->setValue("SubscribeSettings", QVariant(subscribeSettings));
    settings->setValue("TrojanSettings", QVariant(trojanSettings));
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
        Logger::error(QString("[Import] cannot open %1").arg(file));
        return;
    }
    if(!JSONFile.isReadable()) {
        qCritical() << "Error: cannot read " << file;
        Logger::error(QString("[Import] cannot read %1").arg(file));
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
        Logger::error(QString("[Import] JSON Document %1 is empty!").arg(file));
        return;
    }
    QJsonObject JSONObj = JSONDoc.object();
    QJsonArray CONFArray = JSONObj["configs"].toArray();
    if (CONFArray.isEmpty()) {
        Logger::error(QString("[Import] configs in %1 is empty!").arg(file));
        return;
    }

    for (QJsonArray::iterator it = CONFArray.begin(); it != CONFArray.end(); ++it) {
        QJsonObject json = (*it).toObject();
        TQProfile p;
        p.type = json["type"].toString();
        p.name = json["remarks"].toString();
        p.serverPort = json["server_port"].toInt();
        p.serverAddress = json["server"].toString();
        p.verifyCertificate = json["verify_certificate"].toBool();
        p.verifyHostname = json["verify_hostname"].toBool();
        p.method = json["method"].toString();
        p.password = json["password"].toString();
        p.uuid = json["uuid"].toString();
        p.protocol = json["protocol"].toString();
        p.protocolParam = json["protocolParam"].toString();
        p.obfs = json["obfs"].toString();
        p.obfsParam = json["obfsParam"].toString();
        p.plugin = json["plugin"].toString();
        p.pluginParam = json["pluginParam"].toString();
        p.sni = json["sni"].toString();
        p.reuseSession = json["reuse_session"].toBool();
        p.sessionTicket = json["session_ticket"].toBool();
        p.reusePort = json["reuse_port"].toBool();
        p.tcpFastOpen = json["tcp_fast_open"].toBool();
        p.mux = json["mux"].toBool();
        p.vmessSettings = json["vmessSettings"].toObject();
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
        json["method"] = QJsonValue(con->profile.method);
        json["password"] = QJsonValue(con->profile.password);
        json["uuid"] = QJsonValue(con->profile.uuid);
        json["protocol"] = QJsonValue(con->profile.password);
        json["protocolParam"] = QJsonValue(con->profile.password);
        json["obfs"] = QJsonValue(con->profile.obfs);
        json["obfsParam"] = QJsonValue(con->profile.obfsParam);
        json["plugin"] = QJsonValue(con->profile.plugin);
        json["pluginParam"] = QJsonValue(con->profile.pluginParam);
        json["sni"] = QJsonValue(con->profile.sni);
        json["reuse_session"] = QJsonValue(con->profile.reuseSession);
        json["session_ticket"] = QJsonValue(con->profile.sessionTicket);
        json["reuse_port"] = QJsonValue(con->profile.reusePort);
        json["tcp_fast_open"] = QJsonValue(con->profile.tcpFastOpen);
        json["mux"] = QJsonValue(con->profile.mux);
        json["vmessSettings"] = QJsonValue(con->profile.vmessSettings);
        confArray.append(QJsonValue(json));
    }

    QJsonObject JSONObj;
    JSONObj["configs"] = QJsonValue(confArray);

    QJsonDocument JSONDoc(JSONObj);

    QFile JSONFile(file);
    JSONFile.open(QIODevice::WriteOnly | QIODevice::Text);
    if (!JSONFile.isOpen()) {
        qCritical() << "Error: cannot open " << file;
        Logger::error(QString("[Export] cannot open %1").arg(file));
        return;
    }
    if(!JSONFile.isWritable()) {
        qCritical() << "Error: cannot write into " << file;
        Logger::error(QString("[Export] cannot write into %1").arg(file));
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

void ConfigHelper::importShadowrocketJson(ConnectionTableModel *model, const QString &file)
{
    QFile JSONFile(file);
    JSONFile.open(QIODevice::ReadOnly | QIODevice::Text);
    if (!JSONFile.isOpen()) {
        Logger::error(QString("[Import] cannot open %1").arg(file));
        return;
    }
    if(!JSONFile.isReadable()) {
        Logger::error(QString("[Import] cannot read %1").arg(file));
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
        Logger::error(QString("[Import] JSON Document %1 is empty!").arg(file));
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
        json["type"] = QJsonValue(Utils::toCamelCase(con->profile.type));
        json["title"] = QJsonValue(con->profile.name);
        json["host"] = QJsonValue(con->profile.serverAddress);
        json["port"] = QJsonValue(con->profile.serverPort);
        json["method"] = QJsonValue(con->profile.method);
        json["password"] = QJsonValue(con->profile.password);
        json["uuid"] = QJsonValue(con->profile.uuid);
        json["proto"] = QJsonValue(con->profile.protocol);
        json["protoParam"] = QJsonValue(con->profile.protocolParam);
        json["obfs"] = QJsonValue(con->profile.obfs);
        json["obfsParam"] = QJsonValue(con->profile.obfsParam);
        confArray.append(QJsonValue(json));
    }

    QJsonDocument JSONDoc(confArray);

    QFile JSONFile(file);
    JSONFile.open(QIODevice::WriteOnly | QIODevice::Text);
    if (!JSONFile.isOpen()) {
        Logger::error(QString("[Export] cannot open %1").arg(file));
        return;
    }
    if(!JSONFile.isWritable()) {
        Logger::error(QString("[Export] cannot write into %1").arg(file));
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
        uri += con->getURI(con->getProfile().type);
    }
    uri = uri.toUtf8().toBase64();

    QFile Subscribe(file);
    Subscribe.open(QIODevice::WriteOnly | QIODevice::Text);
    if (!Subscribe.isOpen()) {
        Logger::error(QString("[Export] cannot open %1").arg(file));
        return;
    }
    if(!Subscribe.isWritable()) {
        Logger::error(QString("[Export] cannot write into %1").arg(file));
        return;
    }

    Subscribe.write(uri.toUtf8().data());
    Subscribe.close();
}

QJsonObject ConfigHelper::generateVmessSettings()
{
    QJsonObject object;
    object["network"] = "tcp";
    QJsonObject tcp;
    QJsonObject tcpHeader;
    tcpHeader["type"] = "none";
    tcpHeader["request"] = "{\n    \"version\": \"1.1\",\n    \"method\": \"GET\",\n    \"path\": [],\n    \"headers\": {}\n}";
    tcpHeader["response"] = "{\n    \"version\": \"1.1\",\n    \"status\": \"200\",\n    \"reason\": \"OK\",\n    \"headers\": {}\n}";
    tcp["header"] = tcpHeader;
    object["tcp"] = QJsonValue(tcp);
    QJsonObject http;
    http["path"] = "/";
    object["http"] = QJsonValue(http);
    QJsonObject ws;
    ws["path"] = "/";
    object["ws"] = QJsonValue(ws);
    QJsonObject kcp;
    kcp["mtu"] = 1460;
    kcp["tti"] = 20;
    kcp["uplinkCapacity"] = 5;
    kcp["congestion"] = false;
    kcp["downlinkCapacity"] = 20;
    kcp["readBufferSize"] = 1;
    kcp["writeBufferSize"] = 1;
    QJsonObject kcpHeader;
    kcpHeader["type"] = "none";
    kcp["header"] = kcpHeader;
    object["kcp"] = kcp;
    QJsonObject tls;
    tls["enable"] = false;
    tls["allowInsecure"] = false;
    tls["allowInsecureCiphers"] = false;
    tls["serverName"] = "";
    object["tls"] = QJsonValue(tls);
    return object;
}

Connection* ConfigHelper::configJsonToConnection(const QString &file)
{
    QFile JSONFile(file);
    JSONFile.open(QIODevice::ReadOnly | QIODevice::Text);
    if (!JSONFile.isOpen()) {
        Logger::error(QString("[Connection] cannot open %1").arg(file));
    }
    if(!JSONFile.isReadable()) {
        Logger::error(QString("[Connection] cannot read %1").arg(file));
    }

    QJsonParseError pe;
    QJsonDocument JSONDoc = QJsonDocument::fromJson(JSONFile.readAll(), &pe);
    JSONFile.close();
    if (pe.error != QJsonParseError::NoError) {
        qCritical() << pe.errorString();
    }
    if (JSONDoc.isEmpty()) {
        Logger::error(QString("[Connection] JSON Document %1 is empty!").arg(file));
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
    configObj["local_addr"] = inboundSettings["enableIpv6Support"].toBool() ? (inboundSettings["shareOverLan"].toBool() ? "::" : "::1") : (inboundSettings["shareOverLan"].toBool() ? "0.0.0.0" : "127.0.0.1");
    configObj["local_port"] = inboundSettings["socks5LocalPort"].toInt();
    configObj["remote_addr"] = profile.serverAddress;
    configObj["remote_port"] = profile.serverPort;
    configObj["buffer_size"] = trojanSettings["bufferSize"];
    QJsonArray passwordArray;
    passwordArray.append(profile.password);
    configObj["password"] = QJsonValue(passwordArray);
    configObj["log_level"] = generalSettings["logLevel"].toInt();
    configObj["log_file"] = Utils::getLogDir() + "/core.log";
    QJsonObject ssl;
    ssl["verify"] = profile.verifyCertificate;
    ssl["verify_hostname"] = profile.verifyHostname;
    ssl["cert"] = trojanSettings["trojanCertPath"].toString();
    ssl["cipher"] = trojanSettings["trojanCipher"].toString();
    ssl["cipher_tls13"] = trojanSettings["trojanCipherTLS13"].toString();
    ssl["sni"] = profile.sni;
    QJsonArray alpnArray;
    alpnArray.append("h2");
    alpnArray.append("http/1.1");
    ssl["alpn"] = QJsonValue(alpnArray);
    ssl["reuse_session"] = profile.reuseSession;
    ssl["session_ticket"] = profile.sessionTicket;
    ssl["curves"] = "";
    ssl["fingerprint"] = parseTLSFingerprint(trojanSettings["fingerprint"].toInt());
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
    mux["concurrency"] = profile.muxConcurrency;
    configObj["mux"] = QJsonValue(mux);
    QJsonObject websocket;
    websocket["enabled"] = profile.websocket;
    websocket["path"] = profile.websocketPath;
    websocket["hostname"] = profile.websocketHostname;
    websocket["obfuscation_password"] = profile.websocketObfsPassword;
    websocket["double_tls"] = profile.websocketDoubleTLS;
    configObj["websocket"] = QJsonValue(websocket);
    QJsonObject router;
    router["enabled"] = trojanSettings["enableTrojanRouter"].toBool();
    if (router["enabled"].toBool()) {
        router["direct"] = appendJsonArray(routerSettings["domain"].toObject()["direct"].toArray(), routerSettings["ip"].toObject()["direct"].toArray());
        router["proxy"] = appendJsonArray(routerSettings["domain"].toObject()["proxy"].toArray(), routerSettings["ip"].toObject()["proxy"].toArray());
        router["block"] = appendJsonArray(routerSettings["domain"].toObject()["block"].toArray(), routerSettings["ip"].toObject()["block"].toArray());
        router["default_policy"] = "proxy";
        router["domain_strategy"] = parseDomainStrategy(routerSettings["domainStrategy"].toString());
    }
    configObj["router"] = router;
    QJsonObject api;
    api["enabled"] = trojanSettings["enableTrojanAPI"].toBool();
    api["api_addr"] = "127.0.0.1";
    api["api_port"] = trojanSettings["trojanAPIPort"].toInt();
    configObj["api"] = QJsonValue(api);
    QJsonObject forward_proxy;
    forward_proxy["enabled"] = outboundSettings["forwardProxy"].toBool();
    forward_proxy["proxy_addr"] = outboundSettings["forwardProxyAddress"].toString();
    forward_proxy["proxy_port"] = outboundSettings["forwardProxyPort"].toInt();
    if (outboundSettings["forwardProxyAuthentication"].toBool()) {
        forward_proxy["username"] = outboundSettings["forwardProxyUsername"].toString();
        forward_proxy["password"] = outboundSettings["forwardProxyPassword"].toString();
    }
    configObj["forward_proxy"] = QJsonValue(forward_proxy);
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
        Logger::error(QString("cannot open %1").arg(file));
        return;
    }
    if(!JSONFile.isWritable()) {
        Logger::error(QString("cannot write into %1").arg(file));
        return;
    }

    JSONFile.write(JSONDoc.toJson());
    JSONFile.close();

}

void ConfigHelper::generateV2rayJson(TQProfile &profile)
{
    QJsonObject configObj;
    QJsonObject log;
    log["access"] = Utils::getLogDir() + "/core.log";
    log["error"] = Utils::getLogDir() + "/core.log";
    log["level"] = "info";
    configObj["log"] = log;
    QJsonObject stats;
    configObj["stats"] = stats;
    QJsonObject api;
    api["tag"] = "api";
    QJsonArray apiServices;
    apiServices.append("StatsService");
    api["services"] = apiServices;
    configObj["api"] = api;
    QJsonObject policy;
    QJsonObject system;
    system["statsInboundUplink"] = true;
    system["statsInboundDownlink"] = true;
    policy["system"] = system;
    configObj["policy"] = policy;
    QJsonObject routing;
    routing["domainStrategy"] = routerSettings["domainStrategy"].toString();
    QJsonArray rules;
    QJsonObject apiRule;
    apiRule["type"] = "field";
    apiRule["outboundTag"] = "api";
    QJsonArray apiRuleInboundTag;
    apiRuleInboundTag.append("api_in");
    apiRule["inboundTag"] = apiRuleInboundTag;
    QJsonObject direct;
    direct["type"] = "field";
    if (routerSettings["domain"].toObject()["direct"].toArray().size() != 0)
        direct["domain"] = routerSettings["domain"].toObject()["direct"].toArray();
    if (routerSettings["domain"].toObject()["direct"].toArray().size() != 0)
        direct["ip"] = routerSettings["ip"].toObject()["direct"].toArray();
    direct["outboundTag"] = "direct";
    QJsonObject proxy;
    proxy["type"] = "field";
    if (routerSettings["domain"].toObject()["proxy"].toArray().size() != 0)
        proxy["domain"] = routerSettings["domain"].toObject()["proxy"].toArray();
    if (routerSettings["ip"].toObject()["proxy"].toArray().size() != 0)
        proxy["ip"] = routerSettings["ip"].toObject()["proxy"].toArray();
    proxy["outboundTag"] = "proxy";
    QJsonObject block;
    block["type"] = "field";
    if (routerSettings["ip"].toObject()["block"].toArray().size() != 0)
     block["domain"] = routerSettings["domain"].toObject()["block"].toArray();
    if (routerSettings["ip"].toObject()["block"].toArray().size() != 0)
        block["ip"] = routerSettings["ip"].toObject()["block"].toArray();
    block["outboundTag"] = "block";
    rules.append(apiRule);
    if (routerSettings["domain"].toObject()["direct"].toArray().size() != 0 || routerSettings["ip"].toObject()["direct"].toArray().size() != 0)
        rules.append(direct);
    if (routerSettings["domain"].toObject()["proxy"].toArray().size() != 0 || routerSettings["ip"].toObject()["proxy"].toArray().size() != 0)
        rules.append(proxy);
    if (routerSettings["domain"].toObject()["block"].toArray().size() != 0 || routerSettings["ip"].toObject()["block"].toArray().size() != 0)
        rules.append(block);
    routing["rules"] = rules;
    configObj["routing"] = routing;
    QJsonArray inboundsArray;
    QJsonObject socks;
    QJsonObject socksSettings;
    socks["port"] = inboundSettings["socks5LocalPort"];
    socks["protocol"] = "socks";
    socksSettings["udp"] = true;
    socksSettings["auth"] = "noauth";
    socksSettings["ip"] = getInboundSettings()["enableIpv6Support"].toBool() ? (getInboundSettings()["shareOverLan"].toBool() ? "::" : "::1") : (getInboundSettings()["shareOverLan"].toBool() ? "0.0.0.0" : "127.0.0.1");
    socks["settings"] = socksSettings;
    socks["tag"] = "inbound";
    QJsonObject apiIn;
    apiIn["listen"] = "127.0.0.1";
    apiIn["port"] = trojanSettings["trojanAPIPort"].toInt();
    apiIn["protocol"] = "dokodemo-door";
    QJsonObject apiInSettings;
    apiInSettings["address"] = "127.0.0.1";
    apiIn["settings"] = apiInSettings;
    apiIn["tag"] = "api_in";
    inboundsArray.append(apiIn);
    inboundsArray.append(socks);
    configObj["inbounds"] = inboundsArray;
    QJsonArray outboundsArray;
    QJsonObject outbound;
    outbound["protocol"] = "vmess";
    outbound["sendThrough"] = "0.0.0.0";
    outbound["tag"] = "proxy";
    QJsonArray vnextArray;
    QJsonObject vnext;
    vnext["address"] = profile.serverAddress;
    vnext["port"] = profile.serverPort;
    QJsonArray usersArray;
    QJsonObject users;
    users["id"] = profile.uuid;
    users["alterId"] = profile.alterID;
    users["security"] = profile.security;
    usersArray.append(users);
    vnext["users"] = usersArray;
    vnextArray.append(vnext);
    QJsonObject streamSettings;
    streamSettings["network"] = profile.vmessSettings["network"].toString();
    streamSettings["security"] = profile.vmessSettings["tls"].toObject()["enable"].toBool() ? "tls" : "none";
    if (streamSettings["network"] == "tcp") {
        QJsonObject tcpSettings;
        QJsonObject tcpHeader;
        tcpHeader["type"] = profile.vmessSettings["tcp"].toObject()["header"].toObject()["type"].toString();
        tcpHeader["request"] = QJsonDocument::fromJson(profile.vmessSettings["tcp"].toObject()["header"].toObject()["request"].toString().toUtf8().data()).object();
        tcpHeader["response"] = QJsonDocument::fromJson(profile.vmessSettings["tcp"].toObject()["header"].toObject()["response"].toString().toUtf8().data()).object();
        tcpSettings["header"] = tcpHeader;
        streamSettings["tcpSettings"] = tcpSettings;
    } else if (streamSettings["network"] == "kcp") {
        QJsonObject kcpSettings;
        kcpSettings["mtu"] = profile.vmessSettings["kcp"].toObject()["mtu"].toInt();
        kcpSettings["tti"] = profile.vmessSettings["kcp"].toObject()["tti"].toInt();
        kcpSettings["uplinkCapacity"] = profile.vmessSettings["kcp"].toObject()["uplinkCapacity"].toInt();
        kcpSettings["downlinkCapacity"] = profile.vmessSettings["kcp"].toObject()["downlinkCapacity"].toInt();
        kcpSettings["congestion"] = profile.vmessSettings["kcp"].toObject()["congestion"].toBool();
        kcpSettings["readBufferSize"] = profile.vmessSettings["kcp"].toObject()["readBufferSize"].toInt();
        kcpSettings["downlinkCapacity"] = profile.vmessSettings["kcp"].toObject()["downlinkCapacity"].toInt();
        QJsonObject kcpHeader;
        kcpHeader["type"] = profile.vmessSettings["kcp"].toObject()["header"].toObject()["type"].toString();
        kcpSettings["header"] = kcpHeader;
        streamSettings["kcpSettings"] = kcpSettings;
    } else if (streamSettings["network"] == "ws") {
        QJsonObject wsSettings;
        wsSettings["path"] = profile.vmessSettings["ws"].toObject()["path"];
        wsSettings["headers"] = profile.vmessSettings["ws"].toObject()["header"];
        streamSettings["wsSettings"] = wsSettings;
    } else if (streamSettings["network"] == "http") {
        QJsonObject httpSettings;
        httpSettings["host"] = profile.vmessSettings["http"].toObject()["host"].toArray();
        httpSettings["path"] = profile.vmessSettings["http"].toObject()["path"].toString();
        streamSettings["httpSettings"] = httpSettings;
    } else if (streamSettings["network"] == "quic") {
        QJsonObject quicSettings;
        quicSettings["security"] = profile.vmessSettings["quic"].toObject()["security"].toString();
        quicSettings["key"] = profile.vmessSettings["quic"].toObject()["key"].toString();
        QJsonObject quicHeader;
        quicHeader["type"] = profile.vmessSettings["quic"].toObject()["header"].toObject()["type"].toString();
        streamSettings["quicSettings"] = quicSettings;
    }
    QJsonObject tlsSettings;
    tlsSettings["allowInsecure"] = profile.vmessSettings["tls"].toObject()["allowInsecure"].toBool();
    tlsSettings["allowInsecureCiphers"] = profile.vmessSettings["tls"].toObject()["allowInsecureCiphers"].toBool();
    tlsSettings["serverName"] = profile.vmessSettings["tls"].toObject()["serverName"].toString();
    tlsSettings["alpn"] = profile.vmessSettings["tls"].toObject()["alpn"].toArray();
    streamSettings["tlsSettings"] = tlsSettings;
    QJsonObject freedom;
    freedom["protocol"] = "freedom";
    freedom["sendThrough"] = "0.0.0.0";
    QJsonObject settingsFreedom;
    settingsFreedom["domainStrategy"] = "AsIs";
    settingsFreedom["redirect"] = ":0";
    settingsFreedom["userLevel"] = 0;
    freedom["settings"] = settingsFreedom;
    freedom["streamSettings"] = QJsonObject();
    freedom["tag"] = "direct";
    QJsonObject blackhole;
    blackhole["protocol"] = "blackhole";
    blackhole["sendThrough"] = "0.0.0.0";
    QJsonObject response;
    response["type"] = "none";
    QJsonObject settingsBlackHole;
    settingsBlackHole["response"] = response;
    blackhole["settings"] = settingsBlackHole;
    blackhole["streamSettings"] = QJsonObject();
    blackhole["tag"] = "block";
    QJsonObject vmessOutSettings;
    vmessOutSettings["vnext"] = vnextArray;
    QJsonObject mux;
    mux["enabled"] = profile.mux;
    mux["concurrency"] = profile.muxConcurrency;
    outbound["mux"] = mux;
    outbound["settings"] = vmessOutSettings;
    outbound["streamSettings"] = streamSettings;
    outboundsArray.append(outbound);
    outboundsArray.append(freedom);
    outboundsArray.append(blackhole);
    configObj["outbounds"] = outboundsArray;

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
        Logger::error(QString("cannot open %1").arg(file));
        return;
    }
    if(!JSONFile.isWritable()) {
        Logger::error(QString("cannot write into %1").arg(file));
        return;
    }

    JSONFile.write(JSONDoc.toJson());
    JSONFile.close();
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
    text.replace(QString("__STATUS__"), QString("%1:%2").arg(getInboundSettings()["enableIpv6Support"].toBool() ? (getInboundSettings()["shareOverLan"].toBool() ? "::" : "::1") : (getInboundSettings()["shareOverLan"].toBool() ? "0.0.0.0" : "127.0.0.1")).arg(QString::number(inboundSettings["haproxyStatusPort"].toInt())));
    text.replace(QString("__TROJAN__"), QString("%1:%2").arg(getInboundSettings()["enableIpv6Support"].toBool() ? (getInboundSettings()["shareOverLan"].toBool() ? "::" : "::1") : (getInboundSettings()["shareOverLan"].toBool() ? "0.0.0.0" : "127.0.0.1")).arg(QString::number(inboundSettings["haproxyPort"].toInt())));
    text.replace(QString("__MODE__"), QString("%1"));
    file.seek(0); // go to the beginning of the file
    file.write(text.toUtf8()); // write the new text back to the file
    file.close(); // close the file handle.
}

QJsonArray ConfigHelper::appendJsonArray(QJsonArray array1, QJsonArray array2)
{
    for (QJsonValue value : array2) {
        array1.append(value);
    }

    return array1;
}

QString ConfigHelper::parseDomainStrategy(QString ds) const
{
    if (ds == "AsIs")
        return "as_is";
    else if (ds == "IPIfNonMatch")
        return "ip_if_nonmatch";
    else if (ds == "IPOnDemand")
        return "ip_on_demand";
    else
        return "";
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

QJsonObject ConfigHelper::getGeneralSettings() const
{
    return generalSettings;
}

QJsonObject ConfigHelper::getInboundSettings() const
{
    return inboundSettings;
}

QJsonObject ConfigHelper::getOutboundSettings() const
{
    return outboundSettings;
}

QJsonObject ConfigHelper::getSubscribeSettings() const
{
    return subscribeSettings;
}

QJsonObject ConfigHelper::getGraphSettings() const
{
    return graphSettings;
}

QJsonObject ConfigHelper::getRouterSettings() const
{
    return routerSettings;
}

QJsonObject ConfigHelper::getTrojanSettings() const
{
    return trojanSettings;
}

void ConfigHelper::setGeneralSettings(QJsonObject gs, QJsonObject is, QJsonObject os, QJsonObject ss, QJsonObject fs, QJsonObject rs, QJsonObject ts)
{
    if (gs["toolbarStyle"].toInt() != generalSettings["toolbarStyle"].toInt()) {
        emit toolbarStyleChanged(static_cast<Qt::ToolButtonStyle>(gs["toolbarStyle"].toInt()));
    }
    generalSettings = gs;
    inboundSettings = is;
    outboundSettings = os;
    subscribeSettings = ss;
    graphSettings = fs;
    routerSettings = rs;
    trojanSettings = ts;
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
    autoUpdateSubscribes = settings->value("AutoUpdateSubscribes", QVariant(false)).toBool();
    systemProxyMode = settings->value("SystemProxyMode", QVariant("pac")).toString();
    serverLoadBalance = settings->value("ServerLoadBalance", QVariant(false)).toBool();
    showToolbar = settings->value("ShowToolbar", QVariant(true)).toBool();
    showFilterBar = settings->value("ShowFilterBar", QVariant(true)).toBool();
    QJsonObject gtemp;
    gtemp["theme"] = "Fusion";
    gtemp["darkTheme"] = false;
    gtemp["toolbarStyle"] = 3;
    gtemp["logLevel"] = 1;
    gtemp["systemTrayMaximumServer"] = 0;
    gtemp["startAtLogin"] = false;
    gtemp["hideWindowOnStartup"] = false;
    gtemp["onlyOneInstace"] = true;
    gtemp["checkPortAvailability"] = true;
    gtemp["enableNotification"] = true;
    gtemp["hideDockIcon"] = false;
    gtemp["showToolbar"] = true;
    gtemp["showFilterBar"] = true;
    gtemp["nativeMenuBar"] = false;
    gtemp["showAiportAndDonation"] = true;
    generalSettings = settings->value("GeneralSettings", QVariant(gtemp)).toJsonObject();
    QJsonObject itemp;
    itemp["enableHttpMode"] = true;
    itemp["shareOverLan"] = false;
    itemp["enableIpv6Support"] = false;
    itemp["socks5LocalPort"] = 51837;
    itemp["httpLocalPort"] = 58591;
    itemp["pacLocalPort"] = 8070;
    itemp["haproxyStatusPort"] = 2080;
    itemp["haproxyPort"] = 7777;
    inboundSettings = settings->value("InboundSettings", QVariant(itemp)).toJsonObject();
    QJsonObject otemp;
    otemp["forwardProxy"] = false;
    otemp["forwardProxyType"] = 0;
    otemp["forwardProxyAddress"] = "127.0.0.1";
    otemp["forwardProxyPort"] = 1086;
    otemp["forwardProxyAuthentication"] = false;
    otemp["forwardProxyUsername"] = "";
    otemp["forwardProxyPassword"] = "";
    outboundSettings = settings->value("OutboundSettings", QVariant(otemp)).toJsonObject();
    QJsonObject stemp;
    stemp["gfwListUrl"] = 2;
    stemp["updateUserAgent"] = QString("Trojan-Qt5/%1").arg(APP_VERSION);
    stemp["filterKeyword"] = "";
    stemp["maximumSubscribe"] = 0;
    stemp["autoFetchGroupName"] = true;
    stemp["overwriteAllowInsecure"] = false;
    stemp["overwriteAllowInsecureCiphers"] = false;
    stemp["overwriteTcpFastOpen"] = false;
    subscribeSettings = settings->value("SubscribeSettings", QVariant(stemp)).toJsonObject();
    QJsonObject ftemp;
    ftemp["downloadSpeedColor"] = QColor::fromRgb(134, 196, 63).name();
    ftemp["uploadSpeedColor"] = QColor::fromRgb(50, 153, 255).name();
    graphSettings = settings->value("GraphSettings", QVariant(ftemp)).toJsonObject();
    QJsonObject rtemp;
    rtemp["domainStrategy"] = "AsIs";
    routerSettings = settings->value("RouterSettings", QVariant(rtemp)).toJsonObject();
    QJsonObject ttemp;
    ttemp["fingerprint"] = 2;
    ttemp["enableTrojanAPI"] = true;
    ttemp["enableTrojanRouter"] = false;
    ttemp["trojanAPIPort"] = 57721;
    ttemp["trojanCertPath"] = "";
    ttemp["trojanCipher"] = "ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES128-GCM-SHA256:ECDHE-ECDSA-CHACHA20-POLY1305:ECDHE-RSA-CHACHA20-POLY1305:ECDHE-ECDSA-AES256-GCM-SHA384:ECDHE-RSA-AES256-GCM-SHA384:ECDHE-ECDSA-AES256-SHA:ECDHE-ECDSA-AES128-SHA:ECDHE-RSA-AES128-SHA:ECDHE-RSA-AES256-SHA:DHE-RSA-AES128-SHA:DHE-RSA-AES256-SHA:AES128-SHA:AES256-SHA:DES-CBC3-SHA";
    ttemp["trojanCipherTLS13"] = "TLS_AES_128_GCM_SHA256:TLS_CHACHA20_POLY1305_SHA256:TLS_AES_256_GCM_SHA384";
    ttemp["bufferSize"] = 32;
    trojanSettings = settings->value("TrojanSettings", QVariant(ttemp)).toJsonObject();
}

void ConfigHelper::checkProfileDataUsageReset(TQProfile &profile)
{
    QDate currentDate = QDate::currentDate();
    if (profile.nextResetDate.isNull()){//invalid if the config.ini is old
        //the default reset day is 1 of every month
        profile.nextResetDate = QDate(currentDate.year(), currentDate.month(), 1);
        qDebug() << "config.ini upgraded from old version";
        profile.totalDownloadUsage += profile.currentDownloadUsage;//we used to use received
        profile.totalUploadUsage += profile.currentUploadUsage; //we used to use sent
    }

    if (profile.nextResetDate < currentDate) {//not <= because that'd casue multiple reset on this day
        profile.currentDownloadUsage = 0;
        profile.currentUploadUsage = 0;
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
            "X-GNOME-Autostart-enabled=true\n"
            "StartWMClass=trojan-qt5"
            "MimeType=x-scheme-handler/ss;x-scheme-handler/ssr;x-scheme-handler/vmess;x-scheme-handler/trojan;x-scheme-handler/snell;x-scheme-handler/trojan-qt5;x-scheme-handler/felix;\n");
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

    if (this->getGeneralSettings()["startAtLogin"].toBool()) {
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
