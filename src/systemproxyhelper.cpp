#include "systemproxyhelper.h"
#if defined (Q_OS_WIN)
#include <Windows.h>
#include "sysproxy/windows.h"
#endif

SystemProxyHelper::SystemProxyHelper()
{

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
        system(QString("networksetup -setwebproxy \"Wi-Fi\" %1 %2").arg(profile.localAddress).arg(QString::number(profile.localHttpPort)).toStdString().c_str());
        system(QString("networksetup -setsecurewebproxy \"Wi-Fi\" %1 %2").arg(profile.localAddress).arg(QString::number(profile.localHttpPort)).toStdString().c_str());
        system(QString("networksetup -setsocksfirewallproxy \"Wi-Fi\" %1 %2").arg(profile.localAddress).arg(QString::number(profile.localPort)).toStdString().c_str());
    } else if (enable) {
        system(QString("networksetup -setsocksfirewallproxy \"Wi-Fi\" %1 %2").arg(profile.localAddress).arg(QString::number(profile.localPort)).toStdString().c_str());
    } else {
        system("networksetup -setwebproxystate \"Wi-Fi\" off");
        system("networksetup -setsecurewebproxystate \"Wi-Fi\" off");
        system("networksetup -setsocksfirewallproxystate \"Wi-Fi\" off");
    }
#elif defined (Q_OS_LINUX)
    if (system("gsettings --version > /dev/null") == 0) {
        system(QString("gsettings set org.gnome.system.proxy.socks host %1").arg(profile.serverAddress).toStdString().c_str());
        system(QString("gsettings set org.gnome.system.proxy.socks port %1").arg(QString::number(profile.serverPort)).toStdString().c_str());
    }
#endif
}
