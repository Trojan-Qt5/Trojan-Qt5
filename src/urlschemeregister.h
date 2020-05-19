#ifndef URLSCHEMEREGISTER_H
#define URLSCHEMEREGISTER_H

#include <QObject>
#include <QApplication>
#include "Windows.h"
#include "winreg.h"

class UrlSchemeRegister: public QObject
{
    enum class Mode {
        Check,
        Write,
    };

    struct UrlSchemeDescriptor {
        QString executable = qApp->applicationFilePath(); // Full path.
        QString arguments; // Additional arguments.
        QString protocol; // 'myprotocol'
        QString protocolName = "Trojan Qt5 Url Scheme Link"; // "My Protocol Link"
        QString shortAppName = "trojanqt5"; // "myapp"
        QString iconFileName; // "myapplication"
        bool forceUpdateIcon = false;
        bool skipDesktopFileRegistration = false;
        QString longAppName = "TrojanQt5"; // "MyApplication"
        QString displayAppName = "Trojan-Qt5"; // "My Application"
        QString displayAppDescription = "A cross platform ss/ssr/vmess/trojan client"; // "My Nice Application"
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
