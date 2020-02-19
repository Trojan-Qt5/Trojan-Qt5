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

    void modify(TQProfile profile);

private:
    QDir configDir;
    QString pac;
};

#endif // PACSERVER_H
