#ifndef THEMEHELPER_H
#define THEMEHELPER_H

#include <QObject>

class ThemeHelper : public QObject
{
    Q_OBJECT
public:
    explicit ThemeHelper(QObject *parent = nullptr);

    static bool isSystemDarkTheme();

    static void setupThemeOnStartup();

    static void applyLightPalette();
    static void applyDarkPalette();

    static void applyLightQss();
    static void applyDarkQss();

signals:

};

#endif // THEMEHELPER_H
