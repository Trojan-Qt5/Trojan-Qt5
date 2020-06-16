#ifndef V2RAYTHREAD_H
#define V2RAYTHREAD_H

#include <QThread>

class V2rayThread: public QThread
{
public:
    V2rayThread(QString filePath);
    ~V2rayThread();

private:
    QString filePath;

protected:
  void run();

public slots:
  void stop();
};

#endif // V2RAYTHREAD_H
