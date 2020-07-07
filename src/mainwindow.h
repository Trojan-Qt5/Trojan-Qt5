/*
 * Copyright (C) 2014-2016 Symeon Huang <hzwhuang@gmail.com>
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLocalServer>
#include <QPointer>

#include "connectiontablemodel.h"
#include "connectionsortfilterproxymodel.h"
#include "confighelper.h"
#include "statusnotifier.h"
#include "subscribemanager.h"
#if defined (Q_OS_WIN)
#include "winsparkle.h"
#endif
#if defined (Q_OS_MAC)
#include "sparkle/CocoaInitializer.h"
#include "sparkle/SparkleAutoUpdater.h"
#endif

class StatusBar;


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(ConfigHelper *confHelper, QWidget *parent = 0);
    ~MainWindow();

    void startAutoStartConnections();
    QList<TQProfile> getAllServers();
    TQProfile getConnectedServer();
    void onAddServerFromSystemTray(QString type);
    void onToggleServerFromSystemTray(TQProfile profile);
    bool isInstanceRunning() const;

public slots:
    void onHandleDataFromUrlScheme(const QString &);
    void onAddURIFromSubscribe(TQProfile);

private:
    Ui::MainWindow *ui;

    QPointer<StatusBar> m_statusBar;

    ConnectionTableModel *model;
    ConnectionSortFilterProxyModel *proxyModel;
    ConfigHelper *configHelper;
    StatusNotifier *notifier;
    SubscribeManager *sbMgr;
#if defined (Q_OS_MAC)
    AutoUpdater* updater;
#endif

    QLocalServer* instanceServer;
    bool instanceRunning;
    void initLog();
    void initSparkle();
    void initSingleInstance();

    void newProfile(Connection *newCon);
    void editRow(int row);
    void blockChildrenSignals(bool);
    void checkCurrentIndex();
    void setupActionIcon();

    static const QUrl issueUrl;

private slots:
    void onToggleConnection(bool);
    void onImportGuiJson();
    void onImportConfigYaml();
    void onExportGuiJson();
    void onExportShadowrocketJson();
    void onExportSubscribe();
    void onSaveManually();
    void onAddManually(QString type);
    void onAddScreenQRCode();
    void onAddScreenQRCodeCapturer();
    void onAddQRCodeFile();
    void onAddFromURI();
    void onAddFromPasteBoardURI();
    void onAddFromConfigJSON();
    void onAddFromShadowrocketJSON();
    void onDelete();
    void onEdit();
    void onShare();
    void onConnect();
    void onForceConnect();
    void onDisconnect();
    void onConnectionStatusChanged(const int row, const bool running);
    void onLatencyTest();
    void onClearTrafficStats();
    void onMoveUp();
    void onMoveDown();
    void onGeneralSettings();
    void onUserRuleSettings();
    void checkCurrentIndex(const QModelIndex &index);
    void onAbout();
    void onGuiLog();
    void onCoreLog();
    void onReportBug();
    void onCustomContextMenuRequested(const QPoint &pos);
    void onFilterToggled(bool);
    void onFilterTextChanged(const QString &text);
    void onQRCodeCapturerResultFound(const QString &uris);
    void onCheckUpdate();
    void onStatusAvailable(QList<quint64> data);
    void onSingleInstanceConnect();

protected slots:
    void hideEvent(QHideEvent *e);
    void showEvent(QShowEvent *e);
    void closeEvent(QCloseEvent *e);
};
#endif // MAINWINDOW_H
