#ifndef URLSCHEMEREGISTER_H
#define URLSCHEMEREGISTER_H

#include <QObject>
#include "Windows.h"
#include "winreg.h"

class UrlSchemeRegister: public QObject
{
    enum class Mode {
        Check,
        Write,
    };

public:
    UrlSchemeRegister();
    bool OpenRegKey(Mode mode, const QString &key, PHKEY rkey);
    bool RemoveRegKey(const QString &key);
    bool SetKeyValue(Mode mode, HKEY rkey, const QString &name, QString value);
    bool RemoveKeyValue(HKEY rkey, const QString &name);
    bool FullRegister(Mode mode, const UrlSchemeDescriptor &descriptor);
    bool CheckUrlScheme(const UrlSchemeDescriptor &descriptor);
    void RegisterUrlScheme(const UrlSchemeDescriptor &descriptor);
    void UnregisterUrlScheme(const UrlSchemeDescriptor &descriptor);
};

#endif // URLSCHEMEREGISTER_H
