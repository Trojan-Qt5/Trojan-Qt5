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

#include "v2raystruct.h"
#include "trojangostruct.h"

struct TQProfile
{
    TQProfile();
    TQProfile(const QString &uri);

    TQProfile fromSocks5Uri(const std::string& socks5Uri) const;
    TQProfile fromHttpUri(const std::string& httpUri) const;
    TQProfile fromSSUri(const std::string& ssUri) const;
    TQProfile fromOldSSUri(const std::string& ssUri) const;
    TQProfile fromSSRUri(const std::string& trojanUri) const;
    TQProfile fromVmessUri(const std::string& vmessUri) const;
    TQProfile fromTrojanUri(const std::string& trojanUri) const;
    TQProfile fromSnellUri(const std::string& snellUri) const;

    QString toSocks5Uri() const;
    QString toHttpUri() const;
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
    bool tcpFastOpen;
    int latency;
    quint64 currentDownloadUsage;
    quint64 currentUploadUsage;
    quint64 totalDownloadUsage;
    quint64 totalUploadUsage;
    QDateTime lastTime; //last time this connection is used
    QDate nextResetDate; //next scheduled date to reset data usage
    // socks5/http only
    QString username;
    // ss/ssr/snell only
    QString method;
    QString protocol;
    QString protocolParam;
    QString obfs;
    QString obfsParam;
    QString plugin;
    QString pluginParam;
    // trojan only
    QString sni;
    bool verifyCertificate;
    bool reuseSession;
    bool sessionTicket;
    TrojanGoSettings trojanGoSettings;
    // vmess only
    QString uuid;
    int alterID;
    QString security;
    QString testsEnabled;
    VmessSettings vmessSettings;

    static const int LATENCY_TIMEOUT = -1;
    static const int LATENCY_ERROR = -2;
    static const int LATENCY_UNKNOWN = -3;
};
Q_DECLARE_METATYPE(TQProfile)

QDataStream& operator << (QDataStream &out, const TQProfile &p);
QDataStream& operator >> (QDataStream &in, TQProfile &p);

#endif // TQPROFILE_H
