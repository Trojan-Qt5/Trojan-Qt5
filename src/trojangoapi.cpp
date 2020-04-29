#include "trojangoapi.h"
#include "logger.h"
#include "confighelper.h"

using namespace api;
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

TrojanGoAPI::TrojanGoAPI()
{
    thread = new QThread();
    this->moveToThread(thread);
    connect(thread, SIGNAL(started()), this, SLOT(run()));
    running = true;
    thread->start();
}


TrojanGoAPI::~TrojanGoAPI()
{
    stop();
    thread->wait();
    thread = nullptr;
    delete thread;
}

void TrojanGoAPI::run()
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
        TrojanService service;
        Stub = service.NewStub(Channel);
        StatsReply reply;
        StatsRequest request;
        request.set_password("");
        ClientContext context;
        Status status = Stub->QueryStats(&context, request, &reply);

        if (!status.ok()) {
            Logger::error(QString("Trojan API Request failed: %1 (%2)").arg(status.error_code()).arg(QString::fromStdString(status.error_message())));
        }

        quint64 up = reply.upload_speed();
        quint64 down = reply.download_speed();

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
