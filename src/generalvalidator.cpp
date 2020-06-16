#include "tqprofile.h"
#include "generalvalidator.h"

#include <QDebug>

bool GeneralValidator::validateAll(const QString &input)
{
    if (validateSocks5(input) || validateHttp(input) || validateSS(input) || validateSSR(input) || validateVmess(input) || validateTrojan(input) || validateSnell(input))
        return true;
    return false;
}

bool GeneralValidator::validateSocks5(const QString &input)
{
    bool valid = true;
    try {
        TQProfile tqprofile;
        tqprofile.fromSocks5Uri(input.toStdString());
    } catch(const std::exception&) {
        valid = false;
    }
    return valid;
}

bool GeneralValidator::validateHttp(const QString &input)
{
    bool valid = true;
    try {
        TQProfile tqprofile;
        tqprofile.fromHttpUri(input.toStdString());
    } catch(const std::exception&) {
        valid = false;
    }
    return valid;
}

bool GeneralValidator::validateSS(const QString &input)
{
    bool valid = true;
    try {
        TQProfile tqprofile;
        tqprofile.fromSSUri(input.toStdString());
    } catch(const std::exception&) {
        valid = validateSSOld(input);
    }
    return valid;
}

bool GeneralValidator::validateSSOld(const QString &input)
{
    bool valid = true;
    try {
        TQProfile tqprofile;
        tqprofile.fromOldSSUri(input.toStdString());
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

bool GeneralValidator::validateVmess(const QString &input)
{
    bool valid = true;
    try {
        TQProfile tqprofile;
        tqprofile.fromVmessUri(input.toStdString());
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
    } catch(const std::exception&) {
        valid = false;
    }
    return valid;
}

bool GeneralValidator::validateSnell(const QString &input)
{
    bool valid = true;
    try {
        TQProfile tqprofile;
        tqprofile.fromSnellUri(input.toStdString());
    } catch(const std::exception&) {
        valid = false;
    }
    return valid;
}

bool GeneralValidator::validatePort(const QString &port)
{
    bool ok;
    if (port.isEmpty())
        return true;
    port.toUShort(&ok);
    return ok;
}
