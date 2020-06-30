#include "themehelper.h"
#include "utils.h"

#include <QApplication>
#include <QFile>
#include <QSettings>
#include <QPalette>
#include <QStyle>
#include <QStyleFactory>

using namespace std;

ThemeHelper::ThemeHelper(QObject *parent) : QObject(parent)
{}

void ThemeHelper::registerListen()
{
    // TODO to implement on Windows / Linux
    return;
}

bool ThemeHelper::isSystemDarkTheme()
{
#if defined (Q_OS_WIN)
    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",QSettings::NativeFormat);
    return settings.value("AppsUseLightTheme") == 0;
#elif defined (Q_OS_LINUX)
    return qApp->style()->standardPalette().color(QPalette::Window).toHsl().lightness() < 110;
#endif
}

void ThemeHelper::setupTheme()
{
    ConfigHelper *helper = Utils::getConfigHelper();

    if ((isSystemDarkTheme() && helper->getGeneralSettings().systemTheme == 2) || helper->getGeneralSettings().systemTheme == 1) {
        applyDarkPalette();
        applyDarkQss();
    }
    else {
        applyLightPalette();
        applyLightQss();
    }
}

void ThemeHelper::setupThemeOnChange(int index)
{
    if ((isSystemDarkTheme() && index == 2) || index == 1) {
        applyDarkPalette();
        applyDarkQss();
    }
    else {
        applyLightPalette();
        applyLightQss();
    }
}

void ThemeHelper::applyLightPalette()
{
    qApp->setPalette(QStyleFactory::create("Fusion")->standardPalette());
}

void ThemeHelper::applyLightQss()
{
    QFile qssFile(":/qss/light.qss");
    qssFile.open(QIODevice::ReadOnly | QIODevice::Text);
    qApp->setStyleSheet(qssFile.readAll());
}

void ThemeHelper::applyDarkPalette()
{
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53,53,53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(127,127,127));
    darkPalette.setColor(QPalette::Base, QColor(42,42,42));
    darkPalette.setColor(QPalette::AlternateBase, QColor(66,66,66));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Disabled, QPalette::Text, QColor(127,127,127));
    darkPalette.setColor(QPalette::Dark, QColor(35,35,35));
    darkPalette.setColor(QPalette::Shadow, QColor(20,20,20));
    darkPalette.setColor(QPalette::Button, QColor(53,53,53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(127,127,127));
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42,130,218));
    darkPalette.setColor(QPalette::Highlight, QColor(42,130,218));
    darkPalette.setColor(QPalette::Disabled, QPalette::Highlight, QColor(80,80,80));
    darkPalette.setColor(QPalette::HighlightedText, Qt::white);
    darkPalette.setColor(QPalette::Disabled, QPalette::HighlightedText, QColor(127,127,127));
    qApp->setPalette(darkPalette);
}

void ThemeHelper::applyDarkQss()
{
    QFile qssFile(":/qss/dark.qss");
    qssFile.open(QIODevice::ReadOnly | QIODevice::Text);
    qApp->setStyleSheet(qssFile.readAll());
}
