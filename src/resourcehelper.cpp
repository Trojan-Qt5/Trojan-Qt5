#include "resourcehelper.h"

#include <QProcess>
#include <QDir>
#include <QFile>
#include <QStandardPaths>

ResourceHelper::ResourceHelper()
{
}

void ResourceHelper::installTAPDriver()
{
    QDir configDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/Trojan-Qt5";
    if (!configDir.exists()) {
        configDir.mkpath(".");
    }

    QString tapInstallerPath = configDir.dirName() + "/tap-windows-9.22.1-I602.exe";

    if (!QFile::exists(tapInstallerPath))
        QFile::copy(":/bin/tap-windows-9.22.1-I602.exe", tapInstallerPath);

    QProcess::startDetached(tapInstallerPath);
}

