#include "subscribemanager.h"
#include "generalvalidator.h"
#include "logger.h"
#include <QNetworkAccessManager>
#include <QNetworkProxy>
#include <QNetworkReply>
#include <QEventLoop>
#include "mainwindow.h"

SubscribeManager::SubscribeManager(MainWindow *w, ConfigHelper *ch) : window(w), helper(ch)
{
    thread = new QThread();
    this->moveToThread(thread);
    connect(thread, SIGNAL(started()), this, SLOT(updateAllSubscribes()));
    connect(this, &SubscribeManager::addUri, window, &MainWindow::onAddURIFromSubscribe);
}

void SubscribeManager::setUseProxy(bool value)
{
    useProxy = value;
}

QString SubscribeManager::checkUpdate(QString url)
{
    Logger::debug(QString("[Subscribe] Subscribe Link: %1").arg(url));
    QNetworkAccessManager* manager = new QNetworkAccessManager();
    QNetworkRequest request(url);
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
    request.setAttribute(QNetworkRequest::HTTP2AllowedAttribute, true);
    request.setRawHeader("User-Agent", helper->getUpdateUserAgent().toUtf8().data());
    if (useProxy) {
        QNetworkProxy proxy;
        proxy.setType(QNetworkProxy::Socks5Proxy);
        proxy.setHostName("127.0.0.1");
        proxy.setPort(helper->getSocks5Port());
        manager->setProxy(proxy);
    }
    QNetworkReply* reply = manager->get(request);
    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
    Logger::debug(QString("[Subscribe] Request Status %1").arg(QString::number(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt())));
    return QString::fromUtf8(reply->readAll());
}

void SubscribeManager::updateAllSubscribesWithThread()
{
    thread->start();
}

void SubscribeManager::updateAllSubscribes()
{
    Logger::debug(QString("[Subscribe] Check subscribe clicked, use proxy: %1").arg(useProxy));
    QList<TQSubscribe> subscribes = helper->readSubscribes();
    for (int i = 0; i < subscribes.size(); i++) {
        subscribes[i].lastUpdateTime = QDateTime::currentDateTime().toTime_t() - QDateTime::fromString("1970-01-01T00:00:00").toTime_t();
        QString data = checkUpdate(subscribes[i].url);
        QByteArray decodeArray = QByteArray::fromBase64(data.toUtf8().data());
        QString decodeRes = QUrl::fromPercentEncoding(decodeArray); // remove percentage in uri
        decodeRes = decodeRes.replace("\\r", "\r"); // change \\r to \r
        decodeRes = decodeRes.replace("\\n", "\n"); // change \\n to \n
        decodeRes = decodeRes.replace("\r\n", "\n"); // change \r\n to \n
        QStringList list = decodeRes.split("\n");
        Logger::debug(QString("[Subscribe] Read %1 Servers").arg(list.length()));
        for (int x = 0; x < list.length(); x++) {
            if (list[x].isEmpty()) {
                continue;
            }
            if (GeneralValidator::validateSS(list[x]) || GeneralValidator::validateSSR(list[x]) || GeneralValidator::validateVmess(list[x]) || GeneralValidator::validateTrojan(list[x])) {
                if (!isFiltered(TQProfile(list[x]).name) && (x < helper->getMaximumSubscribe() || helper->getMaximumSubscribe() == 0)) {
                    emit addUri(list[x]);
                }
            }
            else {
                Logger::debug(QString("[Subscribe] Server %1 is not valid").arg(list[x]));
            }
        }
        subscribes[i].groupName = TQProfile(list[0]).group;
    }
    helper->saveSubscribes(subscribes);
}

bool SubscribeManager::isFiltered(QString name)
{
    QStringList keywords = helper->getFilterKeyword().split(",");
    if (keywords.size() == 1 && keywords[0] == "")
        return false;

    for (int i = 0; i < keywords.size(); i++)
        if (name.contains(keywords[i])) {
            Logger::debug(QString("[Subscribe] Server %1 meet the filter key word %2, skipping").arg(name).arg(keywords[i]));
            return true;
        }

    return false;
}
