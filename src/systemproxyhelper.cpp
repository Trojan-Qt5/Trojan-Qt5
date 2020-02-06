#include "systemproxyhelper.h"

SystemProxyHelper::SystemProxyHelper()
{

}

void SystemProxyHelper::setSystemProxy(TQProfile profile, bool enable)
{
#if defined(Q_OS_WIN)
#elif defined (Q_OS_MAC)
    if (enable == true) {
        system(QString("networksetup -setsocksfirewallproxy \"Wi-Fi\" %1 %2").arg(profile.localAddress).arg(QString::number(profile.localPort)).toStdString().c_str());
    } else {
        system("networksetup -setsocksfirewallproxystate \"Wi-Fi\" off");
    }
#elif defined (Q_OS_LINUX)
    if (system("gsettings --version > /dev/null") == 0) {
        system(QString("gsettings set org.gnome.system.proxy.socks host %1").arg(profile.serverAddress).toStdString().c_str());
        system(QString("gsettings set org.gnome.system.proxy.socks port %1").arg(QString::number(profile.serverPort)).toStdString().c_str());
    }
#endif
}
