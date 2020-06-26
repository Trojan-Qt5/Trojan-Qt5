#include "pachelper.h"

#include <QApplication>
#include <QCoreApplication>
#include <QDesktopServices>
#include <QJsonArray>
#include <QJsonDocument>
#include <QClipboard>
#include <QNetworkAccessManager>
#include <QNetworkProxy>
#include <QNetworkReply>

#include "confighelper.h"
#include "userrulesdialog.h"
#include "utils.h"

PACHelper::PACHelper()
{
#ifdef Q_OS_WIN
    configDir = QDir::toNativeSeparators(qApp->applicationDirPath()) + "\\pac";
    configFile = QCoreApplication::applicationDirPath() + "/config.ini";
#else
    configDir.setPath(QDir::homePath() + "/.config/trojan-qt5/pac");
    configFile = QDir::homePath() + "/.config/trojan-qt5/config.ini";
#endif

    if (!configDir.exists()) {
        configDir.mkpath(configDir.absolutePath());
    }

    gfwList = configDir.path() + "/gfwlist.txt";
    userRule = configDir.path() + "/user-rule.txt";
    pac = configDir.path() + "/proxy.pac";

    //Copy user-rule text to pac folder.
    if (!QFile::exists(userRule)) {
        QFile::copy(":/pac/user-rule.txt", userRule);
     }

    QFile::copy(":/pac/gfwlist.txt", gfwList);
    QFile::copy(":/pac/trojan_gfw.pac", configDir.path() + "/trojan_gfw.pac");
    QFile::copy(":/pac/trojan_lanip.pac", configDir.path() + "/trojan_lanip.pac");
    QFile::copy(":/pac/trojan_white.pac", configDir.path() + "/trojan_white.pac");
    QFile::copy(":/pac/trojan_white_advanced.pac", configDir.path() + "/trojan_white_advanced.pac");
    QFile::copy(":/pac/trojan_white_r.pac", configDir.path() + "/trojan_white_r.pac");
    QFile::copy(":/pac/trojan_cnip.pac", configDir.path() +"/trojan_cnip.pac");
    Utils::setPermisison(userRule);

    //Initalize when first startup
    if (!QFile::exists(pac))
        typeModify("GFWLIST");
}

PACHelper::~PACHelper()
{}

QByteArray PACHelper::request(QString url)
{
    ConfigHelper *conf = new ConfigHelper(configFile);

    QNetworkAccessManager* manager = new QNetworkAccessManager();
    QNetworkRequest request(url);
    QNetworkProxy proxy;
    proxy.setType(QNetworkProxy::Socks5Proxy);
    proxy.setHostName("127.0.0.1");
    proxy.setPort(conf->getInboundSettings().socks5LocalPort);
    manager->setProxy(proxy);
    QNetworkReply* reply = manager->sendCustomRequest(request, "GET", "");
    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
    return reply->readAll();
}

void PACHelper::copyPACUrl()
{
    ConfigHelper *conf = new ConfigHelper(configFile);
    QClipboard *board = QApplication::clipboard();
    board->setText(QString("http://127.0.0.1:%1/proxy.pac").arg(conf->getInboundSettings().pacLocalPort));
}

void PACHelper::editLocalPACFile()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(pac));
}

void PACHelper::editUserRule()
{
    UserRulesDialog *userRuleDlg = new UserRulesDialog();
    connect(userRuleDlg, &UserRulesDialog::finished,
            userRuleDlg, &UserRulesDialog::deleteLater);
    userRuleDlg->exec();
}

QJsonDocument PACHelper::loadRules()
{
    ConfigHelper *conf = new ConfigHelper(configFile);
    QStringList list;

    if (conf->getSubscribeSettings().gfwListUrl == 0) {
        list = QString::fromUtf8(QByteArray::fromBase64(request("https://raw.githubusercontent.com/gfwlist/gfwlist/master/gfwlist.txt"))).split("\n");
    }
    else if (conf->getSubscribeSettings().gfwListUrl == 1) {
        list = QString::fromUtf8(QByteArray::fromBase64(request("https://raw.githubusercontent.com/Loukky/gfwlist-by-loukky/master/gfwlist.txt"))).split("\n");
    }
    else if (conf->getSubscribeSettings().gfwListUrl == 2) {
        QFile file(gfwList);
        file.open(QIODevice::ReadOnly);
        list = QString::fromUtf8(QByteArray::fromBase64(file.readAll())).split("\n");
        file.close();
    }
    QStringList filedata;
    for (int i=0; i<list.length(); i++) {
        if (!list[i].startsWith("!") && !list[i].startsWith("["))
            if (list[i].length() != 0)
                filedata.append(list[i]);
    }
    QFile userrule(userRule);
    userrule.open(QIODevice::ReadOnly);
    QStringList userlist = QString::fromUtf8(userrule.readAll()).split("\n");
    userrule.close();
    for (int i=0; i<userlist.length(); i++) {
        if (!userlist[i].startsWith("!") && !userlist[i].startsWith("["))
            if (userlist[i].length() != 0)
                filedata.append(userlist[i]);
    }

    QJsonDocument data = QJsonDocument(QJsonArray::fromStringList(filedata));

    return data;
}

void PACHelper::typeModify(QString type)
{
    if (type == "LAN") {
        modify(configDir.path() + "/trojan_lanip.pac");
    } else if (type == "WHITE") {
        modify(configDir.path() + "/trojan_white.pac");
    } else if (type == "WHITE_ADVANCED") {
        modify(configDir.path() + "/trojan_white_advanced.pac");
    } else if (type == "WHITE_R") {
        modify(configDir.path() + "/trojan_white_r.pac");
    } else if (type == "CNIP") {
        modify(configDir.path() + "/trojan_cnip.pac");
    } else if (type == "GFWLIST") {
        modify(configDir.path() + "/trojan_gfw.pac");
    }
    //set System Proxy Again to force system reload pac file
    ConfigHelper *conf = new ConfigHelper(configFile);
    conf->readGeneralSettings();
    if (conf->isTrojanOn())
        if (conf->getSystemProxySettings() == "pac") {
            SystemProxyHelper *sph = new SystemProxyHelper();
            sph->setSystemProxy(0);
            sph->setSystemProxy(2);
        }
}

/**
 * Modify the proxy.pac file
 *
 * @brief PACHelper::modify
 * @param profile the Sever TQProfile
 * @ref https://stackoverflow.com/questions/17919778/qt-finding-and-replacing-text-in-a-file
 */
void PACHelper::modify(QString filename)
{
    ConfigHelper *conf = new ConfigHelper(configFile);

    if (QFile::exists(pac)) {
        QFile::remove(pac);
    }
    QFile::copy(filename, pac);
    Utils::setPermisison(pac);
    QByteArray fileData;
    QFile file(pac);
    file.open(QIODevice::ReadWrite); // open for read and write
    fileData = file.readAll(); // read all the data into the byte array
    QString text(fileData); // add to text string for easy string replace
    text.replace(QString("__SOCKS5__"), QString("SOCKS5 127.0.0.1:%1").arg(QString::number(conf->getInboundSettings().socks5LocalPort)));
    text.replace(QString("__SOCKS__"), QString("SOCKS 127.0.0.1:%1").arg(QString::number(conf->getInboundSettings().socks5LocalPort)));
    text.replace(QString("__PROXY__"), QString("PROXY 127.0.0.1:%1").arg(QString::number(conf->getInboundSettings().httpLocalPort)));
    if (filename == configDir.path() + "/trojan_gfw.pac")
        text.replace(QString("__RULES__"), loadRules().toJson());
    file.seek(0); // go to the beginning of the file
    file.write(text.toUtf8()); // write the new text back to the file
    file.close(); // close the file handle.
}
