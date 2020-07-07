/*
 * Copyright (C) 2015-2016 Symeon Huang <hzwhuang@gmail.com>
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

#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include <QHostInfo>
#include <QHostAddress>
#include <memory>
#include "httpproxy.h"
#include "pachelper.h"
#include "ssthread.h"
#include "SSRThread.hpp"
#include "tun2socksthread.h"
#include "v2raythread.h"
#include "trojanthread.h"
#include "snellthread.h"
#include "systemproxyhelper.h"
#include "tqprofile.h"
#include "routetablehelper.h"
#include "ssgoapi.h"
#include "v2rayapi.h"
#include "trojangoapi.h"
#include "snellgoapi.h"

class Connection : public QObject
{
    Q_OBJECT
public:
    Connection(QObject *parent = 0);
    Connection(const TQProfile &_profile, QObject *parent = 0);
    Connection(QString uri, QObject *parent = 0);
    ~Connection();

    Connection(const Connection&) = delete;
    //Connection(Connection&&) = default;

    const TQProfile &getProfile() const;
    const QString &getName() const;
    QByteArray getURI(QString type) const;
    void setProfile(TQProfile p);
    bool isValid() const;
    const bool &isRunning() const;
    void latencyTest();
    static void onTrojanConnectionDestoryed(Connection& connection, const uint64_t &, const uint64_t &);

signals:
    void stateChanged(bool started);
    void latencyAvailable(const int);
    void newLogAvailable(const QString &);
    void dataUsageChanged(const quint64 &current, const quint64 &total);
    void dataTrafficAvailable(const QList<quint64> data);
    void startFailed();
    void connectionChanged();
    void connectionSwitched();


public slots:
    void start();
    void stop();
    void onStartFailed();
    void onNotifyConnectionChanged();

private:
    HttpProxy *http;
    SSThread *ss;
    std::unique_ptr<SSRThread> ssr;
    Tun2socksThread *tun2socks;
    V2rayThread *v2ray;
    TrojanThread *trojan;
    SnellThread *snell;
    RouteTableHelper *rhelper;
    SSGoAPI *ssGoAPI;
    V2rayAPI *v2rayAPI;
    TrojanGoAPI *trojanGoAPI;
    SnellGoAPI *snellGoAPI;

    TQProfile profile;
    bool running;

    void testAddressLatency(const QHostAddress &addr);

    friend class Socks5EditDialog;
    friend class HttpEditDialog;
    friend class SSEditDialog;
    friend class SSREditDialog;
    friend class VmessEditDialog;
    friend class TrojanEditDialog;
    friend class SnellEditDialog;
    friend class NaiveProxyEditDialog;
    friend class ConfigHelper;
    friend class StatusDialog;
    friend class ConnectionItem;

private slots:
    void onServerAddressLookedUp(const QHostInfo &host);
    void onLatencyAvailable(const int);
    void onNewBytesTransmitted(const quint64 &, const quint64 &);
    void onNewV2RayBytesTransmitted(const quint64 &pu, const quint64 &pd, const quint64 &du, const quint64 &dd);

};
Q_DECLARE_METATYPE(Connection*)

#endif // CONNECTION_H
