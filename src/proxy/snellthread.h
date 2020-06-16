#ifndef SNELLTHREAD_H
#define SNELLTHREAD_H

#include <QThread>

class SnellThread: public QThread
{
public:
    SnellThread(QString filePath);
    ~SnellThread();

private:
    QString filePath;

protected:
  void run();

public slots:
  void stop();
};

#endif // SNELLTHREAD_H
