#include "systemproxyhelper.h"
#include <stdio.h>
#include <QDebug>
#if defined (Q_OS_WIN)
#include <Windows.h>
#include "sysproxy/windows.h"
#endif

SystemProxyHelper::SystemProxyHelper()
{}

SystemProxyHelper::~SystemProxyHelper()
{
    setSystemProxy(TQProfile(), false);
}

void SystemProxyHelper::setSystemProxy(TQProfile profile, bool enable)
{
#if defined(Q_OS_WIN)
    if (enable && profile.dualMode) {
        QString server = profile.localAddress + ":" + QString::number(profile.localHttpPort);
        LPTSTR serverString = (LPTSTR) server.utf16();
        int status = setProxy(true, serverString);
    } else {
        int status = setProxy(false, NULL);
    }
#elif defined (Q_OS_MAC)
    if (enable && profile.dualMode) {
        popen(QString("networksetup -setwebproxy \"Wi-Fi\" %1 %2").arg(profile.localAddress).arg(QString::number(profile.localHttpPort)).toStdString().c_str(), "r");
        popen(QString("networksetup -setsecurewebproxy \"Wi-Fi\" %1 %2").arg(profile.localAddress).arg(QString::number(profile.localHttpPort)).toStdString().c_str(), "r");
        popen(QString("networksetup -setsocksfirewallproxy \"Wi-Fi\" %1 %2").arg(profile.localAddress).arg(QString::number(profile.localPort)).toStdString().c_str(), "r");
    } else if (enable) {
        popen(QString("networksetup -setsocksfirewallproxy \"Wi-Fi\" %1 %2").arg(profile.localAddress).arg(QString::number(profile.localPort)).toStdString().c_str(), "r");
    } else {
        popen("networksetup -setwebproxystate \"Wi-Fi\" off", "r");
        popen("networksetup -setsecurewebproxystate \"Wi-Fi\" off", "r");
        popen("networksetup -setsocksfirewallproxystate \"Wi-Fi\" off", "r");
    }
#elif defined (Q_OS_LINUX)
    if (system("gsettings --version > /dev/null") == 0) {
        if (enable && profile.dualMode) {
            popen(QString("gsettings set org.gnome.system.proxy mode manual").toStdString().c_str(), "r");
            popen(QString("gsettings set org.gnome.system.proxy.http host %1").arg(profile.localAddress).toStdString().c_str(), "r");
            popen(QString("gsettings set org.gnome.system.proxy.http port %1").arg(QString::number(profile.localHttpPort)).toStdString().c_str(), "r");
            popen(QString("gsettings set org.gnome.system.proxy.https host %1").arg(profile.localAddress).toStdString().c_str(), "r");
            popen(QString("gsettings set org.gnome.system.proxy.https port %1").arg(QString::number(profile.localHttpPort)).toStdString().c_str(), "r");
        } else if (enable) {
            popen(QString("gsettings set org.gnome.system.proxy mode manual").toStdString().c_str(), "r");
            popen(QString("gsettings set org.gnome.system.proxy.socks host %1").arg(profile.localAddress).toStdString().c_str(), "r");
            popen(QString("gsettings set org.gnome.system.proxy.socks port %1").arg(QString::number(profile.localPort)).toStdString().c_str(), "r");
        } else {
            popen(QString("gsettings set org.gnome.system.proxy.mode none").toStdString().c_str(), "r");
            popen(QString("gsettings set org.gnome.system.proxy.http host ''").toStdString().c_str(), "r");
            popen(QString("gsettings set org.gnome.system.proxy.http port 0").toStdString().c_str(), "r");
            popen(QString("gsettings set org.gnome.system.proxy.https host ''").toStdString().c_str(), "r");
            popen(QString("gsettings set org.gnome.system.proxy.https port 0").toStdString().c_str(), "r");
            popen(QString("gsettings set org.gnome.system.proxy.socks host ''").toStdString().c_str(), "r");
            popen(QString("gsettings set org.gnome.system.proxy.socks port 0").toStdString().c_str(), "r");
        }
#endif
}
