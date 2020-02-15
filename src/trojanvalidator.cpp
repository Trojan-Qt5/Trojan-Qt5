#include "tqprofile.h"
#include "trojanvalidator.h"

bool TrojanValidator::validate(const QString &input)
{
    bool valid = true;
    try {
        TQProfile tqprofile;
        tqprofile.fromUri(input.toStdString());
    } catch(const std::exception&) {
        valid = false;
    }
    return valid;
}

bool TrojanValidator::validatePort(const QString &port)
{
    bool ok;
    port.toUShort(&ok);
    return ok;
}
