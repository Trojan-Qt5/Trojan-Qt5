#ifndef UTILS_H
#define UTILS_H

#include <QObject>
#include "confighelper.h"

class Utils: public QObject
{
    Q_OBJECT

public:
    Utils();

    static QString Base64UrlEncode(QString plainText);
    static QString Base64UrlDecode(QString encodedText);

    static QStringList splitLines(const QString &string);
    static QString toCamelCase(const QString& s);
    static QString getLogDir();
    static void setPermisison(QString &file);

    static QSize smallIconSize(const QWidget *widget = nullptr);
    static QSize mediumIconSize(const QWidget *widget = nullptr);
    static QSize largeIconSize(const QWidget *widget = nullptr);

    static QString bytesConvertor(const quint64 &t);

    static ConfigHelper* getConfigHelper();

};

#endif // UTILS_H
