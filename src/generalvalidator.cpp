#include "tqprofile.h"
#include "generalvalidator.h"

#include <QDebug>

bool GeneralValidator::validateSS(const QString &input)
{
    bool valid = true;
    try {
        TQProfile tqprofile;
        tqprofile.fromSSUri(input.toStdString());
    } catch(const std::exception&) {
        valid = false;
    }
    return valid;
}

bool GeneralValidator::validateSSR(const QString &input)
{
    bool valid = true;
    try {
        TQProfile tqprofile;
        tqprofile.fromSSRUri(input.toStdString());
    } catch(const std::exception&) {
        valid = false;
    }
    return valid;
}

bool GeneralValidator::validateTrojan(const QString &input)
{
    bool valid = true;
    try {
        TQProfile tqprofile;
        tqprofile.fromTrojanUri(input.toStdString());
    } catch(const std::exception& e) {
        qDebug() << e.what();
        valid = false;
    }
    return valid;
}

bool GeneralValidator::validatePort(const QString &port)
{
    bool ok;
    port.toUShort(&ok);
    return ok;
}
