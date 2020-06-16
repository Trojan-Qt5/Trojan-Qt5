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

    void start();
    void stop();
    void setPassword(QString pass);

signals:
     void OnDataReady(const quint64 up, const quint64 down);

public slots:
     void run();

private:
    bool running;
    QThread *thread;
    QString password;
    std::shared_ptr<::grpc::Channel> Channel;
    std::unique_ptr<::trojan::api::TrojanClientService::Stub> Stub;
    ::trojan::api::User *User;

};

#endif // TROJANGOAPI_H
