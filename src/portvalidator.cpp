#include "portvalidator.h"
#include "trojanvalidator.h"

#include <QTcpSocket>

PortValidator::PortValidator(QObject *parent)
    : QValidator(parent)
{}

QValidator::State PortValidator::validate(QString &input, int &) const
{
    if (TrojanValidator::validatePort(input)) {
        return Acceptable;
    }
    else
        return Invalid;
}

bool PortValidator::isInUse(int port)
{
    /** Use TcpSocket to connect to the port. */
    QTcpSocket *socket = new QTcpSocket();
    socket->connectToHost("127.0.0.1", port);
    if (!socket->waitForConnected(300)) {
        return false;
    } else {
        return true;
    }
}
