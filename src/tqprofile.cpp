#include "tqprofile.h"
#include "utils.h"
#include "confighelper.h"

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
    verifyCertificate = true;
    verifyHostname = true;
    reuseSession = true;
    sessionTicket = false;
    reusePort = false;
    tcpFastOpen = false;
    mux = false;
    websocket = false;
    websocketDoubleTLS = false;
    sni = "";
    websocketPath = "";
    websocketHostname = "";
    websocketObfsPassword = "";
    latency = LATENCY_UNKNOWN;
    currentUsage = 0;
    totalUsage = 0;
    QDate currentDate = QDate::currentDate();
    nextResetDate = QDate(currentDate.year(), currentDate.month() + 1, 1);
    // ss/ssr only
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
    vmessSettings = ConfigHelper::generateVmessSettings();

}

TQProfile::TQProfile(const QString &uri)
{
    if (uri.startsWith("ss://"))
        *this = TQProfile::fromSSUri(uri.toStdString());
    else if (uri.startsWith("ssr://"))
        *this = TQProfile::fromSSRUri(uri.toStdString());
    else if (uri.startsWith("vmess://"))
        *this = TQProfile::fromVmessUri(uri.toStdString());
    else if (uri.startsWith("trojan://"))
        *this = TQProfile::fromTrojanUri(uri.toStdString());
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
    result.serverPort = vmess["port"].toVariant().toInt();
    result.uuid = vmess["id"].toString();
    result.alterID = vmess["aid"].toString().toInt();

    QJsonObject vmessSettings = result.vmessSettings;
    vmessSettings["network"] = vmess["net"].toString();

    if (vmessSettings["network"].toString()  == "tcp") {
        QJsonObject tcp = vmessSettings["tcp"].toObject();
        QJsonObject tcpHeader = tcp["header"].toObject();
        tcpHeader["type"] = vmess["type"].toString();
        tcp["header"] = tcpHeader;
        vmessSettings["tcp"] = tcp;
    } else if (vmessSettings["network"].toString() == "http") {
        QJsonObject http = vmessSettings["http"].toObject();
        QJsonArray array;
        foreach(const QString &host, vmess["host"].toString().split(",")) {
            array.push_back(host);
        }
        http["host"] = array;
        http["path"] = vmess["path"].toString();
        vmessSettings["http"] = http;
    } else if (vmessSettings["network"].toString() == "ws") {
        QJsonObject ws = vmessSettings["ws"].toObject();
        QJsonObject wsHeader = ws["host"].toObject()["header"].toObject();
        wsHeader["Host"] = vmess["host"].toString();
        ws["header"] = wsHeader;
        ws["path"] = vmess["path"].toString();
        vmessSettings["ws"] = ws;
    } else if (vmessSettings["network"].toString() == "quic") {
        QJsonObject quic = vmessSettings["quic"].toObject();
        quic["security"] = vmess["host"];
        quic["key"] = vmess["path"];
        vmessSettings["quic"] = quic;
    }

    QJsonObject tls = vmess["tls"].toObject();
    tls["enable"] = vmess["tls"] == "tls" ? true : false;
    vmessSettings["tls"] = tls;

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
    result.verifyCertificate = !query.queryItemValue("allowInsecure").toInt();
    result.sni = query.queryItemValue("sni");
    if (result.sni.isEmpty())
        result.sni = query.queryItemValue("peer");
    result.group = query.queryItemValue("group");

    return result;
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
    vmessObject["addr"] = serverAddress;
    vmessObject["port"] = QString::number(serverPort);
    vmessObject["id"] = uuid;
    vmessObject["aid"] = alterID;
    vmessObject["net"] = vmessSettings["network"].toString();
    if (vmessSettings["network"].toString() == "tcp") {
        vmessObject["type"] = vmessSettings["tcp"].toObject()["header"].toObject()["type"].toString();
    } if (vmessSettings["network"].toString() == "http") {
        vmessObject["host"] = vmessSettings["http"].toObject()["host"].toString().replace("\r\n", "");
        vmessObject["path"] = vmessSettings["http"].toObject()["path"].toString();
    } else if (vmessSettings["network"].toString() == "kcp")
        vmessObject["type"] = vmessSettings["kcp"].toObject()["header"].toObject()["type"].toString();
    else if (vmessSettings["network"].toString() == "ws") {
        foreach (const QString& key, vmessSettings["ws"].toObject()["header"].toObject().keys())
        {
            QJsonValue value = vmessSettings["ws"].toObject()["header"].toObject().value(key);
            if (key == "Host")
                vmessObject["host"] = value.toString();
        }
        vmessObject["path"] = vmessSettings["ws"].toObject()["path"].toString();
    }
    else if (vmessSettings["network"].toString() == "quic") {
        vmessObject["type"] = vmessSettings["quic"].toObject()["header"].toObject()["type"].toString();
        vmessObject["host"] = vmessSettings["quic"].toObject()["security"].toString();
        vmessObject["path"] = vmessSettings["quic"].toObject()["key"].toString();
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
    QString trojanUri = password.toUtf8().toPercentEncoding() + "@" + serverAddress + ":" + QString::number(serverPort) + "?allowinsecure=" + QString::number(int(!verifyCertificate)) + "&tfo=" + QString::number(tcpFastOpen) + "&sni=" + sni + "&mux" + mux + "&ws" + websocket + "&group=" + group.toUtf8().toPercentEncoding();
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
    QString userInfoBase64 = Utils::Base64UrlEncode(method + ":" + password);
    QString snellUri = userInfoBase64 + "@" + serverAddress + ":" + serverPort + "?plugin=obfs-local;obfs=" + obfs + ";obfs-host=" + obfsParam + ";obfs-uri=/";
    return "snell://" + snellUri;
}

QDataStream& operator << (QDataStream &out, const TQProfile &p)
{
    out << p.type << p.autoStart << p.serverPort << p.name << p.serverAddress << p.verifyCertificate << p.verifyHostname << p.password << p.sni << p.reuseSession << p.sessionTicket << p.reusePort << p.tcpFastOpen << p.mux << p.websocket << p.websocketDoubleTLS << p.websocketPath << p.websocketHostname << p.websocketObfsPassword << p.method << p.protocol << p.protocolParam << p.obfs << p.obfsParam << p.plugin << p.pluginParam << p.uuid << p.alterID << p.security << p.vmessSettings << p.latency << p.currentUsage << p.totalUsage << p.lastTime << p.nextResetDate;
    return out;
}

QDataStream& operator >> (QDataStream &in, TQProfile &p)
{
    in >> p.type >> p.autoStart >> p.serverPort >> p.name >> p.serverAddress >> p.verifyCertificate >> p.verifyHostname >> p.password >> p.sni >> p.reuseSession >> p.sessionTicket >> p.reusePort >> p.tcpFastOpen >> p.mux >> p.websocket >> p.websocketDoubleTLS >> p.websocketPath >> p.websocketHostname >> p.websocketObfsPassword >> p.method >> p.protocol >> p.protocolParam >> p.obfs >> p.obfsParam >> p.plugin >> p.pluginParam >> p.uuid >> p.alterID >> p.security >> p.vmessSettings >> p.latency >> p.currentUsage >> p.totalUsage >> p.lastTime >> p.nextResetDate;
    return in;
}
