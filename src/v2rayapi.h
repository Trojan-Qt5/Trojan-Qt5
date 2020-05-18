#ifndef V2RAYAPI_H
#define V2RAYAPI_H

#include <QObject>
#include <QThread>
#include <grpc++/grpc++.h>

#include "v2rayapi.pb.h"
#include "v2rayapi.grpc.pb.h"

class V2rayAPI: public QObject
{
    Q_OBJECT

public:
    V2rayAPI();
    ~V2rayAPI();

    void start();
    void stop();

    qint64 callAPI(QString);

signals:
     void OnDataReady(const quint64 up, const quint64 down);

public slots:
     void run();

private:
    bool running;
    QThread *thread;
    std::shared_ptr<::grpc::Channel> Channel;
    std::unique_ptr<::v2ray::core::app::stats::command::StatsService::Stub> Stub;
};

#endif // V2RAYAPI_H
