#include "confighelper.h"
#include "portvalidator.h"
#include "generalvalidator.h"
#include <QApplication>
#include <QTcpServer>
#include <QDir>

PortValidator::PortValidator(QObject *parent)
    : QValidator(parent)
{}

QValidator::State PortValidator::validate(QString &input, int &) const
{
    if (GeneralValidator::validatePort(input)) {
        return Acceptable;
    }
    else
        return Invalid;
}

QString PortValidator::isInUse(int port)
{
#ifdef Q_OS_WIN
    QString configFile = qApp->applicationDirPath() + "/config.ini";
#else
    QDir configDir = QDir::homePath() + "/.config/trojan-qt5";
    QString configFile = configDir.absolutePath() + "/config.ini";
#endif
    ConfigHelper *conf = new ConfigHelper(configFile);
    QString addr = conf->getInboundSettings()["enableIpv6Support"].toBool() ? (conf->getInboundSettings()["shareOverLan"].toBool() ? "::" : "::1") : (conf->getInboundSettings()["shareOverLan"].toBool() ? "0.0.0.0" : "127.0.0.1");

    QTcpServer *server = new QTcpServer(); // Use TcpServer to listen to the port specified
    bool status = server->listen(QHostAddress(addr), port);
    if (status) {
        server->close();
        server->deleteLater();
        return "";
    } else {
        // There is something listening on it or error encourted.
        server->close();
        server->deleteLater();
        return server->errorString();
    }
}
