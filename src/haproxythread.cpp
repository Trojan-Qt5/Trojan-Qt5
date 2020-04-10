#include "haproxythread.h"

HaproxyThread::HaproxyThread()
{

}

/*
void HaproxyThread::run() {
#ifdef Q_OS_WIN
    QString dir = QCoreApplication::applicationDirPath() + "/haproxy/haproxy.exe";
    QString file = "haproxy.exe -f \"" + QCoreApplication::applicationDirPath() + "/haproxy/haproxy.conf\"";

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
#endif
}

void HaproxyThread::stop() {

}
*/
