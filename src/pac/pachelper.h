#ifndef PACHELPER_H
#define PACHELPER_H

#include <QObject>
#include <QDir>

class PACHelper: public QObject
{
    Q_OBJECT
public:
    PACHelper();
    ~PACHelper();

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

#endif // PACHELPER_H
