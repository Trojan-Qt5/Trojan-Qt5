#include "privoxythread.h"
#include <string>
#include <QCoreApplication>
#include <QDir>
#if defined (Q_OS_WIN)
#include "WinPrivoxy/libprivoxy.h"
#else
#include "privoxy/jcc.h"
#endif

PrivoxyThread::PrivoxyThread(QObject *parent)
{}

void PrivoxyThread::stop() {
}

void PrivoxyThread::run() {
#ifdef Q_OS_WIN
    QString file = QCoreApplication::applicationDirPath() + "/privoxy.conf";
    StartPrivoxy(file.toLocal8Bit().data());
#else
    QDir configDir = QDir::homePath() + "/.config/trojan-qt5";
    QString file = configDir.absolutePath() + "/privoxy.conf";
    start_privoxy(file.toLocal8Bit().data());
#endif
}
