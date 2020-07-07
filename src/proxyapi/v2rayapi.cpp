#include "v2rayapi.h"
#include "logger.h"
#include "confighelper.h"
#include "utils.h"

using namespace v2ray::core::app::stats::command;
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

V2rayAPI::V2rayAPI()
{
    thread = new QThread();
    this->moveToThread(thread);
    connect(thread, SIGNAL(started()), this, SLOT(run()));
}

V2rayAPI::~V2rayAPI()
{
    stop();
    thread->wait();
    delete thread;
}

void V2rayAPI::start()
{
    running = true;
    thread->start();
}

void V2rayAPI::run()
{
    ConfigHelper *conf = Utils::getConfigHelper();

    QString address = QString("127.0.0.1:%1").arg(conf->getCoreSettings().apiPort);

    while (running) {

        Channel = grpc::CreateChannel(address.toStdString(), grpc::InsecureChannelCredentials());
        StatsService service;
        Stub = service.NewStub(Channel);

        if (conf->getCoreSettings().countOutboundTraffic) {
            quint64 proxyUp = callAPI("outbound>>>proxy>>>traffic>>>uplink");
            quint64 proxyDown = callAPI("outbound>>>proxy>>>traffic>>>downlink");
            quint64 directUp = callAPI("outbound>>>direct>>>traffic>>>uplink");
            quint64 directDown = callAPI("outbound>>>direct>>>traffic>>>downlink");
            emit OnDataReady(proxyUp, proxyDown, directUp, directDown);
        } else {
            quint64 up = callAPI("inbound>>>inbound>>>traffic>>>uplink");
            quint64 down = callAPI("inbound>>>inbound>>>traffic>>>downlink");
            emit OnDataReady(up, down, 0, 0);
        }

        QThread::msleep(1000); // sleep one second
    }

    thread->exit();
}

qint64 V2rayAPI::callAPI(QString name)
{
    GetStatsResponse reply;
    GetStatsRequest request;
    request.set_name(name.toStdString());
    request.set_reset(true);
    ClientContext context;
    Status status = Stub->GetStats(&context, request, &reply);

    if (!status.ok()) {
        Logger::error(QString("[API] V2ray API Request failed: %1 (%2)").arg(status.error_code()).arg(QString::fromStdString(status.error_message())));
    }

    qint64 data = reply.stat().value();

    return data;
}

void V2rayAPI::stop()
{
    running = false;
}
