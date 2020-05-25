#include "pacserver.h"

#include <QDir>
#include <QFile>
#if defined (Q_OS_WIN)
#include <QCoreApplication>
#endif

#include "qhttprequest.h"
#include "qhttpresponse.h"
#include "qhttpserver.h"
#include "confighelper.h"

PACServer::PACServer()
{
    connect(&server, &QHttpServer::newRequest, this, &PACServer::onHandleRequest);
}

PACServer::~PACServer()
{
    server.close();
}

void PACServer::listen()
{
#ifdef Q_OS_WIN
    QString configFile = qApp->applicationDirPath() + "/config.ini";
#else
    QDir configDir = QDir::homePath() + "/.config/trojan-qt5";
    QString configFile = configDir.absolutePath() + "/config.ini";
#endif
    ConfigHelper *conf = new ConfigHelper(configFile);

    QString addr = conf->getInboundSettings()["enableIpv6Support"].toBool() ? (conf->getInboundSettings()["shareOverLan"].toBool() ? "::" : "::1") : (conf->getInboundSettings()["shareOverLan"].toBool() ? "0.0.0.0" : "127.0.0.1");
    server.listen(QHostAddress(addr), conf->getInboundSettings()["pacLocalPort"].toInt());
}

QString PACServer::loadPACFile()
{
#if defined (Q_OS_WIN)
    QFile file(qApp->applicationDirPath() + "/pac/proxy.pac");
#else
    QFile file(QDir::homePath() + "/.config/trojan-qt5/pac/proxy.pac");
#endif
    file.open(QIODevice::ReadOnly);
    QString data = file.readAll();
    file.close();
    return data;
}

void PACServer::onHandleRequest(QHttpRequest *req, QHttpResponse *rsp)
{
    if (req->path() == "/proxy.pac" && req->method() == QHttpRequest::HTTP_GET) {
        rsp->setHeader("Server", "Trojan-Qt5");
        rsp->setHeader("Content-Type", "application/x-ns-proxy-autoconfig");
        rsp->setHeader("Connection", "close");
        rsp->writeHead(200);
        rsp->end(loadPACFile().toUtf8().data());
    } else {
        rsp->setHeader("Server", "Trojan-Qt5");
        rsp->setHeader("Connection", "close");
        rsp->writeHead(404);
        rsp->end();
    }
}
