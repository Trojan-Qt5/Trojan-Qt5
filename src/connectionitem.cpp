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
    return 8;
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
        case 1://server
            if (role == Qt::ForegroundRole) {
                return QVariant(QColor::fromRgb(112, 112, 112));
            }
            return QVariant(con->profile.serverAddress);
        case 2://status
            if (role == Qt::ForegroundRole) {
                return QVariant(convertStatusToColor(con->isRunning()));
            }
            return con->isRunning() ? QVariant("Connected")
                                    : QVariant("Disconnected");
        case 3://latency
            if (role == Qt::ForegroundRole) {
                return QVariant(convertLatencyToColor(con->profile.latency));
            } else if (role == Qt::DisplayRole) {
                return QVariant(convertLatencyToString(con->profile.latency));
            } else {
                return QVariant(con->profile.latency);
            }
        case 4://data usage (term)
            if (role == Qt::ForegroundRole) {
                return QVariant(QColor::fromRgb(112, 112, 112));
            } else if (role == Qt::DisplayRole) {
                return QVariant(convertBytesToHumanReadable(con->profile.currentUsage));
            } else {
                return QVariant(con->profile.currentUsage);
            }
        case 5://data usage (total)
            if (role == Qt::ForegroundRole) {
                return QVariant(QColor::fromRgb(112, 112, 112));
            } else if (role == Qt::DisplayRole) {
                return QVariant(convertBytesToHumanReadable(con->profile.totalUsage));
            } else {
                return QVariant(con->profile.totalUsage);
            }
        case 6://reset date
            if (role == Qt::ForegroundRole) {
                return QVariant(QColor::fromRgb(112, 112, 112));
            } else if (role == Qt::DisplayRole) {
                return QVariant(con->profile.nextResetDate.toString(Qt::SystemLocaleShortDate));
            } else {
                return QVariant(con->profile.nextResetDate);
            }
        case 7://last used
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
                /** RGB come from @eejworks. */
                return QColor::fromRgb(11, 155, 28, 255);
            } else if (latency > 100 && latency < 200) {
                /** RGB come from @eejworks. */
                return QColor::fromRgb(4, 156, 213, 255);
            } else if (latency > 200) {
                return QColor::fromRgb(255, 148, 0);
            }
    }
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
                       + QStringLiteral(" ") + tr("s");
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
