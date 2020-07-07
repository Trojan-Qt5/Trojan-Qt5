#include "ssgoapi.h"
#include "logger.h"
#include "confighelper.h"
#include "utils.h"

using namespace shadowsocks::api;
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

SSGoAPI::SSGoAPI()
{
    thread = new QThread();
    this->moveToThread(thread);
    connect(thread, SIGNAL(started()), this, SLOT(run()));
}

SSGoAPI::~SSGoAPI()
{
    stop();
    thread->wait();
    delete thread;
}

void SSGoAPI::start()
{
    running = true;
    thread->start();
}

void SSGoAPI::run()
{

    ConfigHelper *conf = Utils::getConfigHelper();

    QString address = QString("127.0.0.1:%1").arg(conf->getCoreSettings().apiPort);

    while (running) {

        Channel = grpc::CreateChannel(address.toStdString(), grpc::InsecureChannelCredentials());
        SSService service;
        Stub = service.NewStub(Channel);
        StatsReply reply;
        StatsRequest request;
        ClientContext context;
        Status status = Stub->QueryStats(&context, request, &reply);

        if (!status.ok()) {
            Logger::error(QString("[API] Shadowsocks API Request failed: %1 (%2)").arg(status.error_code()).arg(QString::fromStdString(status.error_message())));
        }

        quint64 up = reply.upload_speed();
        quint64 down = reply.download_speed();

        if (up >= 0 && down >= 0)
            emit OnDataReady(up, down);

        QThread::msleep(1000); // sleep one second
    }

    thread->exit();
}

void SSGoAPI::stop()
{
    running = false;
}
