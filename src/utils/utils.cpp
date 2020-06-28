#include "utils.h"
#include <QApplication>
#include <QDir>
#include <QStandardPaths>
#include <QStyle>

Utils::Utils()
{}

QString Utils::Base64UrlEncode(QString plainText)
{
    QString encodedText = plainText.toUtf8().toBase64();
    encodedText = encodedText.replace(QChar('+'), QChar('-')).replace(QChar('/'), QChar('_')).replace(QChar('='), "");
    return encodedText;
}

QString Utils::Base64UrlDecode(QString encodedText)
{
    QByteArray encodedArray = encodedText.replace(QChar('-'), QChar('+')).replace(QChar('_'), QChar('/')).toUtf8();
    QString plainText = QByteArray::fromBase64(encodedArray, QByteArray::Base64Option::OmitTrailingEquals);
    return plainText;
}

QStringList Utils::splitLines(const QString &string)
{
    return string.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);
}

QString Utils::toCamelCase(const QString& s)
{
    QStringList parts = s.split(' ', QString::SkipEmptyParts);
    for (int i = 0; i < parts.size(); ++i)
        parts[i].replace(0, 1, parts[i][0].toUpper());

    return parts.join(" ");
}

QString Utils::getLogDir()
{
#if defined (Q_OS_WIN)
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
#elif defined(Q_OS_MAC)
    return QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/Library/Logs/Trojan-Qt5";
#elif defined (Q_OS_LINUX)
    return QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation) + "/trojan-qt5";
#endif
}

void Utils::setPermisison(QString &file)
{
    QFile::setPermissions(file, QFile::ReadOwner | QFile::WriteOwner | QFile::ReadGroup | QFile::WriteGroup);
}

QSize Utils::smallIconSize(const QWidget *widget)
{
    // Get DPI scaled icon size (device-dependent), see QT source
    // under a 1080p screen is usually 16x16
    const int s = QApplication::style()->pixelMetric(QStyle::PM_SmallIconSize, nullptr, widget);
    return {s, s};
}

QSize Utils::mediumIconSize(const QWidget *widget)
{
    // under a 1080p screen is usually 24x24
    return ((smallIconSize(widget) + largeIconSize(widget)) / 2);
}

QSize Utils::largeIconSize(const QWidget *widget)
{
    // Get DPI scaled icon size (device-dependent), see QT source
    // under a 1080p screen is usually 32x32
    const int s = QApplication::style()->pixelMetric(QStyle::PM_LargeIconSize, nullptr, widget);
    return {s, s};
}

QString Utils::bytesConvertor(const quint64 &t)
{
    if (t >= (double)1024L * (double)1024L * (double)1024L * (double)1024L)
        return QString::number(t / (double)1024 / (double)1024 / (double)1024 / (double)1024, 'f', 2) + " TB";
    else if (t >= (double)1024L * (double)1024L * (double)1024L)
        return QString::number(t / (double)1024 / (double)1024 / (double)1024, 'f', 2) + " GB";
    else if (t >= (double)1024 * (double)1024)
        return QString::number(t / (double)1024 / (double)1024, 'f', 2) + " MB";
    else if (t >= (double)1024)
        return QString::number(t / (double)1024, 'f', 2) + " KB";
    else
        return QString::number(t, 'f', 2) + " B";
}

QString Utils::getConfigPath()
{
#ifdef Q_OS_WIN
    QString configPath = qApp->applicationDirPath();
#else
    QDir configDir = QDir::homePath() + "/.config/trojan-qt5";
    QString configPath = configDir.absolutePath();
#endif

    return configPath;
}

ConfigHelper* Utils::getConfigHelper()
{
    QString configFile = getConfigPath() + "/config.ini";
    ConfigHelper *helper = new ConfigHelper(configFile);
    return helper;
}

QJsonObject Utils::convertWsHeader(QList<WsHeader> headers)
{
    QJsonObject object;
    for (WsHeader header : headers)
        object[header.key] = header.value;
    return object;
}

QList<WsHeader> Utils::convertQJsonObject(const QJsonObject &object)
{
    QList<WsHeader> wsHeaders;
    for (const QString &key : object.keys()) {
        QJsonValue val = object.value(key);
        WsHeader header;
        header.key = key;
        header.value = val.toString();
        wsHeaders.append(header);
    }
    return wsHeaders;
}

/*
QString Utils::getLocalAddr()
{
#ifdef Q_OS_WIN
    QString configFile = qApp->applicationDirPath() + "/config.ini";
#else
    QDir configDir = QDir::homePath() + "/.config/trojan-qt5";
    QString configFile = configDir.absolutePath() + "/config.ini";
#endif

    ConfigHelper *conf = new ConfigHelper(configFile);
    return
}*/
