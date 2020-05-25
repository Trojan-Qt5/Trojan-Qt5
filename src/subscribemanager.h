#ifndef SUBSCRIBEMANAGER_H
#define SUBSCRIBEMANAGER_H

#include <QObject>

#include "confighelper.h"

class MainWindow;

class SubscribeManager : public QObject
{
    Q_OBJECT

public:
    explicit SubscribeManager(MainWindow *w, ConfigHelper *ch);

    void setUseProxy(bool);

    QString checkUpdate(QString url);

    bool isFiltered(QString name);

    void updateAllSubscribesWithThread();

signals:
    void addUri(TQProfile);

public slots:

    void updateAllSubscribes();

private:
    QThread *thread;
    ConfigHelper *helper;
    bool useProxy;
    MainWindow *window;
};

#endif // SUBSCRIBEMANAGER_H
