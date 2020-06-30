#include "confighelper.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include <QMessageBox>
#include <cstdlib>

#include "logger.h"
#include "v2raystruct.h"
#if defined (Q_OS_WIN)
    #include "yaml-cpp/yaml.h"
#else
    #include <yaml-cpp/yaml.h>
#endif
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

QStringList ConfigHelper::jsonArraytoStringlist(const QJsonArray &array)
{
    QStringList list;
    for (const QJsonValue &val: array) {
        QString data = val.toString();
        list.append(data);
    }
    return list;
}

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
    settings->setValue("TestSettings", QVariant(testSettings));
    settings->setValue("GraphSettings", QVariant(graphSettings));
    settings->setValue("RouterSettings", QVariant(routerSettings));
    settings->setValue("SubscribeSettings", QVariant(subscribeSettings));
    settings->setValue("TrojanSettings", QVariant(trojanSettings));
}

void ConfigHelper::onConfigUpdateFromOldVersion()
{
    QMessageBox::critical(NULL, tr("Failed to start Trojan-Qt5"), tr("Your config.ini was upgraded from old version of Trojan-Qt5.\nA clean install is required.\nCheckout wiki for instructions"));
    exit(1);
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
        p.muxConcurrency = !json["mux_concurrency"].isNull() ? json["mux_concurrency"].toInt() : 8;
        p.muxIdleTimeout = !json["mux_idle_timeout"].isNull() ? json["mux_idle_timeout"].toInt() : 60;
        p.vmessSettings = !json["vmessSettings"].isNull() ? parseVmessSettings(json["vmessSettings"].toObject()) : parseVmessSettings(QJsonObject());
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
        json["method"] = QJsonValue(con->profile.method);
        json["password"] = QJsonValue(con->profile.password);
        json["uuid"] = QJsonValue(con->profile.uuid);
        json["protocol"] = QJsonValue(con->profile.protocol);
        json["protocolParam"] = QJsonValue(con->profile.protocolParam);
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
        json["mux_concurrency"] = QJsonValue(con->profile.muxConcurrency);
        json["mux_idle_timeout"] = QJsonValue(con->profile.muxConcurrency);
        json["vmessSettings"] = QJsonValue(exportVmessSettings(con->profile.vmessSettings));
        confArray.append(QJsonValue(json));
    }

    QJsonObject JSONObj;
    JSONObj["configs"] = QJsonValue(confArray);

    QJsonDocument JSONDoc(JSONObj);

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

void ConfigHelper::importConfigYaml(ConnectionTableModel *model, const QString &file)
{
    YAML::Node config = YAML::LoadFile(file.toStdString());
    const YAML::Node& proxies = config["Proxy"];
    for (std::size_t i = 0; i < proxies.size(); i++) {
        const YAML::Node& proxy = proxies[i];
        TQProfile p;
        QString type = QString::fromStdString(proxy["type"].as<string>());
        p.name = QString::fromStdString(proxy["name"].as<string>());
        p.serverAddress = QString::fromStdString(proxy["server"].as<string>());
        p.serverPort = proxy["port"].as<int>();
        if (type == "socks5") {
            try {
                p.username = QString::fromStdString(proxy["username"].as<string>());
            } catch (...) {}
            try {
                p.password = QString::fromStdString(proxy["password"].as<string>());
            } catch (...) {}
        } else if (type == "http") {
            try {
                p.username = QString::fromStdString(proxy["username"].as<string>());
            } catch (...) {}
            try {
                p.password = QString::fromStdString(proxy["password"].as<string>());
            } catch (...) {}
        } else if (type == "ssr") {
            p.password = QString::fromStdString(proxy["password"].as<string>());
            p.method = QString::fromStdString(proxy["cipher"].as<string>());
            p.protocol = QString::fromStdString(proxy["protocol"].as<string>());
            p.protocolParam = QString::fromStdString(proxy["protocolparam"].as<string>());
            p.obfs = QString::fromStdString(proxy["obfs"].as<string>());
            p.obfsParam = QString::fromStdString(proxy["obfsparam"].as<string>());
        } else if (type == "trojan") {
            p.password = QString::fromStdString(proxy["password"].as<string>());
            try {
                p.verifyCertificate = !proxy["skip-cert-verify"].as<bool>();
            } catch (...) {}
            try {
                p.sni = QString::fromStdString(proxy["sni"].as<string>());
            } catch (...) {}
        } else if (type == "snell") {
            p.password = QString::fromStdString(proxy["psk"].as<string>());
        }
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

void ConfigHelper::exportSubscribe(const ConnectionTableModel &model, const QString &file)
{
    QString uri;
    int size = model.rowCount();
    for (int i = 0; i < size; ++i) {
        Connection *con = model.getItem(i)->getConnection();
        uri += con->getURI(con->getProfile().type);
        uri += "\r\n";
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

VmessSettings ConfigHelper::parseVmessSettings(const QJsonObject &settings)
{
    VmessSettings vmessSettings;
    if (settings.isEmpty())
        return vmessSettings;
    vmessSettings.network = settings["network"].toString();
    vmessSettings.tcp.type = settings["tcp"].toObject()["header"].toObject()["type"].toString();
    vmessSettings.tcp.request = settings["tcp"].toObject()["header"].toObject()["request"].toString();
    vmessSettings.tcp.response = settings["tcp"].toObject()["header"].toObject()["response"].toString();
    vmessSettings.http.host = jsonArraytoStringlist(settings["http"].toObject()["host"].toArray());
    vmessSettings.http.path = settings["http"].toObject()["path"].toString();
    vmessSettings.ws.header = Utils::convertQJsonObject(settings["ws"].toObject()["header"].toObject());
    vmessSettings.ws.path = settings["ws"].toObject()["path"].toString();
    vmessSettings.kcp.mtu = settings["kcp"].toObject()["mtu"].toInt();
    vmessSettings.kcp.tti = settings["kcp"].toObject()["tti"].toInt();
    vmessSettings.kcp.uplinkCapacity = settings["kcp"].toObject()["uplinkCapacity"].toInt();
    vmessSettings.kcp.downlinkCapacity = settings["kcp"].toObject()["downlinkCapacity"].toInt();
    vmessSettings.kcp.congestion = settings["kcp"].toObject()["congestion"].toBool();
    vmessSettings.kcp.readBufferSize = settings["kcp"].toObject()["readBufferSize"].toInt();
    vmessSettings.kcp.writeBufferSize = settings["kcp"].toObject()["writeBufferSize"].toInt();
    vmessSettings.kcp.type = settings["kcp"].toObject()["header"].toObject()["type"].toString();
    vmessSettings.kcp.seed = settings["kcp"].toObject()["seed"].toString();
    vmessSettings.quic.security = settings["quic"].toObject()["security"].toString();
    vmessSettings.quic.key = settings["quic"].toObject()["key"].toString();
    vmessSettings.quic.type = settings["quic"].toObject()["header"].toObject()["type"].toString();
    vmessSettings.tls.enable = settings["tls"].toObject()["enable"].toBool();
    vmessSettings.tls.allowInsecure = settings["tls"].toObject()["allowInsecure"].toBool();
    vmessSettings.tls.allowInsecureCiphers = settings["tls"].toObject()["allowInsecureCiphers"].toBool();
    vmessSettings.tls.serverName = settings["tls"].toObject()["serverName"].toString();
    vmessSettings.tls.alpn = jsonArraytoStringlist(settings["tls"].toObject()["alpn"].toArray());
    return vmessSettings;
}

QJsonObject ConfigHelper::exportVmessSettings(const VmessSettings &settings)
{
    QJsonObject object;
    object["network"] = settings.network;
    QJsonObject tcp;
    QJsonObject tcpHeader;
    tcpHeader["request"] = settings.tcp.request;
    tcpHeader["response"] = settings.tcp.response;
    tcpHeader["type"] = settings.tcp.type;
    tcp["header"] = tcpHeader;
    object["tcp"] = tcp;
    QJsonObject http;
    http["host"] = QJsonArray::fromStringList(settings.http.host);
    http["path"] = settings.http.path;
    object["http"] = http;
    QJsonObject ws;
    ws["header"] = Utils::convertWsHeader(settings.ws.header);
    ws["path"] = settings.ws.path;
    object["ws"] = ws;
    QJsonObject kcp;
    kcp["mtu"] = settings.kcp.mtu;
    kcp["tti"] = settings.kcp.tti;
    kcp["uplinkCapacity"] = settings.kcp.uplinkCapacity;
    kcp["downlinkCapacity"] = settings.kcp.downlinkCapacity;
    kcp["congestion"] = settings.kcp.congestion;
    kcp["readBufferSize"] = settings.kcp.readBufferSize;
    kcp["writeBufferSize"] = settings.kcp.writeBufferSize;
    QJsonObject kcpHeader;
    kcpHeader["type"] = settings.kcp.type;
    kcp["header"] = kcpHeader;
    kcp["seed"] = settings.kcp.seed;
    object["kcp"] = kcp;
    QJsonObject quic;
    quic["security"] = settings.quic.security;
    quic["key"] = settings.quic.key;
    QJsonObject quicHeader;
    quicHeader["type"] = settings.quic.type;
    quic["header"] = quicHeader;
    object["quic"] = quic;
    QJsonObject tls;
    tls["enable"] = settings.tls.enable;
    tls["allowInsecure"] = settings.tls.allowInsecure;
    tls["allowInsecureCiphers"] = settings.tls.allowInsecureCiphers;
    if (!settings.tls.serverName.isEmpty())
        tls["serverName"] = settings.tls.serverName;
    if (settings.tls.alpn.size() != 0)
        tls["alpn"] = QJsonArray::fromStringList(settings.tls.alpn);
    object["tls"] = tls;

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
    p.reuseSession = configObj["ssl"].toObject()["reuse_session"].toBool();
    p.sessionTicket = configObj["ssl"].toObject()["session_ticket"].toBool();
    p.reusePort = configObj["tcp"].toObject()["reuse_port"].toBool();
    p.tcpFastOpen = configObj["tcp"].toObject()["fast_open"].toBool();
    p.mux = configObj["mux"].toObject()["enabled"].toBool();
    Connection *con = new Connection(p, this);
    return con;
}

void ConfigHelper::generateSocks5HttpJson(QString type, TQProfile &profile)
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
    routing["domainStrategy"] = routerSettings.domainStrategy;
    QJsonArray rules;
    QJsonObject apiRule;
    apiRule["type"] = "field";
    apiRule["outboundTag"] = "api";
    QJsonArray apiRuleInboundTag;
    apiRuleInboundTag.append("api_in");
    apiRule["inboundTag"] = apiRuleInboundTag;
    QJsonObject bypassBittorrent;
    if (outboundSettings.bypassBittorrent) {
        bypassBittorrent["outboundTag"] = "direct";
        QJsonArray protocol;
        protocol.append("bittorrent");
        bypassBittorrent["protocol"] = protocol;
        bypassBittorrent["type"] = "field";
    }
    QJsonObject bypassChinaMainlandIP;
    QJsonObject bypassChinaMainlandDo;
    if (outboundSettings.bypassChinaMainland) {
        QJsonArray bypassIPArr;
        bypassIPArr.append("geoip:cn");
        bypassChinaMainlandIP["ip"] = bypassIPArr;
        bypassChinaMainlandIP["outboundTag"] = "direct";
        bypassChinaMainlandIP["type"] = "field";
        QJsonArray bypassDoArr;
        bypassDoArr.append("geosite:cn");
        bypassChinaMainlandDo["domain"] = bypassDoArr;
        bypassChinaMainlandDo["outboundTag"] = "direct";
        bypassChinaMainlandDo["type"] = "field";
    }
    QJsonObject directDO;
    directDO["type"] = "field";
    QJsonObject directIP;
    directIP["type"] = "field";
    if (routerSettings.domainDirect.size() != 0)
        directDO["domain"] = QJsonArray::fromStringList(routerSettings.domainDirect);
    if (routerSettings.ipDirect.size() != 0)
        directIP["ip"] = QJsonArray::fromStringList(routerSettings.ipDirect);
    directDO["outboundTag"] = "direct";
    directIP["outboundTag"] = "direct";
    QJsonObject proxyDO;
    proxyDO["type"] = "field";
    QJsonObject proxyIP;
    proxyIP["type"] = "field";
    if (routerSettings.domainProxy.size() != 0)
        proxyDO["domain"] = QJsonArray::fromStringList(routerSettings.domainProxy);
    if (routerSettings.ipProxy.size() != 0)
        proxyIP["ip"] = QJsonArray::fromStringList(routerSettings.ipProxy);
    proxyDO["outboundTag"] = "proxy";
    proxyIP["outboundTag"] = "proxy";
    QJsonObject blockDO;
    blockDO["type"] = "field";
    QJsonObject blockIP;
    blockIP["type"] = "field";
    if (routerSettings.domainBlock.size() != 0)
        blockDO["domain"] = QJsonArray::fromStringList(routerSettings.domainBlock);
    if (routerSettings.ipBlock.size() != 0)
        blockIP["ip"] = QJsonArray::fromStringList(routerSettings.ipBlock);
    blockDO["outboundTag"] = "block";
    blockIP["outboundTag"] = "block";
    rules.append(apiRule);
    if (outboundSettings.bypassBittorrent)
        rules.append(bypassBittorrent);
    if (outboundSettings.bypassChinaMainland) {
        rules.append(bypassChinaMainlandIP);
        rules.append(bypassChinaMainlandDo);
    }
    if (routerSettings.domainDirect.size() != 0)
        rules.append(directDO);
    if (routerSettings.ipDirect.size() != 0)
        rules.append(directIP);
    if (routerSettings.domainProxy.size() != 0)
        rules.append(proxyDO);
    if (routerSettings.ipProxy.size() != 0)
        rules.append(proxyIP);
    if (routerSettings.domainBlock.size() != 0)
        rules.append(blockDO);
    if (routerSettings.ipBlock.size() != 0)
        rules.append(blockIP);
    routing["rules"] = rules;
    configObj["routing"] = routing;
    QJsonArray inboundsArray;
    QJsonObject socks;
    QJsonObject socksSettings;
    socks["listen"] = inboundSettings.enableIpv6Support ? (inboundSettings.shareOverLan ? "::" : "::1") : (inboundSettings.shareOverLan ? "0.0.0.0" : "127.0.0.1");
    socks["port"] = inboundSettings.socks5LocalPort;
    socks["protocol"] = "socks";
    socksSettings["udp"] = true;
    socksSettings["auth"] = "noauth";
    socksSettings["ip"] = inboundSettings.enableIpv6Support ? (inboundSettings.shareOverLan ? "::" : "::1") : (inboundSettings.shareOverLan ? "0.0.0.0" : "127.0.0.1");
    socks["settings"] = socksSettings;
    socks["tag"] = "inbound";
    if (inboundSettings.inboundSniffing) {
        QJsonObject sniffing;
        sniffing["enabled"] = true;
        QJsonArray destOverride;
        destOverride.append("http");
        destOverride.append("tls");
        sniffing["destOverride"] = destOverride;
        socks["sniffing"] = sniffing;
    }
    QJsonObject apiIn;
    apiIn["listen"] = "127.0.0.1";
    apiIn["port"] = trojanSettings.trojanAPIPort;
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
    outbound["protocol"] = type;
    outbound["sendThrough"] = "0.0.0.0";
    outbound["tag"] = "proxy";
    QJsonArray serversArray;
    QJsonObject server;
    server["address"] = profile.serverAddress;
    server["port"] = profile.serverPort;
    QJsonArray usersArray;
    QJsonObject user;
    if (!profile.username.isEmpty())
        user["username"] = "";
    if (!profile.password.isEmpty())
        user["password"] = profile.password;
    if (type == "socks")
        user["level"] = 0;
    usersArray.append(user);
    if (!profile.password.isEmpty() || !profile.username.isEmpty())
        server["users"] = usersArray;
    serversArray.append(server);
    QJsonObject serverObject;
    serverObject["servers"] = serversArray;
    outbound["settings"] = serverObject;
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
    routing["domainStrategy"] = routerSettings.domainStrategy;
    QJsonArray rules;
    QJsonObject apiRule;
    apiRule["type"] = "field";
    apiRule["outboundTag"] = "api";
    QJsonArray apiRuleInboundTag;
    apiRuleInboundTag.append("api_in");
    apiRule["inboundTag"] = apiRuleInboundTag;
    QJsonObject bypassBittorrent;
    if (outboundSettings.bypassBittorrent) {
        bypassBittorrent["outboundTag"] = "direct";
        QJsonArray protocol;
        protocol.append("bittorrent");
        bypassBittorrent["protocol"] = protocol;
        bypassBittorrent["type"] = "field";
    }
    QJsonObject bypassChinaMainlandIP;
    QJsonObject bypassChinaMainlandDo;
    if (outboundSettings.bypassChinaMainland) {
        QJsonArray bypassIPArr;
        bypassIPArr.append("geoip:cn");
        bypassChinaMainlandIP["ip"] = bypassIPArr;
        bypassChinaMainlandIP["outboundTag"] = "direct";
        bypassChinaMainlandIP["type"] = "field";
        QJsonArray bypassDoArr;
        bypassDoArr.append("geosite:cn");
        bypassChinaMainlandDo["domain"] = bypassDoArr;
        bypassChinaMainlandDo["outboundTag"] = "direct";
        bypassChinaMainlandDo["type"] = "field";
    }
    QJsonObject directDO;
    directDO["type"] = "field";
    QJsonObject directIP;
    directIP["type"] = "field";
    if (routerSettings.domainDirect.size() != 0)
        directDO["domain"] = QJsonArray::fromStringList(routerSettings.domainDirect);
    if (routerSettings.ipDirect.size() != 0)
        directIP["ip"] = QJsonArray::fromStringList(routerSettings.ipDirect);
    directDO["outboundTag"] = "direct";
    directIP["outboundTag"] = "direct";
    QJsonObject proxyDO;
    proxyDO["type"] = "field";
    QJsonObject proxyIP;
    proxyIP["type"] = "field";
    if (routerSettings.domainProxy.size() != 0)
        proxyDO["domain"] = QJsonArray::fromStringList(routerSettings.domainProxy);
    if (routerSettings.ipProxy.size() != 0)
        proxyIP["ip"] = QJsonArray::fromStringList(routerSettings.ipProxy);
    proxyDO["outboundTag"] = "proxy";
    proxyIP["outboundTag"] = "proxy";
    QJsonObject blockDO;
    blockDO["type"] = "field";
    QJsonObject blockIP;
    blockIP["type"] = "field";
    if (routerSettings.domainBlock.size() != 0)
        blockDO["domain"] = QJsonArray::fromStringList(routerSettings.domainBlock);
    if (routerSettings.ipBlock.size() != 0)
        blockIP["ip"] = QJsonArray::fromStringList(routerSettings.ipBlock);
    blockDO["outboundTag"] = "block";
    blockIP["outboundTag"] = "block";
    rules.append(apiRule);
    if (outboundSettings.bypassBittorrent)
        rules.append(bypassBittorrent);
    if (outboundSettings.bypassChinaMainland) {
        rules.append(bypassChinaMainlandIP);
        rules.append(bypassChinaMainlandDo);
    }
    if (routerSettings.domainDirect.size() != 0)
        rules.append(directDO);
    if (routerSettings.ipDirect.size() != 0)
        rules.append(directIP);
    if (routerSettings.domainProxy.size() != 0)
        rules.append(proxyDO);
    if (routerSettings.ipProxy.size() != 0)
        rules.append(proxyIP);
    if (routerSettings.domainBlock.size() != 0)
        rules.append(blockDO);
    if (routerSettings.ipBlock.size() != 0)
        rules.append(blockIP);
    routing["rules"] = rules;
    configObj["routing"] = routing;
    QJsonArray inboundsArray;
    QJsonObject socks;
    QJsonObject socksSettings;
    socks["listen"] = inboundSettings.enableIpv6Support ? (inboundSettings.shareOverLan ? "::" : "::1") : (inboundSettings.shareOverLan ? "0.0.0.0" : "127.0.0.1");
    socks["port"] = inboundSettings.socks5LocalPort;
    socks["protocol"] = "socks";
    socksSettings["udp"] = true;
    socksSettings["auth"] = "noauth";
    socksSettings["ip"] = inboundSettings.enableIpv6Support ? (inboundSettings.shareOverLan ? "::" : "::1") : (inboundSettings.shareOverLan ? "0.0.0.0" : "127.0.0.1");
    socks["settings"] = socksSettings;
    socks["tag"] = "inbound";
    if (inboundSettings.inboundSniffing) {
        QJsonObject sniffing;
        sniffing["enabled"] = true;
        QJsonArray destOverride;
        destOverride.append("http");
        destOverride.append("tls");
        sniffing["destOverride"] = destOverride;
        socks["sniffing"] = sniffing;
    }
    QJsonObject apiIn;
    apiIn["listen"] = "127.0.0.1";
    apiIn["port"] = trojanSettings.trojanAPIPort;
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
    users["testsEnabled"] = profile.testsEnabled;
    usersArray.append(users);
    vnext["users"] = usersArray;
    vnextArray.append(vnext);
    QJsonObject streamSettings;
    streamSettings["network"] = profile.vmessSettings.network;
    streamSettings["security"] = profile.vmessSettings.tls.enable ? "tls" : "none";
    if (streamSettings["network"] == "tcp") {
        QJsonObject tcpSettings;
        QJsonObject tcpHeader;
        tcpHeader["type"] = profile.vmessSettings.tcp.type;
        tcpHeader["request"] = QJsonDocument::fromJson(profile.vmessSettings.tcp.request.toUtf8().data()).object();
        tcpHeader["response"] = QJsonDocument::fromJson(profile.vmessSettings.tcp.response.toUtf8().data()).object();
        tcpSettings["header"] = tcpHeader;
        streamSettings["tcpSettings"] = tcpSettings;
    } else if (streamSettings["network"] == "kcp") {
        QJsonObject kcpSettings;
        kcpSettings["mtu"] = profile.vmessSettings.kcp.mtu;
        kcpSettings["tti"] = profile.vmessSettings.kcp.tti;
        kcpSettings["uplinkCapacity"] = profile.vmessSettings.kcp.uplinkCapacity;
        kcpSettings["downlinkCapacity"] = profile.vmessSettings.kcp.downlinkCapacity;
        kcpSettings["congestion"] = profile.vmessSettings.kcp.congestion;
        kcpSettings["readBufferSize"] = profile.vmessSettings.kcp.readBufferSize;
        kcpSettings["writeBufferSize"] = profile.vmessSettings.kcp.writeBufferSize;
        kcpSettings["downlinkCapacity"] = profile.vmessSettings.kcp.downlinkCapacity;
        if (!profile.vmessSettings.kcp.seed.isEmpty() && !profile.vmessSettings.kcp.seed.isNull())
            kcpSettings["seed"] = profile.vmessSettings.kcp.seed;
        QJsonObject kcpHeader;
        kcpHeader["type"] = profile.vmessSettings.kcp.type;
        kcpSettings["header"] = kcpHeader;
        streamSettings["kcpSettings"] = kcpSettings;
    } else if (streamSettings["network"] == "ws") {
        QJsonObject wsSettings;
        wsSettings["path"] = profile.vmessSettings.ws.path;
        wsSettings["headers"] = Utils::convertWsHeader(profile.vmessSettings.ws.header);
        streamSettings["wsSettings"] = wsSettings;
    } else if (streamSettings["network"] == "http") {
        QJsonObject httpSettings;
        httpSettings["host"] = QJsonArray::fromStringList(profile.vmessSettings.http.host);
        httpSettings["path"] = profile.vmessSettings.http.path;
        streamSettings["httpSettings"] = httpSettings;
    } else if (streamSettings["network"] == "quic") {
        QJsonObject quicSettings;
        quicSettings["security"] = profile.vmessSettings.quic.security;
        quicSettings["key"] = profile.vmessSettings.quic.key;
        QJsonObject quicHeader;
        quicHeader["type"] = profile.vmessSettings.quic.type;
        streamSettings["quicSettings"] = quicSettings;
    }
    QJsonObject tlsSettings;
    tlsSettings["allowInsecure"] = profile.vmessSettings.tls.allowInsecure;
    tlsSettings["allowInsecureCiphers"] = profile.vmessSettings.tls.allowInsecureCiphers;
    tlsSettings["serverName"] = profile.vmessSettings.tls.serverName;
    tlsSettings["alpn"] = QJsonArray::fromStringList(profile.vmessSettings.tls.alpn);
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

void ConfigHelper::generateTrojanJson(TQProfile &profile)
{
    QJsonObject configObj;
    configObj["run_type"] = "client";
    configObj["local_addr"] = inboundSettings.enableIpv6Support ? (inboundSettings.shareOverLan ? "::" : "::1") : (inboundSettings.shareOverLan ? "0.0.0.0" : "127.0.0.1");
    configObj["local_port"] = inboundSettings.socks5LocalPort;
    configObj["remote_addr"] = profile.serverAddress;
    configObj["remote_port"] = profile.serverPort;
    configObj["buffer_size"] = trojanSettings.bufferSize;
    QJsonArray passwordArray;
    passwordArray.append(profile.password);
    configObj["password"] = QJsonValue(passwordArray);
    configObj["log_level"] = generalSettings.logLevel;
    configObj["log_file"] = Utils::getLogDir() + "/core.log";
    QJsonObject ssl;
    ssl["verify"] = profile.verifyCertificate;
    ssl["verify_hostname"] = true;
    ssl["cert"] = trojanSettings.trojanCertPath;
    ssl["cipher"] = trojanSettings.trojanCipher;
    ssl["cipher_tls13"] = trojanSettings.trojanCipherTLS13;
    ssl["sni"] = profile.sni;
    QJsonArray alpnArray;
    alpnArray.append("h2");
    alpnArray.append("http/1.1");
    ssl["alpn"] = QJsonValue(alpnArray);
    ssl["reuse_session"] = profile.reuseSession;
    ssl["session_ticket"] = profile.sessionTicket;
    ssl["curves"] = "";
    ssl["fingerprint"] = parseTLSFingerprint(trojanSettings.fingerprint);
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
    mux["idle_timeout"] = profile.muxIdleTimeout;
    configObj["mux"] = QJsonValue(mux);
    QJsonObject websocket;
    websocket["enabled"] = profile.websocket;
    websocket["path"] = profile.websocketPath;
    websocket["hostname"] = profile.websocketHostname;
    websocket["obfuscation_password"] = profile.websocketObfsPassword;
    websocket["double_tls"] = profile.websocketDoubleTLS;
    configObj["websocket"] = QJsonValue(websocket);
    QJsonObject router;
    router["enabled"] = trojanSettings.enableTrojanRouter;
    if (router["enabled"].toBool()) {
        router["geoip"] = trojanSettings.geoPath + "/geoip.dat";
        router["geosite"] = trojanSettings.geoPath+ "/geosite.dat";
        router["direct"] = appendJsonArray(QJsonArray::fromStringList(routerSettings.domainDirect), QJsonArray::fromStringList(routerSettings.ipDirect));
        router["proxy"] = appendJsonArray(QJsonArray::fromStringList(routerSettings.domainProxy), QJsonArray::fromStringList(routerSettings.ipProxy));
        router["block"] = appendJsonArray(QJsonArray::fromStringList(routerSettings.domainBlock), QJsonArray::fromStringList(routerSettings.ipBlock));
        router["default_policy"] = "proxy";
        router["domain_strategy"] = parseDomainStrategy(routerSettings.domainStrategy);
    }
    configObj["router"] = router;
    QJsonObject api;
    api["enabled"] = trojanSettings.enableTrojanAPI;
    api["api_addr"] = "127.0.0.1";
    api["api_port"] = trojanSettings.trojanAPIPort;
    configObj["api"] = QJsonValue(api);
    QJsonObject forward_proxy;
    forward_proxy["enabled"] = outboundSettings.forwardProxy;
    forward_proxy["proxy_addr"] = outboundSettings.forwardProxyAddress;
    forward_proxy["proxy_port"] = outboundSettings.forwardProxyPort;
    if (outboundSettings.forwardProxyAuthentication) {
        forward_proxy["username"] = outboundSettings.forwardProxyUsername;
        forward_proxy["password"] = outboundSettings.forwardProxyPassword;
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

void ConfigHelper::generateSnellJson(TQProfile &profile)
{
    QJsonObject configObj;
    configObj["local_addr"] = inboundSettings.enableIpv6Support ? (inboundSettings.shareOverLan ? "::" : "::1") : (inboundSettings.shareOverLan? "0.0.0.0" : "127.0.0.1");
    configObj["local_port"] = inboundSettings.socks5LocalPort;
    configObj["remote_addr"] = profile.serverAddress;
    configObj["remote_port"] = profile.serverPort;
    configObj["log_level"] = generalSettings.logLevel;
    configObj["log_file"] = Utils::getLogDir() + "/core.log";
    configObj["buffer_size"] = trojanSettings.bufferSize;
    configObj["psk"] = profile.password;
    QJsonObject api;
    api["enabled"] = trojanSettings.enableTrojanAPI;
    api["api_addr"] = "127.0.0.1";
    api["api_port"] = trojanSettings.trojanAPIPort;
    configObj["api"] = QJsonValue(api);
    QJsonObject obfs;
    obfs["obfs_type"] = profile.obfs;
    obfs["obfs_host"] = profile.obfsParam;
    configObj["obfs"] = QJsonValue(obfs);
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
    text.replace(QString("__STATUS__"), QString("%1:%2").arg(inboundSettings.enableIpv6Support ? (inboundSettings.shareOverLan ? "::" : "::1") : (inboundSettings.shareOverLan) ? "0.0.0.0" : "127.0.0.1")).arg(QString::number(inboundSettings.haproxyStatusPort));
    text.replace(QString("__TROJAN__"), QString("%1:%2").arg(inboundSettings.enableIpv6Support ? (inboundSettings.shareOverLan ? "::" : "::1") : (inboundSettings.shareOverLan ? "0.0.0.0" : "127.0.0.1")).arg(QString::number(inboundSettings.haproxyPort)));
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

GeneralSettings ConfigHelper::getGeneralSettings() const
{
    return generalSettings;
}

InboundSettings ConfigHelper::getInboundSettings() const
{
    return inboundSettings;
}

OutboundSettings ConfigHelper::getOutboundSettings() const
{
    return outboundSettings;
}

TestSettings ConfigHelper::getTestSettings() const
{
    return testSettings;
}

SubscribeSettings ConfigHelper::getSubscribeSettings() const
{
    return subscribeSettings;
}

GraphSettings ConfigHelper::getGraphSettings() const
{
    return graphSettings;
}

RouterSettings ConfigHelper::getRouterSettings() const
{
    return routerSettings;
}

TrojanSettings ConfigHelper::getTrojanSettings() const
{
    return trojanSettings;
}

void ConfigHelper::setGeneralSettings(GeneralSettings gs, InboundSettings is, OutboundSettings os, TestSettings es, SubscribeSettings ss, GraphSettings fs, RouterSettings rs, TrojanSettings ts)
{
    if (gs.toolBarStyle != generalSettings.toolBarStyle) {
        emit toolbarStyleChanged(static_cast<Qt::ToolButtonStyle>(gs.toolBarStyle));
    }
    generalSettings = gs;
    inboundSettings = is;
    outboundSettings = os;
    testSettings = es;
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
    GeneralSettings gSettings;
    generalSettings = settings->value("GeneralSettings", QVariant(gSettings)).value<GeneralSettings>();
    InboundSettings iSettings;
    inboundSettings = settings->value("InboundSettings", QVariant(iSettings)).value<InboundSettings>();
    OutboundSettings oSettings;
    outboundSettings = settings->value("OutboundSettings", QVariant(oSettings)).value<OutboundSettings>();
    TestSettings eSettings;
    testSettings = settings->value("TestSettings", QVariant(eSettings)).value<TestSettings>();
    SubscribeSettings sSettings;
    subscribeSettings = settings->value("SubscribeSettings", QVariant(sSettings)).value<SubscribeSettings>();
    GraphSettings fSettings;
    graphSettings = settings->value("GraphSettings", QVariant(fSettings)).value<GraphSettings>();
    RouterSettings rSettings;
    routerSettings = settings->value("RouterSettings", QVariant(rSettings)).value<RouterSettings>();
    TrojanSettings tSettings;
    tSettings.geoPath = Utils::getConfigPath() + QDir::toNativeSeparators("/dat");
    trojanSettings = settings->value("TrojanSettings", QVariant(tSettings)).value<TrojanSettings>();
}

void ConfigHelper::checkProfileDataUsageReset(TQProfile &profile)
{
    QDate currentDate = QDate::currentDate();
    if (profile.nextResetDate.isNull()){//invalid if the config.ini is old
        //the default reset day is 1 of every month
        profile.nextResetDate = QDate(currentDate.year(), currentDate.month(), 1);
        onConfigUpdateFromOldVersion();
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

    if (this->getGeneralSettings().startAtLogin) {
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
