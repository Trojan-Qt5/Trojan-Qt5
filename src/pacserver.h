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

    void modify(QString filename);

    QByteArray request(QString url);

public slots:
    void typeModify(QString type);
    void copyPACUrl();
    void editLocalPACFile();
    void editUserRule();

private:
    QDir configDir;
    QString configFile;
    QString gfwList;
    QString userRule;
    QString pac;
};

#endif // PACSERVER_H
