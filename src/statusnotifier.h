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

#include <QObject>
#include <QSystemTrayIcon>
#include <QMenu>

class MainWindow;

class StatusNotifier : public QObject
{
    Q_OBJECT
public:
    StatusNotifier(MainWindow *w, bool startHiden, QObject *parent = 0);

    void initActions();
    void initConnections();
    void updateMenu();

signals:
    void toggleConnection(bool);

public slots:
    void activate();
    void onToggleMode(QAction *action);
    void onToggleConnection();
    void showNotification(const QString &);
    void changeIcon(bool started);
    void onWindowVisibleChanged(bool visible);

private:
    QMenu systrayMenu;
    QAction *trojanQt5Action;
    QAction *toggleTrojanAction;
    QMenu *ModeMenu;
    QMenu *pacMenu;
    QActionGroup *ModeGroup;
    QAction *disableModeAction;
    QAction *pacModeAction;
    QAction *globalModeAction;
    QAction *updatePACToBypassLAN;
    QAction *updatePACToChnWhite;
    QAction *updatePACToChnIP;
    QAction *updatePACToGFWList;
    QAction *updatePACToChnOnly;
    QAction *copyPACUrl;
    QAction *editLocalPACFile;
    QAction *editGFWListUserRule;

    QAction *minimiseRestoreAction;
    QSystemTrayIcon systray;
    MainWindow *window;
};

#endif // STATUSNOTIFIER_H
