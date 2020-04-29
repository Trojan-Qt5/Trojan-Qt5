#include "portvalidator.h"
#include "generalvalidator.h"
#include "logger.h"

#include <QTcpServer>

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
    QTcpServer *server = new QTcpServer(); // Use TcpServer to listen to the port specified
    bool status = server->listen(QHostAddress::LocalHost, port);
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
