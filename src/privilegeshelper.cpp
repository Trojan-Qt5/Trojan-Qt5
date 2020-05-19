#include "privilegeshelper.h"

#include <QMessageBox>

#if defined (Q_OS_WIN)
#include <Windows.h>
#include <Processthreadsapi.h>
#include <Securitybaseapi.h>
#elif defined (Q_OS_MAC) || defined (Q_OS_LINUX)
#include <unistd.h>
#endif

PrivilegesHelper::PrivilegesHelper()
{}

bool PrivilegesHelper::checkPrivileges()
{
#if defined (Q_OS_WIN)
    BOOL fIsElevated = FALSE;
    HANDLE hToken = NULL;
    TOKEN_ELEVATION elevation;
    DWORD dwSize;

    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
    {
        if (hToken)
        {
            CloseHandle(hToken);
            hToken = NULL;
        }
    }


    if (!GetTokenInformation(hToken, TokenElevation, &elevation, sizeof(elevation), &dwSize))
    {
        if (hToken)
        {
            CloseHandle(hToken);
            hToken = NULL;
        }
    }

    fIsElevated = elevation.TokenIsElevated;

    if (hToken)
    {
        CloseHandle(hToken);
        hToken = NULL;
    }

    return (bool)fIsElevated;

#elif defined (Q_OS_MAC) || defined(Q_OS_LINUX)
    if (geteuid() == 0)
        return true;
    else
        return false;
#endif

}

void PrivilegesHelper::showWarning()
{
    QString link;
#if defined (Q_OS_WIN)
    link = tr("https://www.isumsoft.com/windows-10/run-an-app-as-administrator-in-windows-10.html");
#elif defined (Q_OS_MAC)
    link = tr("https://osxdaily.com/2013/02/06/how-to-run-gui-apps-as-root-in-mac-os-x/");
#elif defined (Q_OS_LINUX)
    link = tr("https://askubuntu.com/questions/207466/how-to-run-applications-as-root");
#endif
    QMessageBox::warning(NULL, tr("Can't start tun2socks"), QString(tr("You need to run this application as admin/root\nSee the following link for a reference:\n%1")).arg(link), QMessageBox::Ok);
}
