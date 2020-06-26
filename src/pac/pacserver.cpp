#include "pacserver.h"
#include "qhttprequest.h"
#include "qhttpresponse.h"
#include "qhttpserver.h"
#include "confighelper.h"
#include "logger.h"
#include "utils.h"
#if defined (Q_OS_WIN)
#include <QApplication>
#endif

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
    ConfigHelper *conf = Utils::getConfigHelper();

    QString addr = conf->getInboundSettings().enableIpv6Support ? (conf->getInboundSettings().shareOverLan ? "::" : "::1") : (conf->getInboundSettings().shareOverLan ? "0.0.0.0" : "127.0.0.1");
    bool status = server.listen(QHostAddress(addr), conf->getInboundSettings().pacLocalPort);
    if (!status)
        Logger::warning(QString("[PAC Server] failed to listen on %1:%2, PAC will not be functional").arg(addr).arg(conf->getInboundSettings().pacLocalPort));
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
