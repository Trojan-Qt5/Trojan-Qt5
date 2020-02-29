#ifndef PACSERVER_H
#define PACSERVER_H

#include <QObject>
#include <QDir>
#include <QHttpServer>

class PACServer: public QObject
{
    Q_OBJECT
public:
    PACServer();
    ~PACServer();

    QJsonDocument loadRules();
    void modify();

private:
    QDir configDir;
    QString configFile;
    QString gfwList;
    QString userRule;
    QString pac;
};

#endif // PACSERVER_H
