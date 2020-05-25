#include "v2rayapi.h"
#include "logger.h"
#include "confighelper.h"
#if defined (Q_OS_WIN)
#include <QCoreApplication>
#endif

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
#ifdef Q_OS_WIN
    QString configFile = qApp->applicationDirPath() + "/config.ini";
#else
    QDir configDir = QDir::homePath() + "/.config/trojan-qt5";
    QString configFile = configDir.absolutePath() + "/config.ini";
#endif

    ConfigHelper *conf = new ConfigHelper(configFile);

    QString address = QString("127.0.0.1:%1").arg(conf->getTrojanSettings()["trojanAPIPort"].toInt());

    while (running) {

        Channel = grpc::CreateChannel(address.toStdString(), grpc::InsecureChannelCredentials());
        StatsService service;
        Stub = service.NewStub(Channel);

        quint64 up = callAPI("inbound>>>inbound>>>traffic>>>uplink");
        quint64 down = callAPI("inbound>>>inbound>>>traffic>>>downlink");

        if (up >= 0 && down >= 0)
            emit OnDataReady(up, down);

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
