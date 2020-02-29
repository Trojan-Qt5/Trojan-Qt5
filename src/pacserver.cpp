#include "confighelper.h"
#include "pacserver.h"

#include <QCoreApplication>
#include <QJsonArray>
#include <QJsonDocument>

PACServer::PACServer()
{
#ifdef Q_OS_WIN
    configDir = QDir::toNativeSeparators(QCoreApplication::applicationDirPath()) + "\\pac";
    configFile = QCoreApplication::applicationDirPath() + "/config.ini";
#else
    configDir = QDir::homePath() + "/.config/trojan-qt5/pac";
    configFile = QDir::homePath() + "/.config/trojan-qt5/config.ini";
#endif

    if (!configDir.exists()) {
        configDir.mkpath(configDir.absolutePath());
    }

    gfwList = configDir.path() + "/gfwlist.txt";
    userRule = configDir.path() + "/user-rule.txt";
    pac = configDir.path() + "/proxy.pac";

    /** Copy user-rule text to pac folder. */
    if (!QFile::exists(userRule))
        QFile::copy(":/pac/user-rule.txt", userRule);
        QFile::setPermissions(userRule, QFile::WriteOwner | QFile::ReadOwner | QFile::ReadGroup | QFile::ReadOther);

    QFile::copy(":/pac/gfwlist.txt", gfwList);
}

PACServer::~PACServer()
{
}

QJsonDocument PACServer::loadRules()
{
    QFile file(gfwList);
    file.open(QIODevice::ReadOnly);
    QStringList list = QString::fromUtf8(QByteArray::fromBase64(file.readAll())).split("\n");
    file.close();
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

/**
 * Modify the proxy.pac file
 *
 * @brief PACServer::modify
 * @param profile the Sever TQProfile
 * @ref https://stackoverflow.com/questions/17919778/qt-finding-and-replacing-text-in-a-file
 */
void PACServer::modify()
{
    ConfigHelper *conf = new ConfigHelper(configFile);
    if (QFile::exists(pac)) {
        QFile::remove(pac);
    }
    QFile::copy(":/pac/abp.js", pac);
    QFile::setPermissions(pac, QFile::WriteOwner | QFile::ReadOwner | QFile::ReadGroup | QFile::ReadOther);
    QByteArray fileData;
    QFile file(pac);
    file.open(QIODevice::ReadWrite); // open for read and write
    fileData = file.readAll(); // read all the data into the byte array
    QString text(fileData); // add to text string for easy string replace
    text.replace(QString("__SOCKS5ADDR__:__SOCKS5PORT__"), QString("%1:%2").arg(conf->getSocks5Address()).arg(QString::number(conf->getSocks5Port())));
    text.replace(QString("__PROXYADDR__:__PROXYPORT__"), QString("%1:%2").arg(conf->getHttpAddress()).arg(QString::number(conf->getHttpPort())));
    text.replace(QString("__RULES__"), loadRules().toJson());
    file.seek(0); // go to the beginning of the file
    file.write(text.toUtf8()); // write the new text back to the file
    file.close(); // close the file handle.
}
