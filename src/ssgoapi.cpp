#include "ssgoapi.h"
#include "logger.h"
#include "confighelper.h"
#if defined (Q_OS_WIN)
#include <QCoreApplication>
#endif

using namespace api;
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
    thread = nullptr;
    delete thread;
}

void SSGoAPI::start()
{
    running = true;
    thread->start();
}

void SSGoAPI::run()
{
#ifdef Q_OS_WIN
    QString configFile = QCoreApplication::applicationDirPath() + "/config.ini";
#else
    QDir configDir = QDir::homePath() + "/.config/trojan-qt5";
    QString configFile = configDir.absolutePath() + "/config.ini";
#endif

    ConfigHelper *conf = new ConfigHelper(configFile);

    QString address = QString("127.0.0.1:%1").arg(conf->getTrojanAPIPort());

    while (running) {

        Channel = grpc::CreateChannel(address.toStdString(), grpc::InsecureChannelCredentials());
        SSService service;
        Stub = service.NewStub(Channel);
        StatsReply reply;
        StatsRequest request;
        ClientContext context;
        Status status = Stub->QueryStats(&context, request, &reply);

        if (!status.ok()) {
            Logger::error(QString("Shadowsocks API Request failed: %1 (%2)").arg(status.error_code()).arg(QString::fromStdString(status.error_message())));
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
