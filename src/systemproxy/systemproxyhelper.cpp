#include "utils.h"
#include "systemproxyhelper.h"
#include <array>
#include <stdio.h>
#include <sstream>
#include <regex>
#include <QCoreApplication>
#if defined (Q_OS_WIN)
#include <Windows.h>
#include "win.h"
#elif defined (Q_OS_MAC)
#include "mac.h"
#endif

SystemProxyHelper::SystemProxyHelper()
{}

/**
 * set system proxy to none after deconstruct
 *
 * @brief SystemProxyHelper::~SystemProxyHelper
 */
SystemProxyHelper::~SystemProxyHelper()
{
    setSystemProxy(0);
}

/**
 * @brief SystemProxyHelper::runShell
 * @param cmd the command to execute
 * @return the execute result
 * @ref https://stackoverflow.com/questions/478898/how-do-i-execute-a-command-and-get-the-output-of-the-command-within-c-using-po
 */
std::string SystemProxyHelper::runShell(QString cmd)
{
#if !defined (Q_OS_WIN)
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.toStdString().c_str(), "r"), pclose);
    if (!pipe)
      throw std::runtime_error("popen() failed!");
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
      result += buffer.data();
    return result;
#else
    return "";
#endif
}

/**
 * Main entry of setting the system proxy
 *
 * @brief SystemProxyHelper::setSystemProxy
 * @param profile Server TQProfile
 * @param method 0: off 1: global 2: pac
 * @ref https://github.com/j1ml/proxydriver/blob/master/proxydriver.sh
 * @ref https://github.com/trojan-gfw/trojan-qt/blob/master/src/AppManager.cpp
 * @ref https://github.com/shadowsocks/ShadowsocksX-NG/blob/develop/proxy_conf_helper/main.m
 */
void SystemProxyHelper::setSystemProxy(int method)
{
    ConfigHelper *conf = Utils::getConfigHelper();

#if defined(Q_OS_WIN)
    if (method == 1 && conf->getInboundSettings().enableHttpMode) {
        QString server = "127.0.0.1:" + QString::number(conf->getInboundSettings().httpLocalPort);
        LPTSTR serverString = (LPTSTR) server.utf16();
        int status = setProxy(1, serverString);
    } else if (method == 2) {
        QString pac = QString("http://127.0.0.1:%2/proxy.pac").arg(QString::number(conf->getInboundSettings().pacLocalPort));
        LPTSTR pacString = (LPTSTR) pac.utf16();
        int status = setProxy(2, pacString);
    } else if (method == 0) {
        int status = setProxy(0, NULL);
    }
#elif defined (Q_OS_MAC)

    if (method == 1 && conf->getInboundSettings().enableHttpMode) {
        runShell(QString("/Library/Application\\ Support/Trojan-Qt5/proxy_conf_helper -m global -l 127.0.0.1 -p %1 -s 127.0.0.1 -r %2").arg(QString::number(conf->getInboundSettings().socks5LocalPort)).arg(QString::number(conf->getInboundSettings().httpLocalPort)));
    } else if (method == 1) {
        runShell(QString("/Library/Application\\ Support/Trojan-Qt5/proxy_conf_helper -m global -l 127.0.0.1 -p %1").arg(QString::number(conf->getInboundSettings().socks5LocalPort)));
    } else if (method == 2) {
        runShell(QString("/Library/Application\\ Support/Trojan-Qt5/proxy_conf_helper -m auto -u http://127.0.0.1:%1/proxy.pac").arg(QString::number(conf->getInboundSettings().pacLocalPort)));
    } else if (method == 0) {
        runShell(QString("/Library/Application\\ Support/Trojan-Qt5/proxy_conf_helper -m off"));
    }

#elif defined (Q_OS_LINUX)
    if (system("gsettings --version > /dev/null") == 0) {
        if (method == 1 && conf->getInboundSettings().enableHttpMode) {
            runShell(QString("gsettings set org.gnome.system.proxy mode manual"));
            runShell(QString("gsettings set org.gnome.system.proxy.http host 127.0.0.1"));
            runShell(QString("gsettings set org.gnome.system.proxy.http port %1").arg(QString::number(conf->getInboundSettings().httpLocalPort)));
            runShell(QString("gsettings set org.gnome.system.proxy.https host 127.0.0.1"));
            runShell(QString("gsettings set org.gnome.system.proxy.https port %1").arg(QString::number(conf->getInboundSettings().httpLocalPort)));
            runShell(QString("gsettings set org.gnome.system.proxy.socks host 127.0.0.1"));
            runShell(QString("gsettings set org.gnome.system.proxy.socks port %1").arg(QString::number(conf->getInboundSettings().socks5LocalPort)));
        } else if (method == 1) {
            runShell(QString("gsettings set org.gnome.system.proxy mode manual"));
            runShell(QString("gsettings set org.gnome.system.proxy.socks host 127.0.0.1"));
            runShell(QString("gsettings set org.gnome.system.proxy.socks port %1").arg(QString::number(conf->getInboundSettings().socks5LocalPort)));
        } else if (method == 2) {
            runShell(QString("gsettings set org.gnome.system.proxy mode auto"));
            runShell(QString("gsettings set org.gnome.system.proxy autoconfig-url http://127.0.0.1:%1/proxy.pac").arg(QString::number(conf->getInboundSettings().pacLocalPort)));
        } else if (method == 0) {
            runShell("gsettings set org.gnome.system.proxy.mode none");
            runShell("gsettings set org.gnome.system.proxy.autoconfig ''");
            runShell("gsettings set org.gnome.system.proxy.http host ''");
            runShell("gsettings set org.gnome.system.proxy.http port 0");
            runShell("gsettings set org.gnome.system.proxy.https host ''");
            runShell("gsettings set org.gnome.system.proxy.https port 0");
            runShell("gsettings set org.gnome.system.proxy.socks host ''");
            runShell("gsettings set org.gnome.system.proxy.socks port 0");
        }
    } else if (system("kwriteconfig5 --help > /dev/null") == 0) {
        if (method == 1 && conf->getInboundSettings().enableHttpMode) {
            runShell("kwriteconfig5 --file kioslaverc --group 'Proxy Settings' --key ProxyType 1");
            runShell(QString("kwriteconfig5 --file kioslaverc --group 'Proxy Settings' --key httpProxy \"127.0.0.1:%1\"").arg(conf->getInboundSettings().httpLocalPort));
            runShell(QString("kwriteconfig5 --file kioslaverc --group 'Proxy Settings' --key httpsProxy \"127.0.0.1:%1\"").arg(conf->getInboundSettings().httpLocalPort));
            runShell(QString("kwriteconfig5 --file kioslaverc --group 'Proxy Settings' --key socksProxy \"127.0.0.1:%1\"").arg(conf->getInboundSettings().socks5LocalPort));
        } else if (method == 1) {
            runShell("kwriteconfig5 --file kioslaverc --group 'Proxy Settings' --key ProxyType 1");
            runShell(QString("kwriteconfig5 --file kioslaverc --group 'Proxy Settings' --key socksProxy \"127.0.0.1:%1\"").arg(conf->getInboundSettings().socks5LocalPort));
        } else if (method == 2) {
            runShell("kwriteconfig5 --file kioslaverc --group 'Proxy Settings' --key ProxyType 2");
            runShell(QString("kwriteconfig5 --file kioslaverc --group 'Proxy Settings' --key 'Proxy Config Script' \"http://127.0.0.1:%1/proxy.pac\"").arg(QString::number(conf->getInboundSettings().pacLocalPort)));
        } else if (method == 0) {
            runShell("kwriteconfig5 --file kioslaverc --group 'Proxy Settings' --key ProxyType 0");
            runShell("kwriteconfig5 --file kioslaverc --group 'Proxy Settings' --key 'Proxy Config Script' \"\"");
            runShell("kwriteconfig5 --file kioslaverc --group 'Proxy Settings' --key httpProxy \"\"");
            runShell("kwriteconfig5 --file kioslaverc --group 'Proxy Settings' --key httpsProxy \"\"");
            runShell("kwriteconfig5 --file kioslaverc --group 'Proxy Settings' --key socksProxy \"\"");
        }

        /** We have to tell KIO after we have modified the kioslaverc. */
        runShell("dbus-send --type=signal /KIO/Scheduler org.kde.KIO.Scheduler.reparseSlaveConfiguration string:''");
    }
#endif
}
