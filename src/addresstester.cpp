/*
 * addresstester.cpp - the source file of AddressTester class
 *
 * Simple way to test the connection's latency.
 * Since it's a just socket connection without any data transfer,
 * the remote doesn't need to be a shadowsocks server.
 *
 * Copyright (C) 2014-2016 Symeon Huang <hzwhuang@gmail.com>
 *
 * This file is part of the libQtShadowsocks.
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

#include "addresstester.h"

#include <QNetworkProxyFactory>

AddressTester::AddressTester(const QHostAddress &_address,
                             const uint16_t &_port,
                             QObject *parent) :
    QObject(parent),
    m_address(_address),
    m_port(_port)
{
    /** Fix when connected to server there is only one 1ms. */
    QNetworkProxyFactory::setUseSystemConfiguration(false);

    m_timer.setSingleShot(true);
    m_time = QTime::currentTime();
    m_socket.setSocketOption(QAbstractSocket::LowDelayOption, 1);

    connect(&m_timer, &QTimer::timeout, this, &AddressTester::onTimeout);
    connect(&m_socket, &QTcpSocket::connected, this, &AddressTester::onConnected);
    connect(&m_socket, &QTcpSocket::readyRead,
            this, &AddressTester::onSocketReadyRead);
    connect(&m_socket,
            static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>
            (&QTcpSocket::error),
            this,
            &AddressTester::onSocketError);
}

void AddressTester::connectToServer(int timeout)
{
    m_time = QTime::currentTime();
    m_timer.start(timeout);
    m_socket.connectToHost(m_address, m_port);
}

void AddressTester::startLagTest(int timeout)
{
    connectToServer(timeout);
}

void AddressTester::onTimeout()
{
    m_socket.abort();
    emit lagTestFinished(LAG_TIMEOUT);
}

void AddressTester::onSocketError(QAbstractSocket::SocketError)
{
    m_timer.stop();
    m_socket.abort();
    emit testErrorString(m_socket.errorString());
    emit lagTestFinished(LAG_ERROR);
}

void AddressTester::onConnected()
{
    m_timer.stop();
    emit lagTestFinished(m_time.msecsTo(QTime::currentTime()));
    m_socket.abort();
}

void AddressTester::onSocketReadyRead()
{
    m_socket.abort();
}
