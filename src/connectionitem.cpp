#include "connectionitem.h"
#include <QFont>
#include <cmath>

#include <QVariant>

ConnectionItem::ConnectionItem(Connection *_con, QObject *parent) :
    QObject(parent),
    con(_con)
{
    if (con) {
        con->setParent(this);
        connect(con, &Connection::stateChanged, this, &ConnectionItem::onConnectionStateChanged);
        connect(con, &Connection::stateChanged, this, &ConnectionItem::stateChanged);
        connect(con, &Connection::dataUsageChanged, this, &ConnectionItem::dataUsageChanged);
        connect(con, &Connection::latencyAvailable, this, &ConnectionItem::onConnectionPingFinished);
        connect(con, &Connection::latencyAvailable, this, &ConnectionItem::latencyChanged);
        connect(con, &Connection::startFailed, this, &ConnectionItem::onStartFailed);
    }
}

const QStringList ConnectionItem::bytesUnits = QStringList()
        << " B" << " KiB" << " MiB" << " GiB" << " TiB"
        << " PiB" << " EiB" << " ZiB" << " YiB";

int ConnectionItem::columnCount()
{
    return 9;
}

QVariant ConnectionItem::data(int column, int role) const
{
    if (!con) {
        return QVariant();
    }

    if ((role == Qt::DisplayRole || role == Qt::EditRole) || role == Qt::ForegroundRole) {
        switch (column) {
        case 0://name
            if (role == Qt::ForegroundRole) {
                return QVariant(QColor::fromRgb(112, 112, 112));
            }
            return QVariant(con->profile.name);
        case 1://type
            if (role == Qt::ForegroundRole) {
                return QVariant(QColor::fromRgb(112, 112, 112));
            }
            return QVariant(convertType(con->profile));
        case 2://server
            if (role == Qt::ForegroundRole) {
                return QVariant(QColor::fromRgb(112, 112, 112));
            }
            return QVariant(con->profile.serverAddress);
        case 3://status
            if (role == Qt::ForegroundRole) {
                return QVariant(convertStatusToColor(con->isRunning()));
            }
            return con->isRunning() ? QVariant("Connected")
                                    : QVariant("Disconnected");
        case 4://latency
            if (role == Qt::ForegroundRole) {
                return QVariant(convertLatencyToColor(con->profile.latency));
            } else if (role == Qt::DisplayRole) {
                return QVariant(convertLatencyToString(con->profile.latency));
            } else {
                return QVariant(con->profile.latency);
            }
        case 5://data usage (term)
            if (role == Qt::ForegroundRole) {
                return QVariant(QColor::fromRgb(112, 112, 112));
            } else if (role == Qt::DisplayRole) {
                return QVariant(convertBytesToHumanReadable(con->profile.currentDownloadUsage + con->profile.currentUploadUsage));
            } else {
                return QVariant(con->profile.currentDownloadUsage + con->profile.currentUploadUsage);
            }
        case 6://data usage (total)
            if (role == Qt::ForegroundRole) {
                return QVariant(QColor::fromRgb(112, 112, 112));
            } else if (role == Qt::DisplayRole) {
                return QVariant(convertBytesToHumanReadable(con->profile.totalDownloadUsage + con->profile.totalUploadUsage));
            } else {
                return QVariant(con->profile.totalDownloadUsage + con->profile.totalUploadUsage);
            }
        case 7://reset date
            if (role == Qt::ForegroundRole) {
                return QVariant(QColor::fromRgb(112, 112, 112));
            } else if (role == Qt::DisplayRole) {
                return QVariant(con->profile.nextResetDate.toString(Qt::SystemLocaleShortDate));
            } else {
                return QVariant(con->profile.nextResetDate);
            }
        case 8://last used
            if (role == Qt::ForegroundRole) {
                return QVariant(QColor::fromRgb(112, 112, 112));
            } else if (role == Qt::DisplayRole) {
                return QVariant(con->profile.lastTime.toString(Qt::SystemLocaleShortDate));
            } else {
                return QVariant(con->profile.lastTime);
            }
        default:
            return QVariant();
        }
    } else if (role == Qt::FontRole) {
        QFont font;
        font.setBold(con->isRunning());
        return QVariant(font);
    } else if (role == Qt::BackgroundRole) {
        if (con->isRunning())
            return QVariant(QColor::fromRgba(qRgba(0, 117, 219, 26)));
        else
            return QVariant(QColor::fromRgb(255, 255, 255));
    }

    return QVariant();
}

QString ConnectionItem::convertType(TQProfile profile)
{
    if (profile.type == "socks5")
        return "SOCKS5";
    else if (profile.type == "http")
        return "HTTP";
    else if (profile.type == "ss")
        return QString("SS / %1").arg(profile.plugin.split(".")[0].length() == 0 ? "NONE" : profile.plugin.split(".")[0].toUpper());
    else if (profile.type == "ssr")
        return QString("SSR / %1").arg(profile.obfs.toUpper());
    else if (profile.type == "trojan")
        return "TROJAN";
    else if (profile.type == "vmess")
        return QString("VMESS / %1").arg(profile.vmessSettings.network == "ws" ? "WEBSOCKET" : profile.vmessSettings.network.toUpper());
    else if (profile.type == "snell")
        return QString("SNELL / %1").arg(profile.obfs.toUpper());
    else
        return "UNKNOWN / ERROR";
}

QColor ConnectionItem::convertStatusToColor(const bool isRunning)
{
    if (isRunning) {
        return QColor::fromRgb(11, 155, 28);
    } else {
        return QColor::fromRgb(181, 181, 181);
    }
}

QColor ConnectionItem::convertLatencyToColor(const int latency)
{
    switch (latency) {
        case TQProfile::LATENCY_TIMEOUT:
            return QColor(Qt::red);
        case TQProfile::LATENCY_ERROR:
            return QColor(Qt::red);
        case TQProfile::LATENCY_UNKNOWN:
            return QColor(Qt::red);
        default:
            if (latency < 100) {
                return QColor::fromRgb(11, 155, 28, 255); //RGB come from @eejworks.
            } else if (latency >= 100 && latency < 200) {
                return QColor::fromRgb(4, 156, 213, 255); //RGB come from @eejworks.
            } else if (latency >= 200) {
                return QColor::fromRgb(255, 148, 0);
            }
    }

    return QColor();
}

QString ConnectionItem::convertLatencyToString(const int latency)
{
    QString latencyStr;
    switch (latency) {
    case TQProfile::LATENCY_TIMEOUT:
        latencyStr = tr("Timeout");
        break;
    case TQProfile::LATENCY_ERROR:
        latencyStr = tr("Error");
        break;
    case TQProfile::LATENCY_UNKNOWN:
        latencyStr = tr("Unknown");
        break;
    default:
        if (latency >= 1000) {
            latencyStr = QString::number(static_cast<double>(latency) / 1000.0)
                       + QStringLiteral(" ") + "s";
        } else {
            latencyStr = QString::number(latency) + QStringLiteral(" ") + "ms";
        }
    }
    return latencyStr;
}

QString ConnectionItem::convertBytesToHumanReadable(quint64 quot)
{
    int unitId = 0;
    quint64 rem = 0;
    for (; quot > 1024; ++unitId) {
        rem = quot % 1024;//the previous rem would be negligible
        quot /= 1024;
    }
    double output = static_cast<double>(quot)
                  + static_cast<double>(rem) / 1024.0;
    return QString("%1 %2").arg(output, 0, 'f', 2).arg(bytesUnits.at(unitId));
}

void ConnectionItem::testLatency()
{
    con->latencyTest();
    emit latencyChanged();
}

void ConnectionItem::clearTraffic()
{
    TQProfile p;
    p = con->getProfile();
    p.currentDownloadUsage = 0;
    p.currentUploadUsage = 0;
    p.totalDownloadUsage = 0;
    p.totalUploadUsage = 0;
    con->setProfile(p);
    emit dataUsageChanged(p.currentDownloadUsage + p.currentUploadUsage, p.totalDownloadUsage + p.totalUploadUsage);
}

Connection* ConnectionItem::getConnection()
{
    return con;
}

void ConnectionItem::onConnectionStateChanged(bool running)
{
    if (running) {
        emit message(con->getName() + " " + tr("connected"));
    } else {
        emit message(con->getName() + " " + tr("disconnected"));
    }
}

void ConnectionItem::onConnectionPingFinished(const int latency)
{
    if (latency == TQProfile::LATENCY_TIMEOUT) {
        emit message(con->getName() + " " + tr("timed out"));
    } else if (latency == TQProfile::LATENCY_ERROR) {
        emit message(con->getName() + " " + tr("latency test failed"));
    }
}

void ConnectionItem::onStartFailed()
{
    emit message(tr("Failed to start") + " " + con->getName());
}
