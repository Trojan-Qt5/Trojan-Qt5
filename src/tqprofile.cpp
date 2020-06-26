#include "tqprofile.h"
#include "utils.h"
#include "confighelper.h"
#include "v2raystruct.h"

#include <QJsonDocument>
#include <QUrlQuery>
#include <QJsonArray>

TQProfile::TQProfile()
{
    type = "trojan";
    group = "";
    autoStart = false;
    serverPort = 443;
    name = QObject::tr("Unnamed Profile");
    tcpFastOpen = false;
    latency = LATENCY_UNKNOWN;
    currentDownloadUsage = 0;
    currentUploadUsage = 0;
    totalDownloadUsage = 0;
    totalUploadUsage = 0;
    QDate currentDate = QDate::currentDate();
    nextResetDate = QDate(currentDate.year(), currentDate.month() + 1, 1);
    // socks5/http only
    username = "";
    // ss/ssr/snell only
    method = QString("aes-256-cfb");
    protocol = QString("origin");
    protocolParam = QString("");
    obfs = QString("plain");
    obfsParam = QString("");
    plugin = QString("");
    pluginParam = QString("");
    // vmess only
    uuid = QString("");
    alterID = 32;
    security = QString("auto");
    // trojan only
    verifyCertificate = true;
    verifyHostname = true;
    reuseSession = true;
    sessionTicket = false;
    reusePort = false;
    mux = false;
    muxConcurrency = 8;
    muxIdleTimeout = 60;
    sni = "";
    websocket = false;
    websocketDoubleTLS = false;
    websocketPath = "";
    websocketHostname = "";
    websocketObfsPassword = "";
}

TQProfile::TQProfile(const QString &uri)
{
    if (uri.startsWith("socks5://"))
        *this = TQProfile::fromSocks5Uri(uri.toStdString());
    else if (uri.startsWith("http://"))
        *this = TQProfile::fromHttpUri(uri.toStdString());
    else if (uri.startsWith("ss://"))
        try {
            *this = TQProfile::fromSSUri(uri.toStdString());
        } catch (...) {
            *this = TQProfile::fromOldSSUri(uri.toStdString());
        }
    else if (uri.startsWith("ssr://"))
        *this = TQProfile::fromSSRUri(uri.toStdString());
    else if (uri.startsWith("vmess://"))
        *this = TQProfile::fromVmessUri(uri.toStdString());
    else if (uri.startsWith("trojan://"))
        *this = TQProfile::fromTrojanUri(uri.toStdString());
    else if (uri.startsWith("snell://"))
        *this = TQProfile::fromSnellUri(uri.toStdString());
}

bool TQProfile::equals(const TQProfile &profile) const
{
    return (type == profile.type
         && serverPort == profile.serverPort
         && group == profile.group
         && name == profile.name
         && verifyCertificate == profile.verifyCertificate
         && verifyHostname == profile.verifyHostname
         && reuseSession == profile.reuseSession
         && reusePort == profile.reusePort
         && tcpFastOpen == profile.tcpFastOpen
         && mux == profile.mux
         && websocket == profile.websocket
         && websocketDoubleTLS == profile.websocketDoubleTLS
         && websocketPath == profile.websocketPath
         && websocketHostname == profile.websocketHostname
         && websocketObfsPassword == profile.websocketObfsPassword);
}

TQProfile TQProfile::fromSocks5Uri(const std::string& socks5Uri) const
{
    std::string prefix = "socks5://";

    if (socks5Uri.length() < 9) {
        throw std::invalid_argument("SOCKS5 URI is too short");
    }

    if (!QString::fromStdString(socks5Uri).startsWith("socks5://")) {
        throw std::invalid_argument("Invalid SOCKS5 URI");
    }

    TQProfile result;

    result.type = "socks5";

    //remove the prefix "socks5://" from uri
    std::string uri(socks5Uri.data() + 9, socks5Uri.length() - 9);

    size_t hashPos = uri.find_last_of('#');
    if (hashPos != std::string::npos) {
        // Get the name/remark
        result.name = QUrl::fromPercentEncoding(QString::fromStdString(uri.substr(hashPos + 1)).toUtf8().data());
        uri.erase(hashPos);
    }

    size_t atPos = uri.find_first_of('@');
    if (atPos != std::string::npos) {
        QString userInfo = QByteArray::fromBase64(QString::fromStdString(uri.substr(0, atPos)).toUtf8().data());
        result.username = userInfo.split(":")[0];
        result.password = userInfo.split(":")[1];
        uri.erase(0, atPos + 1);
        size_t colonPos = uri.find_last_of(':');
        if (colonPos == std::string::npos) {
            throw std::invalid_argument("Can't find the colon separator between hostname and port");
        }
        result.serverAddress = QString::fromStdString(uri.substr(0, colonPos));
        result.serverPort = std::stoi(uri.substr(colonPos + 1));
        uri.erase(0, colonPos + 4);
    } else {
        throw std::invalid_argument("Can't find the at separator between userInfo and hostname");
    }

    return result;
}

TQProfile TQProfile::fromHttpUri(const std::string& httpUri) const
{
    std::string prefix = "http://";

    if (httpUri.length() < 7) {
        throw std::invalid_argument("HTTP URI is too short");
    }

    if (!QString::fromStdString(httpUri).startsWith("http://")) {
        throw std::invalid_argument("Invalid HTTP URI");
    }

    TQProfile result;

    result.type = "http";

    //remove the prefix "http://" from uri
    std::string uri(httpUri.data() + 7, httpUri.length() - 7);

    size_t hashPos = uri.find_last_of('#');
    if (hashPos != std::string::npos) {
        // Get the name/remark
        result.name = QUrl::fromPercentEncoding(QString::fromStdString(uri.substr(hashPos + 1)).toUtf8().data());
        uri.erase(hashPos);
    }

    size_t atPos = uri.find_first_of('@');
    if (atPos != std::string::npos) {
        QString userInfo = QByteArray::fromBase64(QString::fromStdString(uri.substr(0, atPos)).toUtf8().data());
        result.username = userInfo.split(":")[0];
        result.password = userInfo.split(":")[1];
        uri.erase(0, atPos + 1);
        size_t colonPos = uri.find_last_of(':');
        if (colonPos == std::string::npos) {
            throw std::invalid_argument("Can't find the colon separator between hostname and port");
        }
        result.serverAddress = QString::fromStdString(uri.substr(0, colonPos));
        result.serverPort = std::stoi(uri.substr(colonPos + 1));
        uri.erase(0, colonPos + 4);
    } else {
        throw std::invalid_argument("Can't find the at separator between userInfo and hostname");
    }

    return result;
}

TQProfile TQProfile::fromSSUri(const std::string& ssUri) const
{
    std::string prefix = "ss://";
    if (ssUri.length() < 5) {
        throw std::invalid_argument("SS URI is too short");
    }

    if (!QString::fromStdString(ssUri).startsWith("ss://")) {
        throw std::invalid_argument("Invalid SS URI");
    }

    TQProfile result;

    result.type = "ss";

    //remove the prefix "ss://" from uri
    std::string uri(ssUri.data() + 5, ssUri.length() - 5);
    size_t hashPos = uri.find_last_of('#');
    if (hashPos != std::string::npos) {
        // Get the name/remark
        result.name = QUrl::fromPercentEncoding(QString::fromStdString(uri.substr(hashPos + 1)).toUtf8().data());
        uri.erase(hashPos);
    }

    size_t atPos = uri.find_first_of('@');
    if (atPos != std::string::npos) {
        QString userInfo = QByteArray::fromBase64(QString::fromStdString(uri.substr(0, atPos)).toUtf8().data());
        result.method = userInfo.split(":")[0];
        result.password = userInfo.split(":")[1];
        uri.erase(0, atPos + 1);
        size_t colonPos = uri.find_last_of(':');
        if (colonPos == std::string::npos) {
            throw std::invalid_argument("Can't find the colon separator between hostname and port");
        }
        result.serverAddress = QString::fromStdString(uri.substr(0, colonPos));
        result.serverPort = std::stoi(uri.substr(colonPos + 1));
        uri.erase(0, colonPos + 4);
    } else {
        throw std::invalid_argument("Can't find the at separator between userInfo and hostname");
    }

    return  result;
}

TQProfile TQProfile::fromOldSSUri(const std::string& ssUri) const
{
    std::string prefix = "ss://";

    if (ssUri.length() < 5) {
        throw std::invalid_argument("SS URI is too short");
    }

    if (!QString::fromStdString(ssUri).startsWith("ss://")) {
        throw std::invalid_argument("Invalid SS URI");
    }

    TQProfile result;

    result.type = "ss";

    //remove the prefix "ss://" from uri
    std::string uri(ssUri.data() + 5, ssUri.length() - 5);

    size_t hashPos = uri.find_last_of('#');
    if (hashPos != std::string::npos) {
        // Get the name/remark
        result.name = QUrl::fromPercentEncoding(QString::fromStdString(uri.substr(hashPos + 1)).toUtf8().data());
        uri.erase(hashPos);
    }

    //decode base64
    uri = Utils::Base64UrlDecode(QString::fromStdString(uri)).toStdString();

    size_t atPos = uri.find_first_of('@');
    if (atPos != std::string::npos) {
        QString userInfo = QString::fromStdString(uri.substr(0, atPos));
        result.method = userInfo.split(":")[0];
        result.password = userInfo.split(":")[1];
        uri.erase(0, atPos + 1);
        size_t colonPos = uri.find_last_of(':');
        if (colonPos == std::string::npos) {
            throw std::invalid_argument("Can't find the colon separator between hostname and port");
        }
        result.serverAddress = QString::fromStdString(uri.substr(0, colonPos));
        result.serverPort = std::stoi(uri.substr(colonPos + 1));
        uri.erase(0, colonPos + 4);
    } else {
        throw std::invalid_argument("Can't find the at separator between userInfo and hostname");
    }

    return  result;
}

TQProfile TQProfile::fromSSRUri(const std::string& ssrUri) const
{
    std::string prefix = "ssr://";

    if (ssrUri.length() < 6) {
        throw std::invalid_argument("SSR URI is too short");
    }

    if (!QString::fromStdString(ssrUri).startsWith("ssr://")) {
        throw std::invalid_argument("Invalid SSR URI");
    }

    TQProfile result;

    result.type = "ssr";

    //remove the prefix "ssr://" from uri
    std::string uri(ssrUri.data() + 6, ssrUri.length() - 6);
    QString decodedUri = Utils::Base64UrlDecode(QString::fromStdString(uri));
    QStringList decoded = decodedUri.split(":");

    if (decoded.length() == 6) {
        result.serverAddress = decoded[0];
        result.serverPort = decoded[1].toInt();
        result.protocol = decoded[2];
        result.method = decoded[3];
        result.obfs = decoded[4];
    } else {
        throw std::invalid_argument("Not sufficent arguments!");
    }

    QStringList decoded2 = decoded[5].split("/?");

    result.password = Utils::Base64UrlDecode(decoded2[0]);

    QUrl url(decodedUri);
    QUrlQuery query(url.query());

    result.protocolParam = Utils::Base64UrlDecode(query.queryItemValue("protoparam"));
    result.obfsParam = Utils::Base64UrlDecode(query.queryItemValue("obfsparam"));
    result.name = Utils::Base64UrlDecode(query.queryItemValue("remarks"));
    result.group = Utils::Base64UrlDecode(query.queryItemValue("group"));

    return result;
}


TQProfile TQProfile::fromVmessUri(const std::string& vmessUri) const
{
    std::string prefix = "vmess://";

    if (vmessUri.length() < 8) {
        throw std::invalid_argument("Vmess URI is too short");
    }

    //prevent line separator casuing wrong password.
    if (!QString::fromStdString(vmessUri).startsWith("vmess://")) {
         throw std::invalid_argument("Invalid Vmess URI");
    }

    TQProfile result;

    result.type = "vmess";

    //remove the prefix "vmess://" from uri
    std::string uri(vmessUri.data() + 8, vmessUri.length() - 8);
    QJsonDocument doc = QJsonDocument::fromJson(Utils::Base64UrlDecode(QString::fromStdString(uri)).toUtf8().data());
    QJsonObject vmess = doc.object();

    result.name = vmess["ps"].toString();
    result.serverAddress = vmess["add"].toString();
    if (result.serverAddress.isEmpty())
        result.serverAddress = vmess["addr"].toString();
    result.serverPort = vmess["port"].toVariant().toInt();
    result.uuid = vmess["id"].toString();
    result.alterID = vmess["aid"].toString().toInt();

    VmessSettings vmessSettings = result.vmessSettings;
    vmessSettings.network = vmess["net"].toString();

    if (vmessSettings.network == "tcp") {
        vmessSettings.tcp.type = vmess["type"].toString();
    } else if (vmessSettings.network == "http") {
        QJsonArray array;
        foreach(const QString &host, vmess["host"].toString().split(",")) {
            array.push_back(host);
        }
        vmessSettings.http.host = array;
        vmessSettings.http.path = vmess["path"].toString();
    } else if (vmessSettings.network == "ws") {
        QJsonObject wsHeader;
        wsHeader["Host"] = vmess["host"].toString();
        vmessSettings.ws.header = wsHeader;
        vmessSettings.ws.path = vmess["path"].toString();
    } else if (vmessSettings.network == "quic") {
        vmessSettings.quic.security = vmess["host"].toString();
        vmessSettings.quic.key = vmess["path"].toString();
    }

    QJsonObject tls = vmess["tls"].toObject();
    vmessSettings.tls.enable = vmess["tls"] == "tls" ? true : false;

    result.vmessSettings = vmessSettings;

    return result;
}

TQProfile TQProfile::fromTrojanUri(const std::string& trojanUri) const
{
    std::string prefix = "trojan://";

    if (trojanUri.length() < 9) {
        throw std::invalid_argument("Trojan URI is too short");
    }

    //prevent line separator casuing wrong password.
    if (!QString::fromStdString(trojanUri).startsWith("trojan://")) {
         throw std::invalid_argument("Invalid Trojan URI");
    }

    TQProfile result;

    result.type = "trojan";

    //remove the prefix "trojan://" from uri
    std::string uri(trojanUri.data() + 9, trojanUri.length() - 9);
    size_t hashPos = uri.find_last_of('#');
    if (hashPos != std::string::npos) {
        // Get the name/remark
        result.name = QUrl::fromPercentEncoding(QString::fromStdString(uri.substr(hashPos + 1)).toUtf8().data());
        uri.erase(hashPos);
    }

    size_t atPos = uri.find_first_of('@');
    if (atPos != std::string::npos) {
        result.password = QUrl::fromPercentEncoding(QString::fromStdString(uri.substr(0, atPos)).toUtf8().data());
        uri.erase(0, atPos + 1);
        size_t colonPos = uri.find_last_of(':');
        if (colonPos == std::string::npos) {
            throw std::invalid_argument("Can't find the colon separator between hostname and port");
        }
        result.serverAddress = QString::fromStdString(uri.substr(0, colonPos));
        result.serverPort = std::stoi(uri.substr(colonPos + 1));
        uri.erase(0, colonPos + 4);
    } else {
        throw std::invalid_argument("Can't find the at separator between password and hostname");
    }

    QUrl url(QString::fromStdString(trojanUri));
    QUrlQuery query(url.query());
    result.tcpFastOpen = query.queryItemValue("tfo").toInt();
    result.verifyCertificate = !query.queryItemValue("allowinsecure").toInt();
    if (query.queryItemValue("allowinsecure").isEmpty())
         result.verifyCertificate = !query.queryItemValue("allowInsecure").toInt();
    result.sni = query.queryItemValue("sni");
    if (result.sni.isEmpty())
        result.sni = query.queryItemValue("peer");
    result.mux = query.queryItemValue("mux").toInt();
    result.websocket = query.queryItemValue("ws").toInt();
    result.group = query.queryItemValue("group");

    return result;
}

TQProfile TQProfile::fromSnellUri(const std::string& snellUri) const
{
    std::string prefix = "snell://";

    if (snellUri.length() < 8) {
        throw std::invalid_argument("Snell URI is too short");
    }

    //prevent line separator casuing wrong password.
    if (!QString::fromStdString(snellUri).startsWith("snell://")) {
         throw std::invalid_argument("Invalid Snell URI");
    }

    TQProfile result;

    result.type = "snell";

    //remove the prefix "snell://" from uri
    std::string uri(snellUri.data() + 9, snellUri.length() - 9);
    size_t hashPos = uri.find_last_of('#');
    if (hashPos != std::string::npos) {
        // Get the name/remark
        result.name = QUrl::fromPercentEncoding(QString::fromStdString(uri.substr(hashPos + 1)).toUtf8().data());
        uri.erase(hashPos);
    }

    size_t atPos = uri.find_first_of('@');
    if (atPos != std::string::npos) {
        QString userInfo = QString::fromStdString(uri.substr(0, atPos));
        result.method = userInfo.split(":")[0];
        result.password = userInfo.split(":")[1];
        uri.erase(0, atPos + 1);
        size_t colonPos = uri.find_last_of(':');
        if (colonPos == std::string::npos) {
            throw std::invalid_argument("Can't find the colon separator between hostname and port");
        }
        result.serverAddress = QString::fromStdString(uri.substr(0, colonPos));
        result.serverPort = std::stoi(uri.substr(colonPos + 1));
        uri.erase(0, colonPos + 4);
    } else {
        throw std::invalid_argument("Can't find the at separator between userInfo and hostname");
    }

    QUrl url(QString::fromStdString(snellUri));
    QUrlQuery query(url.query());
    result.obfs = query.queryItemValue("obfs");
    result.obfsParam = query.queryItemValue("obfs-host");

    return result;
}

/**
 * @brief TQProfile::toSocks5Uri
 * @return QString uri of socks5 server
 */
QString TQProfile::toSocks5Uri() const
{
    QString userInfoBase64 = Utils::Base64UrlEncode(username + ":" + password);
    return "socks5://" + userInfoBase64 + "@" + serverAddress + ":" + QString::number(serverPort) + "#" + name.toUtf8().toPercentEncoding();
}

/**
 * @brief TQProfile::toHttpUri
 * @return QString uri of socks5 server
 */
QString TQProfile::toHttpUri() const
{
    QString userInfoBase64 = Utils::Base64UrlEncode(username + ":" + password);
    return "http://" + userInfoBase64 + "@" + serverAddress + ":" + QString::number(serverPort) + "#" + name.toUtf8().toPercentEncoding();
}

/**
 * @brief TQProfile::toSSUri
 * @return QString uri of ss server
 */
QString TQProfile::toSSUri() const
{
    QString userInfoBase64 = Utils::Base64UrlEncode(method + ":" + password);
    return "ss://" + userInfoBase64 + "@" + serverAddress + ":" + QString::number(serverPort) + "#" + name.toUtf8().toPercentEncoding();
}

/**
 * @brief TQProfile::toSSRUri
 * @return QString uri of ssr server
 */
QString TQProfile::toSSRUri() const
{
    QString passwordBase64 = Utils::Base64UrlEncode(password);
    QString obfsParamBase64 = Utils::Base64UrlEncode(obfsParam);
    QString protoParamBase64 = Utils::Base64UrlEncode(protocolParam);
    QString remarksBase64 = Utils::Base64UrlEncode(name);
    QString groupBase64 = Utils::Base64UrlEncode(group);
    QString params = QString("obfsparam=%1&protoparam=%2&remarks=%3&group=%4").arg(obfsParamBase64).arg(protoParamBase64).arg(remarksBase64).arg(groupBase64);
    QString ssrUri = QString("%1:%2:%3:%4:%5:%6/?%7").arg(serverAddress).arg(QString::number(serverPort)).arg(protocol).arg(method).arg(obfs).arg(passwordBase64).arg(params);
    return "ssr://" + Utils::Base64UrlEncode(ssrUri);
}

/**
  * @brief TQProfile::toVmessUri
  * @return QString uri for vmess server
 */
QString TQProfile::toVmessUri() const
{
    QJsonObject vmessObject;
    vmessObject["v"] = "2";
    vmessObject["ps"] = name;
    vmessObject["add"] = serverAddress;
    vmessObject["port"] = QString::number(serverPort);
    vmessObject["id"] = uuid;
    vmessObject["aid"] = alterID;
    vmessObject["net"] = vmessSettings.network;
    if (vmessSettings.network == "tcp") {
        vmessObject["type"] = vmessSettings.tcp.type;
    } if (vmessSettings.network == "http") {
        vmessObject["host"] = vmessSettings.http.host;
        vmessObject["path"] = vmessSettings.http.path;
    } else if (vmessSettings.network == "kcp")
        vmessObject["type"] = vmessSettings.kcp.type;
    else if (vmessSettings.network == "ws") {
        foreach (const QString& key, vmessSettings.ws.header.keys())
        {
            QJsonValue value = vmessSettings.ws.header.value(key);
            if (key == "Host")
                vmessObject["host"] = value.toString();
        }
        vmessObject["path"] = vmessSettings.ws.path;
    }
    else if (vmessSettings.network == "quic") {
        vmessObject["type"] = vmessSettings.quic.type;
        vmessObject["host"] = vmessSettings.quic.security;
        vmessObject["path"] = vmessSettings.quic.key;
    }
    vmessObject["tls"] = vmessObject["tls"].toObject()["enable"].toBool() ? "tls" : "none";
    QJsonDocument uri(vmessObject);
    return "vmess://" + Utils::Base64UrlEncode(uri.toJson());
}


/**
 * @brief TQProfile::toTrojanUri
 * @return QString uri of trojan server
 */
QString TQProfile::toTrojanUri() const
{
    QString trojanUri = password.toUtf8().toPercentEncoding() + "@" + serverAddress + ":" + QString::number(serverPort) + "?allowinsecure=" + QString::number(int(!verifyCertificate)) + "&tfo=" + QString::number(tcpFastOpen) + "&sni=" + sni + "&mux=" + QString::number(mux) + "&ws=" + QString::number(websocket) + "&wss=" + QString::number(websocketDoubleTLS) + "&wsPath=" + websocketPath + "&wsHostname=" + websocketHostname + "&wsObfsPassword=" + websocketObfsPassword.toUtf8().toPercentEncoding() + "&group=" + group.toUtf8().toPercentEncoding();
    QByteArray uri = QByteArray(trojanUri.toUtf8());
    uri.prepend("trojan://");
    uri.append("#");
    uri.append(name.toUtf8().toPercentEncoding());
    return QString(uri);
}

/**
 * @brief TQProfile::toSnellUri
 * @return QString uri of snell server
 */
QString TQProfile::toSnellUri() const
{
    QString snellUri = "chacha20-ietf-poly1305:" + password + "@" + serverAddress + ":" + QString::number(serverPort) + "?obfs=" + obfs + "&obfs-host=" + obfsParam.toUtf8().toPercentEncoding();
    QByteArray uri = QByteArray(snellUri.toUtf8());
    uri.prepend("snell://");
    uri.append("#");
    uri.append(name.toUtf8().toPercentEncoding());
    return uri;
}

QDataStream& operator << (QDataStream &out, const TQProfile &p)
{
    out << p.type << p.autoStart << p.serverPort << p.name << p.serverAddress << p.verifyCertificate << p.verifyHostname << p.password << p.sni << p.reuseSession << p.sessionTicket << p.reusePort << p.tcpFastOpen << p.mux << p.muxConcurrency << p.muxIdleTimeout << p.websocket << p.websocketDoubleTLS << p.websocketPath << p.websocketHostname << p.websocketObfsPassword << p.method << p.protocol << p.protocolParam << p.obfs << p.obfsParam << p.plugin << p.pluginParam << p.uuid << p.alterID << p.security << p.vmessSettings << p.latency << p.currentDownloadUsage << p.currentUploadUsage << p.totalDownloadUsage << p.totalUploadUsage << p.lastTime << p.nextResetDate;
    return out;
}

QDataStream& operator >> (QDataStream &in, TQProfile &p)
{
    in >> p.type >> p.autoStart >> p.serverPort >> p.name >> p.serverAddress >> p.verifyCertificate >> p.verifyHostname >> p.password >> p.sni >> p.reuseSession >> p.sessionTicket >> p.reusePort >> p.tcpFastOpen >> p.mux >> p.muxConcurrency >> p.muxIdleTimeout >> p.websocket >> p.websocketDoubleTLS >> p.websocketPath >> p.websocketHostname >> p.websocketObfsPassword >> p.method >> p.protocol >> p.protocolParam >> p.obfs >> p.obfsParam >> p.plugin >> p.pluginParam >> p.uuid >> p.alterID >> p.security >> p.vmessSettings >> p.latency >> p.currentDownloadUsage >> p.currentUploadUsage >> p.totalDownloadUsage >> p.totalUploadUsage >> p.lastTime >> p.nextResetDate;
    return in;
}
