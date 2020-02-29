#include "tqprofile.h"

#include <QDebug>

TQProfile::TQProfile()
{
    autoStart = false;
    dualMode = true;
    serverPort = 443;
    name = QObject::tr("Unnamed Profile");
    verifyCertificate = true;
    verifyHostname = true;
    reuseSession = true;
    sessionTicket = true;
    reusePort = false;
    tcpFastOpen = false;
    latency = LATENCY_UNKNOWN;
    currentUsage = 0;
    totalUsage = 0;
    QDate currentDate = QDate::currentDate();
    nextResetDate = QDate(currentDate.year(), currentDate.month() + 1, 1);
}

TQProfile::TQProfile(const QString &uri)
{
    *this = TQProfile::fromUri(uri.toStdString());
}

TQProfile TQProfile::fromUri(const std::string& trojanUri) const
{
    std::string prefix = "trojan://";

    if (trojanUri.length() < 9) {
        throw std::invalid_argument("Trojan URI is too short");
    }

    /** Prevent line separator casuing wrong password. */
    if (!QString::fromStdString(trojanUri).startsWith("trojan://")) {
         throw std::invalid_argument("Invalid Trojan URI");
    }

    TQProfile result;
    //remove the prefix "trojan://" from uri
    std::string uri(trojanUri.data() + 9, trojanUri.length() - 9);
    size_t hashPos = uri.find_last_of('#');
    if (hashPos != std::string::npos) {
        // Get the name/remark
        result.name = QString::fromStdString(uri.substr(hashPos + 1));
        uri.erase(hashPos);
    }

    size_t atPos = uri.find_first_of('@');
    if (atPos != std::string::npos) {
        result.password = QString::fromStdString(uri.substr(0, atPos));
        uri.erase(0, atPos + 1);
        size_t colonPos = uri.find_last_of(':');
        if (colonPos == std::string::npos) {
            throw std::invalid_argument("Can't find the colon separator between hostname and port");
        }
        result.serverAddress = QString::fromStdString(uri.substr(0, colonPos));
        result.serverPort = std::stoi(uri.substr(colonPos + 1));
    } else {
        throw std::invalid_argument("Can't find the at separator between password and hostname");
    }

    return result;
}

/**
 * @brief TQProfile::toUri
 * @return QString uri of trojan server
 */
QString TQProfile::toUri() const
{
    QString trojanUri = password + "@" + serverAddress + ":" + QString::number(serverPort);
    QByteArray uri = QByteArray(trojanUri.toUtf8());
    uri.prepend("trojan://");
    uri.append("#");
    uri.append(name);
    return QString(uri);
}

QDataStream& operator << (QDataStream &out, const TQProfile &p)
{
    out << p.autoStart << p.serverPort << p.dualMode << p.name << p.serverAddress << p.verifyCertificate << p.verifyHostname << p.password << p.reuseSession << p.sessionTicket << p.reusePort << p.tcpFastOpen << p.latency << p.currentUsage << p.totalUsage << p.lastTime << p.nextResetDate;
    return out;
}

QDataStream& operator >> (QDataStream &in, TQProfile &p)
{
    in >> p.autoStart >> p.serverPort >> p.dualMode >> p.name >> p.serverAddress >> p.verifyCertificate >> p.verifyHostname >> p.password >> p.reuseSession >> p.sessionTicket >> p.reusePort >> p.tcpFastOpen >> p.latency >> p.currentUsage >> p.totalUsage >> p.lastTime >> p.nextResetDate;
    return in;
}
