#include "trojangoapi.h"
#include "logger.h"
#include "confighelper.h"
#include "utils.h"

using namespace trojan::api;
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

TrojanGoAPI::TrojanGoAPI()
{
    thread = new QThread();
    this->moveToThread(thread);
    connect(thread, SIGNAL(started()), this, SLOT(run()));
}

TrojanGoAPI::~TrojanGoAPI()
{
    stop();
    thread->wait();
    delete thread;
}

void TrojanGoAPI::setPassword(QString pass)
{
    password = pass;
}

void TrojanGoAPI::start()
{
    running = true;
    thread->start();
}

void TrojanGoAPI::run()
{
    ConfigHelper *conf = Utils::getConfigHelper();

    QString address = QString("127.0.0.1:%1").arg(conf->getCoreSettings().apiPort);

    while (running) {

        Channel = grpc::CreateChannel(address.toStdString(), grpc::InsecureChannelCredentials());
        TrojanClientService service;
        Stub = service.NewStub(Channel);
        GetTrafficResponse reply;
        GetTrafficRequest request;
        User = new ::trojan::api::User;
        User->set_password(password.toUtf8().data());
        request.set_allocated_user(User);
        ClientContext context;
        Status status = Stub->GetTraffic(&context, request, &reply);

        if (!status.ok()) {
            Logger::error(QString("[API] Trojan API Request failed: %1 (%2)").arg(status.error_code()).arg(QString::fromStdString(status.error_message())));
        }

        quint64 up = reply.speed_current().upload_speed();
        quint64 down = reply.speed_current().download_speed();

        if (up >= 0 && down >= 0)
            emit OnDataReady(up, down);

        QThread::msleep(1000); // sleep one second
    }

    thread->exit();
}

void TrojanGoAPI::stop()
{
    running = false;
}
