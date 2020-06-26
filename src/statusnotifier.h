/*
 * Copyright (C) 2015-2017 Symeon Huang <hzwhuang@gmail.com>
 *
 * shadowsocks-qt5 is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * shadowsocks-qt5 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libQtShadowsocks; see the file LICENSE. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef STATUSNOTIFIER_H
#define STATUSNOTIFIER_H

#include "subscribemanager.h"

#include <QObject>
#include <QSystemTrayIcon>
#include <QMenu>

class MainWindow;

class StatusNotifier : public QObject
{
    Q_OBJECT
public:
    StatusNotifier(MainWindow *w, ConfigHelper *ch, SubscribeManager *sm, QObject *parent = 0);

    void initActions();
    void initConnections();
    void updateMenu();
    void updateServersMenu();
    void onCopyTerminalProxy(QString type);
    void onSetProxyToTelegram();
    void onTrojanSubscribeSettings();
#if defined (Q_OS_WIN)
    void onInstallTAPDriver();
#endif
    void showServerSpeedPlot();

signals:
    void toggleConnection(bool);

public slots:
    void activate();
    void onUpdateSubscribeWithProxy();
    void onUpdateSubscribe();
    void onToggleMode(QAction *action);
    void onToggleConnection();
    void onToggleServer(QAction *actived);
    void onToggleServerLoadBalance(bool checked);
    void showNotification(const QString &);
    void changeIcon(bool started);
    void onWindowVisibleChanged(bool visible);

private:

    QMenu systrayMenu;
    QMenu *ModeMenu;
    QMenu *pacMenu;
    QMenu *serverMenu;
    QMenu *addServerMenu;
    QMenu *subscribeMenu;
    QAction *trojanQt5Action;
    QAction *toggleTrojanAction;
    QAction *disableModeAction;
    QAction *pacModeAction;
    QAction *globalModeAction;
    QAction *advanceModeAction;
    QAction *serverLoadBalance;
    QAction *addManually;
    QAction *addFromScreenQRCode;
    QAction *addFromPasteBoardUri;

    QAction *updatePACToBypassLAN;
    QAction *updatePACToChnWhite;
    QAction *updatePACToChnWhiteAdvanced;
    QAction *updatePACToChnIP;
    QAction *updatePACToGFWList;
    QAction *updatePACToChnOnly;
    QAction *copyPACUrl;
    QAction *editLocalPACFile;
    QAction *editGFWListUserRule;
    QAction *subscribeSettings;
    QAction *updateSubscribe;
    QAction *updateSubscribeBypass;
    QActionGroup *ModeGroup;
    QActionGroup *ServerGroup;

    QAction *serverSpeedPlot;
    QMenu *copyTerminalProxyCommandMenu;
    QAction *terminalWinStyle;
    QAction *terminalUnixStyle;
    QAction *setProxyToTelegram;
    QAction *minimiseRestoreAction;
#if defined (Q_OS_WIN)
    QAction *installTapDriver;
#endif
    QSystemTrayIcon systray;
    MainWindow *window;
    ConfigHelper *helper;
    SubscribeManager *sbMgr;
};

#endif // STATUSNOTIFIER_H
