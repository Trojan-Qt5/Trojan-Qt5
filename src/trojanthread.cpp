#include "trojanthread.h"
#include "3rd/trojan-qt5-core/trojan-qt5-core.h"

TrojanThread::TrojanThread(QString filePath) : filePath(filePath)
{}

TrojanThread::~TrojanThread()
{
    stop();
}

void TrojanThread::run()
{
    startTrojanGo(filePath.toUtf8().data());
}

void TrojanThread::stop()
{
    stopTrojanGo();
}
