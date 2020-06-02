#include "resourcehelper.h"
#include "utils.h"

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

    QString tapInstallerPath = configDir.path() + "/tap-windows-9.22.1-I602.exe";

    if (!QFile::exists(tapInstallerPath))
        QFile::copy(":/bin/tap-windows-9.22.1-I602.exe", tapInstallerPath);

    QStringList param;
    param << "/S";
    QProcess::startDetached(tapInstallerPath, param);
}

void ResourceHelper::copyDatFiles()
{
    QDir fileDir = QDir::toNativeSeparators(Utils::getConfigPath() + "/dat");
    if (!fileDir.exists()) {
        fileDir.mkpath(".");
    }

    QFile geoipFile = fileDir.path() + "/geoip.dat";
    QFile geositeFile = fileDir.path() + "/geosite.dat";

    if (!geoipFile.exists()) {
        QFile::copy(":/dat/geoip.dat", fileDir.path() + "/geoip.dat");
    }

    if (!geositeFile.exists()) {
        QFile::copy(":/dat/geosite.dat", fileDir.path() + "/geosite.dat");
    }

}
