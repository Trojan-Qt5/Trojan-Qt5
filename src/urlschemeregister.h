/*
This file is part of Telegram Desktop,
the official desktop application for the Telegram messaging service.

Copyright (c) 2014-2020 The Telegram Desktop Authors.

Telegram Desktop is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

It is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

In addition, as a special exception, the copyright holders give permission
to link the code of portions of this program with the OpenSSL library.

More information about the Telegram project: https://telegram.org

Full license: https://github.com/telegramdesktop/tdesktop/blob/master/LICENSE
*/

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
    bool RegisterLegacy(Mode mode, const UrlSchemeDescriptor &d);
    void UnregisterLegacy(const UrlSchemeDescriptor &d);
    bool RegisterDefaultProgram(Mode mode, const UrlSchemeDescriptor &d);
    void UnregisterDefaultProgram(const UrlSchemeDescriptor &d);
    bool FullRegister(Mode mode, const UrlSchemeDescriptor &descriptor);
    bool CheckUrlScheme(const UrlSchemeDescriptor &descriptor);
    void RegisterUrlScheme(const UrlSchemeDescriptor &descriptor);
    void UnregisterUrlScheme(const UrlSchemeDescriptor &descriptor);
    void RegisterAllUrlScheme();
    void UnregisterAllUrlScheme();
    bool CheckAllUrlScheme();
};

#endif // URLSCHEMEREGISTER_H
