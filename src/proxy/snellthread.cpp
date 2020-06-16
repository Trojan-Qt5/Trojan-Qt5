#include "snellthread.h"
#include "trojan-qt5-core.h"

SnellThread::SnellThread(QString filePath) : filePath(filePath)
{}

SnellThread::~SnellThread()
{
    stop();
}

void SnellThread::run()
{
    startSnellGo(filePath.toUtf8().data());
}

void SnellThread::stop()
{
    stopSnellGo();
}
