#include "pacserver.h"

#include <QDir>
#include <QFile>
#include <QCoreApplication>

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
    QString configFile = QCoreApplication::applicationDirPath() + "/config.ini";
#else
    QDir configDir = QDir::homePath() + "/.config/trojan-qt5";
    QString configFile = configDir.absolutePath() + "/config.ini";
#endif
    ConfigHelper *conf = new ConfigHelper(configFile);

    QString addr = conf->isEnableIpv6Support() ? (conf->isShareOverLan() ? "::" : "::1") : (conf->isShareOverLan() ? "0.0.0.0" : "127.0.0.1");
    server.listen(QHostAddress(addr), conf->getPACPort());
}

QString PACServer::loadPACFile()
{
#if defined (Q_OS_WIN)
    QFile file(QApplication::applicationDirPath() + "/pac/proxy.pac)";
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
    }
}
