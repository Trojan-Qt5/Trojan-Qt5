#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>

class Logger: public QObject
{
    Q_OBJECT
public:
    static void init(const QString &msg);
    static void debug(const QString &msg);
    static void info(const QString &msg);
    static void warning(const QString &msg);
    static void error(const QString &msg);
};

#endif // LOGGER_H
