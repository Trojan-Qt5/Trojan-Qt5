#ifndef SNELLGOAPI_H
#define SNELLGOAPI_H

#include <QObject>
#include <QThread>
#include <grpc++/grpc++.h>

#include "snellgoapi.pb.h"
#include "snellgoapi.grpc.pb.h"

class SnellGoAPI: public QObject
{
    Q_OBJECT
public:
    SnellGoAPI();
    ~SnellGoAPI();

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
    std::unique_ptr<::snell::api::SnellService::Stub> Stub;
};

#endif // SNELLGOAPI_H
