#ifndef THEMEHELPER_H
#define THEMEHELPER_H

#include <QObject>

class ThemeHelper : public QObject
{
    Q_OBJECT
public:
    explicit ThemeHelper(QObject *parent = nullptr);

    static bool isSystemDarkTheme();

    static void registerListen();
    static void setupTheme();
    static void setupThemeOnChange(int);

    static void applyLightPalette();
    static void applyDarkPalette();

    static void applyLightQss();
    static void applyDarkQss();


};

#endif // THEMEHELPER_H
