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
#include "confighelper.h"
#include "utils.h"

#include <QNetworkAccessManager>
#include <QNetworkProxyFactory>
#include <QEventLoop>
#include <QNetworkReply>

AddressTester::AddressTester(const QHostAddress &_address,
                             const uint16_t &_port,
                             QObject *parent) :
    QObject(parent),
    m_address(_address),
    m_port(_port)
{
    QNetworkProxyFactory::setUseSystemConfiguration(false); //fix when connected to server there is only one 1ms.

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
    ConfigHelper *helper = Utils::getConfigHelper();
    if (helper->getTestSettings().method == 0)
        startTcpPingTest(timeout);
    else if (helper->getTestSettings().method == 1)
        startRealPingTest(timeout);
}

void AddressTester::startTcpPingTest(int timeout)
{
    connectToServer(timeout);
}

void AddressTester::startRealPingTest(int timeout)
{
    ConfigHelper *helper = Utils::getConfigHelper();
    QNetworkAccessManager* manager = new QNetworkAccessManager();
    QNetworkRequest request(helper->getTestSettings().latencyTestUrl);
    request.setRawHeader("User-Agent", helper->getSubscribeSettings().updateUserAgent.toUtf8().data());
    QNetworkProxy proxy;
    proxy.setType(QNetworkProxy::Socks5Proxy);
    proxy.setHostName("127.0.0.1");
    proxy.setPort(helper->getInboundSettings().socks5LocalPort);
    manager->setProxy(proxy);

    m_time = QTime::currentTime();
    QNetworkReply* reply = manager->get(request);
    QEventLoop loop;
    connect(&m_timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    m_timer.start(timeout);
    loop.exec();

    if (m_timer.isActive()) {
        m_timer.stop();
        if (reply->error() != QNetworkReply::NoError || reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() != 204) {
            m_timer.stop();
            emit lagTestFinished(LAG_ERROR);
        } else {
            m_timer.stop();
            emit lagTestFinished(m_time.msecsTo(QTime::currentTime()));
        }
    } else {
        disconnect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        reply->abort();
        reply->deleteLater();
    }
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
