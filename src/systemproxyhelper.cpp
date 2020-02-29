#include "confighelper.h"
#include "systemproxyhelper.h"
#include <array>
#include <stdio.h>
#include <sstream>
#include <regex>
#include <QCoreApplication>
#if defined (Q_OS_WIN)
#include <Windows.h>
#include "sysproxy/windows.h"
#endif

SystemProxyHelper::SystemProxyHelper()
{
}

/**
 * set system proxy to none after deconstruct
 *
 * @brief SystemProxyHelper::~SystemProxyHelper
 */
SystemProxyHelper::~SystemProxyHelper()
{
    setSystemProxy(TQProfile(), false);
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
void SystemProxyHelper::setSystemProxy(TQProfile profile, int method)
{
#ifdef Q_OS_WIN
    QString configFile = QCoreApplication::applicationDirPath() + "/config.ini";
#else
    QDir configDir = QDir::homePath() + "/.config/trojan-qt5";
    QString configFile = configDir.absolutePath() + "/config.ini";
#endif
    ConfigHelper *conf = new ConfigHelper(configFile);

#if defined(Q_OS_WIN)
    if (method == 1 && profile.dualMode) {
        QString server = conf->getHttpAddress() + ":" + QString::number(conf->getHttpPort());
        LPTSTR serverString = (LPTSTR) server.utf16();
        int status = setProxy(1, serverString);
    } else if (method == 2) {
        QString pac = QString("http://%1:%2/proxy.pac").arg(conf->getPACAddress()).arg(QString::number(conf->getPACPort()));
        LPTSTR pacString = (LPTSTR) pac.utf16();
        int status = setProxy(2, pacString);
    } else if (method == 0) {
        int status = setProxy(0, NULL);
    }
#elif defined (Q_OS_MAC)
    std::istringstream s(runShell("networksetup -listnetworkserviceorder"));
    std::vector<std::string> hardwarePorts;
    std::string portName;
    std::regex all("\\((\\d)*\\)\\s(.*)", std::regex_constants::ECMAScript | std::regex_constants::icase);
    std::regex prefix("\\((\\d)*\\)\\s", std::regex_constants::ECMAScript | std::regex_constants::icase);
    while(std::getline(s, portName))
    {
        if(std::regex_match(portName, all))
            if (std::regex_replace(portName, prefix, "") == "AirPort"
             || std::regex_replace(portName, prefix, "") == "Wi-Fi"
             || std::regex_replace(portName, prefix, "") == "Ethernet"
             || std::regex_replace(portName, prefix, "") == "USB 10/100/100 LAN")
                hardwarePorts.push_back(std::regex_replace(portName, prefix, ""));
    }

    for (auto &i : hardwarePorts) {
        if (method == 1 && profile.dualMode) {
            runShell(QString("networksetup -setwebproxy \"%1\" %2 %3").arg(QString::fromStdString(i)).arg(conf->getHttpAddress()).arg(QString::number(conf->getHttpPort())));
            runShell(QString("networksetup -setsecurewebproxy \"%1\" %2 %3").arg(QString::fromStdString(i)).arg(conf->getHttpAddress()).arg(QString::number(conf->getHttpPort())));
            runShell(QString("networksetup -setsocksfirewallproxy \"%1\" %2 %3").arg(QString::fromStdString(i)).arg(conf->getSocks5Address()).arg(QString::number(conf->getSocks5Port())));
        } else if (method == 1) {
            runShell(QString("networksetup -setsocksfirewallproxy \"%1\" %2 %3").arg(QString::fromStdString(i)).arg(conf->getSocks5Address()).arg(QString::number(conf->getSocks5Port())));
        } else if (method == 2) {
            runShell(QString("networksetup -setautoproxyurl \"%1\" http://%2:%3/proxy.pac").arg(QString::fromStdString(i)).arg(conf->getPACAddress()).arg(QString::number(conf->getPACPort())));
        } else if (method == 0) {
            runShell(QString("networksetup -setautoproxystate \"%1\" off").arg(QString::fromStdString(i)));
            runShell(QString("networksetup -setwebproxystate \"%1\" off").arg(QString::fromStdString(i)));
            runShell(QString("networksetup -setsecurewebproxystate \"%1\" off").arg(QString::fromStdString(i)));
            runShell(QString("networksetup -setsocksfirewallproxystate \"%1\" off").arg(QString::fromStdString(i)));
          }
        }

#elif defined (Q_OS_LINUX)
    if (system("gsettings --version > /dev/null") == 0) {
        if (method == 1 && profile.dualMode) {
            runShell(QString("gsettings set org.gnome.system.proxy mode manual"));
            runShell(QString("gsettings set org.gnome.system.proxy.http host %1").arg(conf->getHttpAddress()));
            runShell(QString("gsettings set org.gnome.system.proxy.http port %1").arg(QString::number(conf->getHttpPort())));
            runShell(QString("gsettings set org.gnome.system.proxy.https host %1").arg(conf->getHttpAddress()));
            runShell(QString("gsettings set org.gnome.system.proxy.https port %1").arg(QString::number(conf->getHttpPort())));
            runShell(QString("gsettings set org.gnome.system.proxy.socks host %1").arg(conf->getSocks5Address()));
            runShell(QString("gsettings set org.gnome.system.proxy.socks port %1").arg(QString::number(conf->getSocks5Port())));
        } else if (method == 1) {
            runShell(QString("gsettings set org.gnome.system.proxy mode manual"));
            runShell(QString("gsettings set org.gnome.system.proxy.socks host %1").arg(conf->getSocks5Address()));
            runShell(QString("gsettings set org.gnome.system.proxy.socks port %1").arg(QString::number(conf->getSocks5Port())));
        } else if (method == 2) {
            runShell(QString("gsettings set org.gnome.system.proxy mode auto"));
            runShell(QString("gsettings set org.gnome.system.proxy autoconfig-url http://%1:%2/proxy.pac").arg(conf->getPACAddress()).arg(QString::number(conf->getPACPort())));
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
        if (method == 1 && profile.dualMode) {
            runShell("kwriteconfig5 --file kioslaverc --group 'Proxy Settings' --key ProxyType 1");
            runShell(QString("kwriteconfig5 --file kioslaverc --group 'Proxy Settings' --key httpProxy \"%1:%2\"").arg(profile.localAddress).arg(conf->getHttpPort()));
            runShell(QString("kwriteconfig5 --file kioslaverc --group 'Proxy Settings' --key httpsProxy \"%1:%2\"").arg(profile.localAddress).arg(conf->getHttpPort()));
            runShell(QString("kwriteconfig5 --file kioslaverc --group 'Proxy Settings' --key socksProxy \"%1:%2\"").arg(conf->getSocks5Address()).arg(conf->getSocks5Port()));
        } else if (method == 1) {
            runShell("kwriteconfig5 --file kioslaverc --group 'Proxy Settings' --key ProxyType 1");
            runShell(QString("kwriteconfig5 --file kioslaverc --group 'Proxy Settings' --key socksProxy \"%1:%2\"").arg(conf->getSocks5Address()).arg(conf->getSocks5Port()));
        } else if (method == 2) {
            runShell("kwriteconfig5 --file kioslaverc --group 'Proxy Settings' --key ProxyType 2");
            runShell(QString("kwriteconfig5 --file kioslaverc --group 'Proxy Settings' --key 'Proxy Config Script' \"http://%1:%2/proxy.pac\"").arg(conf->getPACAddress()).arg(QString::number(conf->getPACPort())));
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
