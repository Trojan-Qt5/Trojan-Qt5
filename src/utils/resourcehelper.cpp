#include "resourcehelper.h"
#include "utils.h"

#include <QProcess>
#include <QDir>
#include <QFile>
#include <QStandardPaths>

ResourceHelper::ResourceHelper()
{
}

bool ResourceHelper::isSystemProxyHelperExist()
{
    QFile file("/Library/Application Support/Trojan-Qt5/proxy_conf_helper");
    return file.exists();
}

void ResourceHelper::installSystemProxyHelper()
{
    QDir fileDir = QDir::toNativeSeparators(Utils::getConfigPath() + "/temp");
    if (!fileDir.exists()) {
        fileDir.mkpath(".");
    }

    QFile::copy(":/bin/proxy_conf_helper", fileDir.path() + "/proxy_conf_helper");
    QFile::copy(":/scripts/install_helper.sh", fileDir.path() + "/install_helper.sh");

    QProcess *task = new QProcess;
    QStringList param;
    QString script = QString("do shell script \"bash %1 \" with administrator privileges").arg(fileDir.path() + "/install_helper.sh");
    param << "-l" << "AppleScript";
    task->start("/usr/bin/osascript", param);
    task->write(script.toUtf8());
    task->closeWriteChannel();
    task->waitForFinished();
}

void ResourceHelper::installTAPDriver()
{
    QDir configDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/Trojan-Qt5";
    if (!configDir.exists()) {
        configDir.mkpath(".");
    }

    QString tapInstallerPath = configDir.path() + "/tap-windows-9.21.2.exe";

    if (!QFile::exists(tapInstallerPath))
        QFile::copy(":/bin/tap-windows-9.21.2.exe", tapInstallerPath);

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
