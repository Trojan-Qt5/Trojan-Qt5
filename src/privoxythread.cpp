#include "privoxythread.h"
#include <string>
#include <QCoreApplication>
#include <QDir>
#include <QDebug>
#if !defined (Q_OS_WIN)
#include "privoxy/jcc.h"
#endif

PrivoxyThread::PrivoxyThread(QObject *parent) :
  QThread(parent)
{}

PrivoxyThread::~PrivoxyThread()
{
    stop();
}

void PrivoxyThread::stop() {
#if defined (Q_OS_WIN)
    TerminateProcess(piProcessInfo.hProcess, 0);
#else
    close_privoxy_listening_socket();
#endif
}

void PrivoxyThread::run() {
#ifdef Q_OS_WIN
    QString dir = QCoreApplication::applicationDirPath() + "/privoxy/privoxy.exe";
    QString file = "privoxy.exe \"" + QCoreApplication::applicationDirPath() + "/privoxy/privoxy.conf\"";

    LPTSTR application = (LPTSTR) dir.utf16();
    LPTSTR arg = (LPTSTR) file.utf16();

    memset(&siStartupInfo, 0, sizeof(siStartupInfo));
    memset(&piProcessInfo, 0, sizeof(piProcessInfo));

    siStartupInfo.cb = sizeof(siStartupInfo);
    siStartupInfo.dwFlags = STARTF_USESHOWWINDOW | STARTF_FORCEOFFFEEDBACK | STARTF_USESTDHANDLES;
    siStartupInfo.wShowWindow = SW_HIDE;

    if(CreateProcess(application, arg, 0, 0, FALSE, 0, 0, 0, &siStartupInfo, &piProcessInfo) == FALSE)
    {
        CloseHandle(piProcessInfo.hThread);
        CloseHandle(piProcessInfo.hProcess);
    }
#else
    QDir configDir = QDir::homePath() + "/.config/trojan-qt5";
    QString file = configDir.absolutePath() + "/privoxy.conf";

    start_privoxy(file.toLocal8Bit().data());
#endif
}
