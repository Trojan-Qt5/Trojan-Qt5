#include "themehelper.h"
#include "utils.h"

#include <QApplication>
#include <QFile>

#import <Foundation/Foundation.h>

using namespace std;

ThemeHelper::ThemeHelper(QObject *parent) : QObject(parent)
{

}

bool ThemeHelper::isSystemDarkTheme()
{
#if defined (Q_OS_WIN)
    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",QSettings::NativeFormat);
    return settings.value("AppsUseLightTheme") == 0;
#elif defined (Q_OS_LINUX)
    return false;
#endif
}

void ThemeHelper::setupThemeOnStartup()
{
    ConfigHelper *helper = Utils::getConfigHelper();

    if ((isSystemDarkTheme() && helper->getGeneralSettings()["systemTheme"] == 2) || helper->getGeneralSettings()["systemTheme"] == 1)
        applyDarkQss();
    else
        applyLightQss();
}

void ThemeHelper::applyLightQss()
{
    QFile qssFile(":/qss/light.qss");
    qssFile.open(QIODevice::ReadOnly | QIODevice::Text);
    qApp->setStyleSheet(qssFile.readAll());
}

void ThemeHelper::applyDarkQss()
{
    QFile qssFile(":/qss/dark.qss");
    qssFile.open(QIODevice::ReadOnly | QIODevice::Text);
    qApp->setStyleSheet(qssFile.readAll());
}
