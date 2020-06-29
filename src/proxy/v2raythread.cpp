#include "v2raythread.h"
#include "trojan-qt5-core.h"

#include <QDebug>

V2rayThread::V2rayThread(QString filePath) : filePath(filePath)
{}

V2rayThread::~V2rayThread()
{
    stop();
}

void V2rayThread::run()
{
    startV2rayGo(filePath.toUtf8().data());
}

void V2rayThread::stop()
{
    stopV2rayGo();
}
