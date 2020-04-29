#ifndef TROJANGOAPI_H
#define TROJANGOAPI_H

#include <QObject>
#include <QThread>
#include <grpc++/grpc++.h>

#include "trojangoapi.pb.h"
#include "trojangoapi.grpc.pb.h"

class TrojanGoAPI: public QObject
{
    Q_OBJECT

public:
    TrojanGoAPI();
    ~TrojanGoAPI();

    void stop();

signals:
     void OnDataReady(const quint64 up, const quint64 down);

public slots:
     void run();

private:
    bool running;
    QThread *thread;
    std::shared_ptr<::grpc::Channel> Channel;
    std::unique_ptr<::api::TrojanService::Stub> Stub;

};

#endif // TROJANGOAPI_H
