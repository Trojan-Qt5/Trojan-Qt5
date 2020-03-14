#include "subscribemanager.h"
#include "trojanvalidator.h"
#include <QNetworkAccessManager>
#include <QNetworkProxy>
#include <QNetworkReply>
#include <QEventLoop>

SubscribeManager::SubscribeManager(ConfigHelper *ch, QObject *parent) : QObject(parent), helper(ch)
{
}

QString SubscribeManager::checkUpdate(QString url, bool useProxy)
{
    QNetworkAccessManager* manager = new QNetworkAccessManager();
    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", QString("Trojan-Qt5/%1").arg(APP_VERSION).toUtf8());
    if (useProxy) {
        QNetworkProxy proxy;
        proxy.setType(QNetworkProxy::Socks5Proxy);
        proxy.setHostName(helper->getSocks5Address());
        proxy.setPort(helper->getSocks5Port());
        manager->setProxy(proxy);
    }
    QNetworkReply* reply = manager->sendCustomRequest(request, "GET", "");
    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
    return QString::fromUtf8(reply->readAll());
}

void SubscribeManager::updateAllSubscribes(bool useProxy)
{
    QList<TQSubscribe> subscribes = helper->readSubscribes();
    for (int i = 0; i < subscribes.size(); i++) {
        QString data = checkUpdate(subscribes[i].url, useProxy);
        QString decodeRes = QByteArray::fromBase64(data.toLocal8Bit().data());
        QStringList list = decodeRes.split("\\r\\n");
        for (int i = 0; i< list.length(); i++)
            if (TrojanValidator::validate(list[i]))
                emit addUri(list[i]);
    }
}
