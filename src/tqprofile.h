/*
 * Copyright (C) 2014-2016 Symeon Huang <hzwhuang@gmail.com>
 *
 * shadowsocks-qt5 is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * shadowsocks-qt5 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libQtShadowsocks; see the file LICENSE. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef TQPROFILE_H
#define TQPROFILE_H

#include <QDataStream>
#include <QDate>
#include <QDateTime>
#include <QUrl>
#include <QJsonObject>

struct TQProfile
{
    TQProfile();
    TQProfile(const QString &uri);

    TQProfile fromSSUri(const std::string& ssUri) const;
    TQProfile fromOldSSUri(const std::string& ssUri) const;
    TQProfile fromSSRUri(const std::string& trojanUri) const;
    TQProfile fromVmessUri(const std::string& vmessUri) const;
    TQProfile fromTrojanUri(const std::string& trojanUri) const;
    QString toSSUri() const;
    QString toSSRUri() const;
    QString toVmessUri() const;
    QString toTrojanUri() const;
    QString toSnellUri() const;

    bool equals(const TQProfile &profile) const;

    bool autoStart;
    quint16 serverPort;
    QString type;
    QString group;
    QString name;
    QString serverAddress;
    QString password;
    int latency;
    quint64 currentUsage;
    quint64 totalUsage;
    QDateTime lastTime; //last time this connection is used
    QDate nextResetDate; //next scheduled date to reset data usage
    bool mux;
    int muxConcurrency;
    // ss/ssr/snell only
    QString method;
    QString protocol;
    QString protocolParam;
    QString obfs;
    QString obfsParam;
    QString plugin;
    QString pluginParam;
    // trojan only
    bool verifyCertificate;
    bool verifyHostname;
    bool reuseSession;
    bool sessionTicket;
    bool reusePort;
    bool tcpFastOpen;
    bool websocket;
    bool websocketDoubleTLS;
    QString sni;
    QString websocketPath;
    QString websocketHostname;
    QString websocketObfsPassword;
    // vmess only
    QString uuid;
    int alterID;
    QString security;
    QJsonObject vmessSettings;

    static const int LATENCY_TIMEOUT = -1;
    static const int LATENCY_ERROR = -2;
    static const int LATENCY_UNKNOWN = -3;
};
Q_DECLARE_METATYPE(TQProfile)

QDataStream& operator << (QDataStream &out, const TQProfile &p);
QDataStream& operator >> (QDataStream &in, TQProfile &p);

#endif // TQPROFILE_H
