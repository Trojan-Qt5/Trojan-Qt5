#ifndef SUBSCRIBEMANAGER_H
#define SUBSCRIBEMANAGER_H

#include <QObject>

#include "confighelper.h"

class SubscribeManager : public QObject
{
    Q_OBJECT

public:
    explicit SubscribeManager(ConfigHelper *ch, QObject *parent = nullptr);

    QString checkUpdate(QString url, bool useProxy);

signals:
    void addUri(QString);

public slots:

    void updateAllSubscribes(bool useProxy);

private:
    ConfigHelper *helper;
};

#endif // SUBSCRIBEMANAGER_H
