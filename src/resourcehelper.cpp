#include "resourcehelper.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>

ResourceHelper::ResourceHelper()
{
}

void ResourceHelper::initPrivoxy()
{
    QDir configDir = QCoreApplication::applicationDirPath() + "/privoxy";
    if (!configDir.exists()) {
        configDir.mkpath(".");
    }

    QString privoxy = QCoreApplication::applicationDirPath() + "/privoxy/privoxy.exe";
    QFile::copy(":/bin/privoxy.exe", privoxy);
}

