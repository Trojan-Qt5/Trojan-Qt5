#ifndef PRIVOXYTHREAD_H
#define PRIVOXYTHREAD_H

#include <QThread>
#if defined(Q_OS_WIN)
#include <Windows.h>
#endif

class PrivoxyThread : public QThread
{

public:
  PrivoxyThread(QObject *parent = nullptr);
  ~PrivoxyThread();

private:
#if defined (Q_OS_WIN)
  STARTUPINFO siStartupInfo;
  PROCESS_INFORMATION piProcessInfo;
#endif

protected:
  void run();

public slots:
  void stop();
};

#endif // SERVICETHREAD_H
