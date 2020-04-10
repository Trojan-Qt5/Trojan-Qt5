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

struct TQProfile
{
    TQProfile();
    TQProfile(const QString &uri);

    TQProfile fromUri(const std::string& trojanUri) const;
    QString toUri() const;

    bool autoStart;
    bool isSubscribe;
    quint16 serverPort;
    QString name;
    QString serverAddress;
    QString password;
    QString sni;
    bool verifyCertificate;
    bool verifyHostname;
    bool reuseSession;
    bool sessionTicket;
    bool reusePort;
    bool tcpFastOpen;
    int latency;
    quint64 currentUsage;
    quint64 totalUsage;
    QDateTime lastTime;//last time this connection is used
    QDate nextResetDate;//next scheduled date to reset data usage

    static const int LATENCY_TIMEOUT = -1;
    static const int LATENCY_ERROR = -2;
    static const int LATENCY_UNKNOWN = -3;
};
Q_DECLARE_METATYPE(TQProfile)

QDataStream& operator << (QDataStream &out, const TQProfile &p);
QDataStream& operator >> (QDataStream &in, TQProfile &p);

#endif // TQPROFILE_H
