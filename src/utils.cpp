#include "utils.h"

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
