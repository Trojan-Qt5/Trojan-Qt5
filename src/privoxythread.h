#ifndef PRIVOXYTHREAD_H
#define PRIVOXYTHREAD_H

#include <QThread>

class PrivoxyThread : public QThread
{

public:
  PrivoxyThread(QObject *parent = 0);
  //~ServiceThread();

protected:
  void run();

public slots:
  void stop();
};

#endif // SERVICETHREAD_H
