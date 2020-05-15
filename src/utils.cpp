#include "utils.h"
#include "confighelper.h"
#include <QCoreApplication>
#include <QDir>

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

/*
QString Utils::getLocalAddr()
{
#ifdef Q_OS_WIN
    QString configFile = QCoreApplication::applicationDirPath() + "/config.ini";
#else
    QDir configDir = QDir::homePath() + "/.config/trojan-qt5";
    QString configFile = configDir.absolutePath() + "/config.ini";
#endif

    ConfigHelper *conf = new ConfigHelper(configFile);
    return
}*/
