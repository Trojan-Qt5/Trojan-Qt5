#ifndef SSTHREAD_H
#define SSTHREAD_H

#include <QThread>

class SSThread: public QThread
{
public:
    SSThread(QString clientAddr,
             QString serverAddr,
             QString method,
             QString password,
             QString plugin,
             QString pluginParam,
             bool enableAPI,
             QString apiAddr);
    ~SSThread();

private:
    QString clientAddr;
    QString serverAddr;
    QString method;
    QString password;
    QString plugin;
    QString pluginParam;
    bool enableAPI;
    QString apiAddr;

protected:
  void run();

public slots:
  void stop();
};

#endif // SSTHREAD_H
