#ifndef PACSERVER_H
#define PACSERVER_H

#include <QObject>
#include <QDir>
#include <QHttpServer>

#include "tqprofile.h"

class PACServer: public QObject
{
    Q_OBJECT
public:
    PACServer();
    ~PACServer();

    QJsonDocument loadRules();
    void modify(TQProfile profile);

private:
    QDir configDir;
    QString gfwList;
    QString userRule;
    QString pac;
};

#endif // PACSERVER_H
