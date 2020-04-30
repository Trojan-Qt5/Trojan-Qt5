#include "tqprofile.h"
#include "utils.h"

#include <QDebug>

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
}

TQProfile::TQProfile(const QString &uri)
{
    if (uri.startsWith("ssr://"))
        *this = TQProfile::fromSSRUri(uri.toStdString());
    else if (uri.startsWith("trojan://"))
        *this = TQProfile::fromTrojanUri(uri.toStdString());
}

bool TQProfile::equals(const TQProfile &profile) const
{
    return (serverPort == profile.serverPort
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
        throw std::invalid_argument("SSR URI is too short");
    }

    if (!QString::fromStdString(ssUri).startsWith("ss://")) {
        throw std::invalid_argument("Invalid Trojan URI");
    }

    TQProfile result;

    result.type = "ss";

    //remove the prefix "ss://" from uri
    std::string uri(ssUri.data() + 5, ssUri.length() - 5);

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
        throw std::invalid_argument("Invalid Trojan URI");
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

    QStringList decoded3 = decoded2[1].split("&");

    for (QString data: decoded3) {
        if (data.startsWith("obfs")) {
            data = data.replace("obfsparam=", "");
            result.obfsParam = Utils::Base64UrlDecode(data);
        } else if (data.startsWith("proto")) {
            data = data.replace("protoparam=", "");
            result.protocolParam = Utils::Base64UrlDecode(data);
        } else if (data.startsWith("remarks")) {
            data = data.replace("remarks=", "");
            result.name = Utils::Base64UrlDecode(data);
        } else if (data.startsWith("group")) {
            data = data.replace("group=", "");
            result.group = Utils::Base64UrlDecode(data);
        }
    }

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

    size_t questionMarkPos = uri.find_first_of('?');
    if (questionMarkPos != std::string::npos) {
        result.verifyCertificate = !std::stoi(uri.substr(questionMarkPos + 15));
        uri.erase(0, questionMarkPos + 16);
    }

    QStringList decoded = QString::fromStdString(uri).split("&");

    for (QString data : decoded) {
        if (data.startsWith("tfo")) {
            data = data.replace("tfo=", "");
            result.tcpFastOpen = data.toInt();
        } else if (data.startsWith("sni")) {
            data = data.replace("sni=", "");
            result.sni = data;
        } else if (data.startsWith("group")) {
            data = data.replace("group=", "");
            result.group = data;
        }
    }

    /* Not used any more
    size_t ampersandPos = uri.find_last_of('&');
    if (ampersandPos != std::string::npos) {
        result.sni = QString::fromStdString(uri.substr(ampersandPos + 5));
        uri.erase(ampersandPos, ampersandPos + 6);
    }

    size_t ampersandPos2 = uri.find_first_of('&');
    if (ampersandPos2 != std::string::npos) {
        result.tcpFastOpen = std::stoi(uri.substr(ampersandPos2 + 5));
        uri.erase(ampersandPos2, ampersandPos2 + 6);
    }
    */

    return result;
}


/**
 * @brief TQProfile::toSSUri
 * @return QString uri of ss server
 */
QString TQProfile::toSSUri() const
{
    QString userInfoBase64 = Utils::Base64UrlEncode(password + ":" + method);
    return "ss://" + userInfoBase64 + "@" + serverAddress + ":" + QString::number(serverPort);
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
 * @brief TQProfile::toTrojanUri
 * @return QString uri of trojan server
 */
QString TQProfile::toTrojanUri() const
{
    QString trojanUri = password.toUtf8().toPercentEncoding() + "@" + serverAddress + ":" + QString::number(serverPort) + "?allowinsecure=" + QString::number(int(!verifyCertificate)) + "&tfo=" + QString::number(tcpFastOpen) + "&sni=" + sni + "&group=" + group.toUtf8().toPercentEncoding();
    QByteArray uri = QByteArray(trojanUri.toUtf8());
    uri.prepend("trojan://");
    uri.append("#");
    uri.append(name.toUtf8().toPercentEncoding());
    return QString(uri);
}

QDataStream& operator << (QDataStream &out, const TQProfile &p)
{
    out << p.type << p.autoStart << p.serverPort << p.name << p.serverAddress << p.verifyCertificate << p.verifyHostname << p.password << p.sni << p.reuseSession << p.sessionTicket << p.reusePort << p.tcpFastOpen << p.mux << p.websocket << p.websocketDoubleTLS << p.websocketPath << p.websocketHostname << p.websocketObfsPassword << p.method << p.protocol << p.protocolParam << p.obfs << p.obfsParam << p.plugin << p.pluginParam << p.latency << p.currentUsage << p.totalUsage << p.lastTime << p.nextResetDate;
    return out;
}

QDataStream& operator >> (QDataStream &in, TQProfile &p)
{
    in >> p.type >> p.autoStart >> p.serverPort >> p.name >> p.serverAddress >> p.verifyCertificate >> p.verifyHostname >> p.password >> p.sni >> p.reuseSession >> p.sessionTicket >> p.reusePort >> p.tcpFastOpen >> p.mux >> p.websocket >> p.websocketDoubleTLS >> p.websocketPath >> p.websocketHostname >> p.websocketObfsPassword >> p.method >> p.protocol >> p.protocolParam >> p.obfs >> p.obfsParam >> p.plugin >> p.pluginParam >> p.latency >> p.currentUsage >> p.totalUsage >> p.lastTime >> p.nextResetDate;
    return in;
}
