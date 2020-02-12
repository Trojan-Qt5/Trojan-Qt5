#include "portvalidator.h"
#include "trojanvalidator.h"

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
