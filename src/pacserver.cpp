#include "pacserver.h"

#include <QCoreApplication>

PACServer::PACServer()
{
#ifdef Q_OS_WIN
    configDir = QDir::toNativeSeparators(QCoreApplication::applicationDirPath()) + "\\pac";
#else
    configDir = QDir::homePath() + "/.config/trojan-qt5/pac";
#endif

    if (!configDir.exists()) {
        configDir.mkpath(configDir.absolutePath());
    }

    pac = configDir.path() + "/proxy.pac";
}

PACServer::~PACServer()
{
}

/**
 * Modify the proxy.pac file
 *
 * @brief PACServer::modify
 * @param profile the Sever TQProfile
 * @ref https://stackoverflow.com/questions/17919778/qt-finding-and-replacing-text-in-a-file
 */
void PACServer::modify(TQProfile profile)
{
    if (QFile::exists(pac)) {
        QFile::remove(pac);
    }
    QFile::copy(":/pac/proxy.pac", pac);
    QFile::setPermissions(pac, QFile::WriteOwner | QFile::ReadOwner | QFile::ReadGroup | QFile::ReadOther);
    QByteArray fileData;
    QFile file(pac);
    file.open(QIODevice::ReadWrite); // open for read and write
    fileData = file.readAll(); // read all the data into the byte array
    QString text(fileData); // add to text string for easy string replace
    text.replace(QString("SOCKS5 127.0.0.1:1080"), QString("SOCKS5 %1:%2").arg(profile.localAddress).arg(profile.localPort));
    text.replace(QString("SOCKS 127.0.0.1:1080"), QString("SOCKS %1:%2").arg(profile.localAddress).arg(profile.localPort));
    text.replace(QString("PROXY 127.0.0.1:1081"), QString("PROXY %1:%2").arg(profile.localAddress).arg(profile.localHttpPort));
    file.seek(0); // go to the beginning of the file
    file.write(text.toUtf8()); // write the new text back to the file
    file.close(); // close the file handle.
}
