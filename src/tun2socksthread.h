#ifndef TUN2SOCKSTHREAD_H
#define TUN2SOCKSTHREAD_H

#include <QThread>

class Tun2socksThread : public QThread
{
public:
    Tun2socksThread();
    ~Tun2socksThread();

protected:
  void run();

public slots:
  void stop();
};

#endif // TUN2SOCKSTHREAD_H
