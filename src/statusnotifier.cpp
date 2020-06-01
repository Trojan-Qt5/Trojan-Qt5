#include "statusnotifier.h"
#include "mainwindow.h"
#include "pachelper.h"
#include "systemproxyhelper.h"
#include "subscribedialog.h"
#include "subscribemanager.h"
#include "resourcehelper.h"
#include "speedplot.h"

#include <QApplication>
#include <QClipboard>
#include <QColor>
#include <QDesktopServices>
#ifdef Q_OS_LINUX
#include <QDBusMessage>
#include <QDBusConnection>
#include <QDBusPendingCall>
#endif

StatusNotifier::StatusNotifier(MainWindow *w, ConfigHelper *ch, SubscribeManager *sm, QObject *parent) :
    QObject(parent),
    window(w),
    helper(ch),
    sbMgr(sm)
{
    systray.setIcon(QIcon(":/icons/icons/trojan-qt5_off.png"));
    systray.setToolTip(QString("Trojan-Qt5"));
    connect(&systray, &QSystemTrayIcon::activated, [=]() {
        updateMenu();
        updateServersMenu();
    });
    connect(&systray, &QSystemTrayIcon::activated, [this](QSystemTrayIcon::ActivationReason r) {
        if (r == QSystemTrayIcon::DoubleClick) {
            window->showNormal();
            window->activateWindow();
            window->raise();
        }
    });
    minimiseRestoreAction = new QAction(helper->getGeneralSettings()["hideWindowOnStartup"].toBool() ? tr("Restore") : tr("Minimise"), this);
    connect(minimiseRestoreAction, &QAction::triggered, this, &StatusNotifier::activate);
    initActions();
    initConnections();
    systrayMenu.addAction(minimiseRestoreAction);
    systrayMenu.addAction(QIcon::fromTheme("application-exit", QIcon::fromTheme("exit")), tr("Quit"), qApp, SLOT(quit()));
    systray.setContextMenu(&systrayMenu);
    systray.show();
}

void StatusNotifier::initActions()
{
    //trojan Status and Toggle Action
    trojanQt5Action = new QAction(tr("Trojan: Off")); // for displaying the status
    trojanQt5Action->setEnabled(false);
    toggleTrojanAction = new QAction(tr("Turn On Trojan"));
    toggleTrojanAction->setShortcut(Qt::CTRL + Qt::Key_T);

    //Mode Menu
    ModeMenu = new QMenu(tr("Mode"));
    ModeGroup = new QActionGroup(this);
    ModeGroup->setExclusive(true);
    disableModeAction = new QAction(tr("Disable system proxy"), ModeGroup);
    pacModeAction = new QAction(tr("PAC"), ModeGroup);
    globalModeAction = new QAction(tr("Global"), ModeGroup);
    advanceModeAction = new QAction(tr("Advance"), ModeGroup);
    disableModeAction->setCheckable(true);
    pacModeAction->setCheckable(true);
    globalModeAction->setCheckable(true);
    advanceModeAction->setCheckable(true);
    ModeMenu->addAction(disableModeAction);
    ModeMenu->addAction(pacModeAction);
    ModeMenu->addAction(globalModeAction);
    ModeMenu->addAction(advanceModeAction);
    if (helper->getSystemProxySettings() == "pac")
        pacModeAction->setChecked(true);
    else if (helper->getSystemProxySettings() == "global")
        globalModeAction->setChecked(true);
    else if (helper->getSystemProxySettings() == "direct")
        disableModeAction->setChecked(true);
    else if (helper->getSystemProxySettings() == "advance")
        advanceModeAction->setChecked(true);

    //PAC Menu
    pacMenu = new QMenu(tr("PAC"));
    updatePACToBypassLAN = new QAction(tr("Update local PAC from Lan IP list"));
    updatePACToChnWhite = new QAction(tr("Update local PAC from Chn White list"));
    updatePACToChnWhiteAdvanced = new QAction(tr("Update local from Chn Advance White list")); //Advance White list by @zoeysama
    updatePACToChnIP = new QAction(tr("Update local PAC from Chn IP list"));
    updatePACToGFWList = new QAction(tr("Update local PAC from GFWList"));
    updatePACToChnOnly = new QAction(tr("Update local PAC from Chn Only list"));
    copyPACUrl = new QAction(tr("Copy PAC URL"));
    editLocalPACFile = new QAction(tr("Edit local PAC file"));
    editGFWListUserRule = new QAction(tr("Edit user rule for GFWList"));
    pacMenu->addAction(updatePACToBypassLAN);
    pacMenu->addSeparator();
    pacMenu->addAction(updatePACToChnWhite);
    pacMenu->addAction(updatePACToChnWhiteAdvanced);
    pacMenu->addAction(updatePACToChnIP);
    pacMenu->addAction(updatePACToGFWList);
    pacMenu->addSeparator();
    pacMenu->addAction(updatePACToChnOnly);
    pacMenu->addSeparator();
    pacMenu->addAction(copyPACUrl);
    pacMenu->addAction(editLocalPACFile);
    pacMenu->addAction(editGFWListUserRule);

    //server Menu
    serverMenu = new QMenu(tr("Servers"));
    ServerGroup = new QActionGroup(this);
    ServerGroup->setExclusive(true);
    addServerMenu = new QMenu(tr("Add Server"));
    addManually = new QAction(tr("Add Manually"));
    addFromScreenQRCode = new QAction(tr("Scan QRCode on Screen"));
    addFromPasteBoardUri = new QAction(tr("Add From Pasteboard Uri"));
    addServerMenu->addAction(addManually);
    addServerMenu->addAction(addFromScreenQRCode);
    addServerMenu->addAction(addFromPasteBoardUri);

    //subscribe Menu
    subscribeMenu = new QMenu(tr("Servers Subscribe"));
    subscribeSettings = new QAction(tr("Subscribe setting"));
    updateSubscribe = new QAction(tr("Update subscribe node"));
    updateSubscribeBypass = new QAction(tr("Update subscribe node(bypass proxy)"));
    subscribeMenu->addAction(subscribeSettings);
    subscribeMenu->addAction(updateSubscribe);
    subscribeMenu->addAction(updateSubscribeBypass);

    serverLoadBalance = new QAction(tr("Server Load Balance"));
    serverLoadBalance->setCheckable(true);

    serverSpeedPlot = new QAction(tr("Server Speed Plot"));
    copyTerminalProxyCommand = new QAction(tr("Copy terminal proxy command"));
    setProxyToTelegram = new QAction(tr("Set Proxy to Telegram"));
#if defined (Q_OS_WIN)
    installTapDriver = new QAction(tr("Instal TAP Driver"));
#endif

    //setup systray Menu
    systrayMenu.addAction(trojanQt5Action);
    systrayMenu.addAction(toggleTrojanAction);
    systrayMenu.addSeparator();
    systrayMenu.addMenu(ModeMenu);
    systrayMenu.addMenu(pacMenu);
    systrayMenu.addSeparator();
    systrayMenu.addMenu(serverMenu);
    systrayMenu.addMenu(subscribeMenu);
    systrayMenu.addAction(serverLoadBalance);
    systrayMenu.addSeparator();
    systrayMenu.addAction(serverSpeedPlot);
    systrayMenu.addAction(copyTerminalProxyCommand);
    systrayMenu.addAction(setProxyToTelegram);
#if defined (Q_OS_WIN)
    systrayMenu.addAction(installTapDriver);
#endif
    systrayMenu.addSeparator();

    connect(toggleTrojanAction, &QAction::triggered, this, &StatusNotifier::onToggleConnection);
    connect(addManually, &QAction::triggered, window, [=]() { window->onAddServerFromSystemTray("manually"); });
    connect(addFromScreenQRCode, &QAction::triggered, window, [=]() { window->onAddServerFromSystemTray("qrcode"); });
    connect(addFromPasteBoardUri, &QAction::triggered, window, [=]() { window->onAddServerFromSystemTray("pasteboard"); });
    connect(serverLoadBalance, &QAction::triggered, this, [this]() { onToggleServerLoadBalance(serverLoadBalance->isChecked()); });
    connect(ModeGroup, SIGNAL(triggered(QAction*)), this, SLOT(onToggleMode(QAction*)));
    connect(ServerGroup, SIGNAL(triggered(QAction*)), this, SLOT(onToggleServer(QAction*)));
}

void StatusNotifier::initConnections()
{
    PACHelper *pachelper = new PACHelper();
    connect(updatePACToBypassLAN, &QAction::triggered, pachelper, [=]() { pachelper->typeModify("LAN"); });
    connect(updatePACToChnWhite, &QAction::triggered, pachelper, [=]() { pachelper->typeModify("WHITE"); });
    connect(updatePACToChnWhiteAdvanced, &QAction::triggered, pachelper, [=]() { pachelper->typeModify("WHITE_ADVANCED"); });
    connect(updatePACToChnIP, &QAction::triggered, pachelper, [=]() { pachelper->typeModify("CNIP"); });
    connect(updatePACToGFWList, &QAction::triggered, pachelper, [=]() { pachelper->typeModify("GFWLIST"); });
    connect(updatePACToChnOnly, &QAction::triggered, pachelper, [=]() { pachelper->typeModify("WHITE_R"); });
    connect(copyPACUrl, &QAction::triggered, pachelper, [=]() { pachelper->copyPACUrl(); });
    connect(editLocalPACFile, &QAction::triggered, pachelper, [=]() { pachelper->editLocalPACFile(); });
    connect(editGFWListUserRule, &QAction::triggered, pachelper, [=]() { pachelper->editUserRule(); });
    connect(subscribeSettings, &QAction::triggered, this, [this]() { onTrojanSubscribeSettings(); });
    connect(updateSubscribe, &QAction::triggered, this, &StatusNotifier::onUpdateSubscribeWithProxy);
    connect(updateSubscribeBypass, &QAction::triggered, this, &StatusNotifier::onUpdateSubscribe);
    connect(serverSpeedPlot, &QAction::triggered, this, [this]() { showServerSpeedPlot(); });
    connect(copyTerminalProxyCommand, &QAction::triggered, this, [this]() { onCopyTerminalProxy(); });
    connect(setProxyToTelegram, &QAction::triggered, this, [this]() { onSetProxyToTelegram(); });
#if defined (Q_OS_WIN)
    connect(installTapDriver, &QAction::triggered, this, [this]() { onInstallTAPDriver(); });
#endif
}

void StatusNotifier::updateMenu()
{
    if (helper->getSystemProxySettings() == "pac")
        pacModeAction->setChecked(true);
    else if (helper->getSystemProxySettings() == "global")
        globalModeAction->setChecked(true);
    else if (helper->getSystemProxySettings() == "disable")
        disableModeAction->setChecked(true);
    else if (helper->getSystemProxySettings() == "advance")
        advanceModeAction->setChecked(true);

    serverLoadBalance->setChecked(helper->isEnableServerLoadBalance());
}

void StatusNotifier::updateServersMenu()
{
    QList<TQProfile> serverList = window->getAllServers();
    TQProfile connected = window->getConnectedServer();
    serverMenu->clear();
    serverMenu->addMenu(addServerMenu);
    serverMenu->addSeparator();
    for (int i=0; i<serverList.size(); i++) {
        QAction *action = new QAction(serverList[i].name, ServerGroup);
        action->setCheckable(false);
        action->setIcon(QIcon(QString(":/icons/icons/%1_off.png").arg(serverList[i].type)));
        if (serverList[i].equals(connected))
            action->setIcon(QIcon(QString(":/icons/icons/%1_on.png").arg(serverList[i].type)));
        serverMenu->addAction(action);
    }
}

void StatusNotifier::onToggleMode(QAction *action)
{
    SystemProxyHelper *sph = new SystemProxyHelper();
    helper->readGeneralSettings();

    if (action == disableModeAction) {
        if (helper->isTrojanOn())
            sph->setSystemProxy(0);
        helper->setSystemProxySettings("direct");
    } else if (action == pacModeAction) {
        if (helper->isTrojanOn()) {
            sph->setSystemProxy(0);
            sph->setSystemProxy(2);
        }
        helper->setSystemProxySettings("pac");
    } else if (action == globalModeAction) {
        if (helper->isTrojanOn()) {
            sph->setSystemProxy(0);
            sph->setSystemProxy(1);
        }
        helper->setSystemProxySettings("global");
    } else if (action == advanceModeAction) {
        helper->setSystemProxySettings("advance");
    }

    if (helper->isTrojanOn())
        changeIcon(true);
    else
        changeIcon(false);
}

void StatusNotifier::onUpdateSubscribeWithProxy()
{
    sbMgr = new SubscribeManager(window, helper);
    sbMgr->setUseProxy(true);
    sbMgr->updateAllSubscribesWithThread();
}

void StatusNotifier::onUpdateSubscribe()
{
    sbMgr = new SubscribeManager(window, helper);
    sbMgr->setUseProxy(false);
    sbMgr->updateAllSubscribesWithThread();
}

void StatusNotifier::onToggleConnection()
{
    if (toggleTrojanAction->text() == tr("Turn Off Trojan"))
        emit toggleConnection(false);
    else
        emit toggleConnection(true);
}

void StatusNotifier::onToggleServer(QAction *actived)
{
    QList<TQProfile> serverList = window->getAllServers();
    for (int i=0; i<serverList.size(); i++)
        if (actived->text() == serverList[i].name)
            window->onToggleServerFromSystemTray(serverList[i]);
}

void StatusNotifier::onTrojanSubscribeSettings()
{
    SubscribeDialog *sbDig = new SubscribeDialog(helper);
    sbDig->exec();
}

void StatusNotifier::onToggleServerLoadBalance(bool checked)
{
    helper->readGeneralSettings();
    helper->setServerLoadBalance(checked);
    changeIcon(helper->isTrojanOn());
}

void StatusNotifier::onCopyTerminalProxy()
{
    QClipboard *board = QApplication::clipboard();
    if (helper->getInboundSettings()["enableHttpMode"].toBool())
        board->setText(QString("export HTTP_PROXY=http://127.0.0.1:%1; export HTTPS_PROXY=http://127.0.0.1:%1; export ALL_PROXY=socks5://127.0.0.1:%2").arg(helper->getInboundSettings()["httpLocalPort"].toInt()).arg(helper->getInboundSettings()["socks5LocalPort"].toInt()));
    else
        board->setText(QString("export HTTP_PROXY=socks5://127.0.0.1:%1; export HTTPS_PROXY=socks5://127.0.0.1:%1; export ALL_PROXY=socks5://127.0.0.1:%1").arg(helper->getInboundSettings()["socks5LocalPort"].toInt()));
}

void StatusNotifier::onSetProxyToTelegram()
{
    QDesktopServices::openUrl(QString("tg://socks?server=127.0.0.1&port=%2").arg(helper->getInboundSettings()["socks5LocalPort"].toInt()));
}

#if defined (Q_OS_WIN)
void StatusNotifier::onInstallTAPDriver()
{
    ResourceHelper::installTAPDriver();
}
#endif

void StatusNotifier::activate()
{
    if (!window->isVisible() || window->isMinimized()) {
        window->showNormal();
        window->activateWindow();
        window->raise();
    } else {
        window->hide();
    }
}

void StatusNotifier::showNotification(const QString &msg)
{
    if (helper->getGeneralSettings()["enableNotification"].toBool()) {
#ifdef Q_OS_LINUX
        //Using DBus to send message.
        QDBusMessage method = QDBusMessage::createMethodCall("org.freedesktop.Notifications","/org/freedesktop/Notifications", "org.freedesktop.Notifications", "Notify");
        QVariantList args;
        args << QCoreApplication::applicationName() << quint32(0) << "trojan-qt5" << "Trojan-Qt5" << msg << QStringList () << QVariantMap() << qint32(-1);
        method.setArguments(args);
        QDBusConnection::sessionBus().asyncCall(method);
#else
        systray.showMessage("Trojan-Qt5", msg);
#endif
    }
}

void StatusNotifier::changeIcon(bool started)
{
    if (started) {
        trojanQt5Action->setText(tr("Trojan: On"));
        toggleTrojanAction->setText(tr("Turn Off Trojan"));

        bool enabled = helper->getSystemProxySettings() != "direct";
        bool global = helper->getSystemProxySettings() == "global";
        bool advance = helper->getSystemProxySettings() == "advance";
        bool random = helper->isEnableServerLoadBalance();
        QString mode = helper->getSystemProxySettings();
        QImage image(QString(":/icons/icons/trojan-qt5_%1.png").arg(mode));
        QImage alpha = image.alphaChannel();

        double mul_r = 1.0, mul_g = 1.0, mul_b = 1.0;
        if (!enabled) {
            mul_g = 0.4;
        }
        else if (!global) {
            mul_b = 0.4;
            mul_g = 0.65;
        }
        if (advance) {
            mul_b = 0.2;
            mul_g = 0.3;
        }
        if (!random) {
            mul_r = 0.4;
        }

        for (int x = 0; x < image.width(); x++) {
            for (int y = 0; y < image.height(); y++) {
                // https://www.qtcentre.org/threads/16090-SOLVED-QImage-setPixel-alpha-issue
                int a = qAlpha(image.pixel(x, y));
                QColor color(image.pixel(x, y));

                if (a > 0)
                    image.setPixel(x, y, qRgb(
                                     color.red() * mul_r,
                                     color.green() * mul_g,
                                     color.blue() * mul_b));
            }
        }

        QIcon icon(QPixmap::fromImage(image));
        systray.setIcon(icon);
    } else {
        trojanQt5Action->setText(tr("Trojan: Off"));
        toggleTrojanAction->setText(tr("Turn On Trojan"));
        systray.setIcon(QIcon(":/icons/icons/trojan-qt5_off.png"));
    }
}

void StatusNotifier::showServerSpeedPlot()
{
    SpeedPlot *sp = new SpeedPlot();
    sp->show();
    sp->raise();
    sp->setFocus();
}

void StatusNotifier::onWindowVisibleChanged(bool visible)
{
    minimiseRestoreAction->setText(visible ? tr("Minimise") : tr("Restore"));
}
