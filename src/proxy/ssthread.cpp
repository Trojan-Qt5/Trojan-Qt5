#include "ssthread.h"
#include "trojan-qt5-core.h"

SSThread::SSThread(QString clientAddr,
    QString serverAddr,
    QString method,
    QString password,
    QString plugin,
    QString pluginParam,
    bool enableAPI,
    QString apiAddr)
    : clientAddr(clientAddr)
    , serverAddr(serverAddr)
    , method(method)
    , password(password)
    , plugin(plugin)
    , pluginParam(pluginParam)
    , enableAPI(enableAPI)
    , apiAddr(apiAddr)
{
}

SSThread::~SSThread()
{
    stop();
}

void SSThread::run()
{
    startShadowsocksGo(clientAddr.toUtf8().data(),
                       serverAddr.toUtf8().data(),
                       method.toUtf8().data(),
                       password.toUtf8().data(),
                       plugin.toUtf8().data(),
                       pluginParam.toUtf8().data(),
                       enableAPI,
                       apiAddr.toUtf8().data());
}

void SSThread::stop()
{
    stopShadowsocksGo();
}
