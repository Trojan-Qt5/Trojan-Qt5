/*
 * addresstester.h - the header file of AddressTester class
 *
 * perform non-blocking address tests
 *
 * Copyright (C) 2015-2018 Symeon Huang <hzwhuang@gmail.com>
 * Copyright (C) 2019-2020 TheWanderingCoel <thewanderingcoel@protonmail.com>
 *
 * This file is part of the libQtShadowsocks and modified for Trojan-qt5 usage.
 *
 * libQtShadowsocks is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * libQtShadowsocks is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libQtShadowsocks; see the file LICENSE. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef ADDRESSTESTER_H
#define ADDRESSTESTER_H

#include <QHostAddress>
#include <QTcpSocket>
#include <QTime>
#include <QTimer>

// This class is only meaningful for client-side applications
class AddressTester : public QObject
{
    Q_OBJECT
public:
    AddressTester(const QHostAddress &server_address,
                  const uint16_t &server_port,
                  QObject *parent = 0);

    AddressTester(const AddressTester &) = delete;

    static const int LAG_TIMEOUT = -1;
    static const int LAG_ERROR = -2;

    /*
     * Connectivity test will try to establish a shadowsocks connection with
     * the server. The result is passed by signal connectivityTestFinished().
     * If the server times out, the connectivity will be passed as false.
     *
     * Calling this function does lag (latency) test as well. Therefore, it's
     * the recommended way to do connectivity and latency test with just one
     * function call.
     *
     * Don't call the same AddressTester instance's startConnectivityTest()
     * and startLagTest() at the same time!
     */

signals:
    void lagTestFinished(int);
    void testErrorString(const QString &);

public slots:
    /*
     * The lag test only tests if the server port is open and listeninig
     * bind lagTestFinished() signal to get the test result
     */
    void startLagTest(int timeout = 3000);//3000 msec by default
    void startTcpPingTest(int timeout);
    void startRealPingTest(int timeout);

private:
    QHostAddress m_address;
    uint16_t m_port;
    QTime m_time;
    QTcpSocket m_socket;
    QTimer m_timer;

    void connectToServer(int timeout);

private slots:
    void onTimeout();
    void onSocketError(QAbstractSocket::SocketError);
    void onConnected();
    void onSocketReadyRead();
};

#endif // ADDRESSTESTER_H
