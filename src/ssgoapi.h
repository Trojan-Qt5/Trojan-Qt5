#ifndef SSGOAPI_H
#define SSGOAPI_H

#include <QObject>
#include <QThread>
#include <grpc++/grpc++.h>

#include "ssgoapi.pb.h"
#include "ssgoapi.grpc.pb.h"

class SSGoAPI: public QObject
{
    Q_OBJECT
public:
    SSGoAPI();
    ~SSGoAPI();

    void start();
    void stop();

signals:
     void OnDataReady(const quint64 up, const quint64 down);

public slots:
     void run();

private:
    bool running;
    QThread *thread;
    QString password;
    std::shared_ptr<::grpc::Channel> Channel;
    std::unique_ptr<::shadowsocks::api::SSService::Stub> Stub;
};

#endif // SSGOAPI_H
