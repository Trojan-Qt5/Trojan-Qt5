#ifndef TROJANTHREAD_H
#define TROJANTHREAD_H

#include <QThread>

class TrojanThread: public QThread
{
public:
    TrojanThread(QString filePath);
    ~TrojanThread();

private:
    QString filePath;

protected:
  void run();

public slots:
  void stop();
};

#endif // TROJANTHREAD_H
