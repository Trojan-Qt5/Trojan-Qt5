#ifndef SERVICETHREAD_H
#define SERVICETHREAD_H

#include <QThread>
#include "core/config.h"
#include "core/service.h"

class ServiceThread : public QThread
{
  Q_OBJECT

private:
  Service *service;
  Config *_config;
  void cleanUp();

public:
  ServiceThread(QObject *parent = nullptr);
  Config& config();
  ~ServiceThread();

protected:
  void run();

public slots:
  void stop();

signals:
  void startFailed();
};

#endif // SERVICETHREAD_H
