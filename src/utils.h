#ifndef UTILS_H
#define UTILS_H

#include <QObject>

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

};

#endif // UTILS_H
