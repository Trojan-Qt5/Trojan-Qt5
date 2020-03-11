#include "statusnotifier.h"
#include "mainwindow.h"
#include "pacserver.h"
#include "systemproxyhelper.h"
#include <QApplication>
#include <Cocoa/Cocoa.h>

StatusNotifier::StatusNotifier(MainWindow *w, bool startHiden, QObject *parent) :
    QObject(parent),
    window(w)
{
    systray.setIcon(QIcon(":/icons/icons/trojan-qt5-2.png"));
    systray.setToolTip(QString("Trojan-Qt5"));
    minimiseRestoreAction = new QAction(startHiden ? tr("Restore") : tr("Minimise"), this);
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
#ifdef Q_OS_WIN
    QString configFile = QCoreApplication::applicationDirPath() + "/config.ini";
#else
    QDir configDir = QDir::homePath() + "/.config/trojan-qt5";
    QString configFile = configDir.absolutePath() + "/config.ini";
#endif
    ConfigHelper *conf = new ConfigHelper(configFile);

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
    disableModeAction->setCheckable(true);
    pacModeAction->setCheckable(true);
    globalModeAction->setCheckable(true);
    ModeMenu->addAction(disableModeAction);
    ModeMenu->addAction(pacModeAction);
    ModeMenu->addAction(globalModeAction);
    if (conf->isEnablePACMode() && conf->isAutoSetSystemProxy())
        pacModeAction->setChecked(true);
    else if (conf->isAutoSetSystemProxy())
        globalModeAction->setChecked(true);
    else
        disableModeAction->setChecked(true);

    //PAC Menu
    pacMenu = new QMenu(tr("PAC"));
    updatePACToBypassLAN = new QAction(tr("Update local PAC from Lan IP list"));
    updatePACToChnWhite = new QAction(tr("Update local PAC from Chn White list"));
    updatePACToChnIP = new QAction(tr("Update local PAC from Chn IP list"));
    updatePACToGFWList = new QAction(tr("Update local PAC from GFWList"));
    updatePACToChnOnly = new QAction(tr("Update local PAC from Chn Only list"));
    copyPACUrl = new QAction(tr("Copy PAC URL"));
    editLocalPACFile = new QAction(tr("Edit local PAC file"));
    editGFWListUserRule = new QAction(tr("Edit user rule for GFWList"));
    pacMenu->addAction(updatePACToBypassLAN);
    pacMenu->addSeparator();
    pacMenu->addAction(updatePACToChnWhite);
    pacMenu->addAction(updatePACToChnIP);
    pacMenu->addAction(updatePACToGFWList);
    pacMenu->addSeparator();
    pacMenu->addAction(updatePACToChnOnly);
    pacMenu->addSeparator();
    pacMenu->addAction(copyPACUrl);
    pacMenu->addAction(editLocalPACFile);
    pacMenu->addAction(editGFWListUserRule);

    //setup systray Menu
    systrayMenu.addAction(trojanQt5Action);
    systrayMenu.addAction(toggleTrojanAction);
    systrayMenu.addSeparator();
    systrayMenu.addMenu(ModeMenu);
    systrayMenu.addMenu(pacMenu);
    systrayMenu.addSeparator();

    connect(toggleTrojanAction, &QAction::triggered, this, &StatusNotifier::onToggleConnection);
    connect(ModeGroup, SIGNAL(triggered(QAction*)), this, SLOT(onToggleMode(QAction*)));

}

void StatusNotifier::initConnections()
{
    PACServer *pacserver = new PACServer();
    connect(updatePACToBypassLAN, &QAction::triggered, pacserver, [=]() { pacserver->typeModify("LAN"); });
    connect(updatePACToChnWhite, &QAction::triggered, pacserver, [=]() { pacserver->typeModify("WHITE"); });
    connect(updatePACToChnIP, &QAction::triggered, pacserver, [=]() { pacserver->typeModify("CNIP"); });
    connect(updatePACToGFWList, &QAction::triggered, pacserver, [=]() { pacserver->typeModify("GFWLIST"); });
    connect(updatePACToChnOnly, &QAction::triggered, pacserver, [=]() { pacserver->typeModify("WHITE_R"); });
    connect(copyPACUrl, &QAction::triggered, pacserver, [=]() { pacserver->copyPACUrl(); });
    connect(editLocalPACFile, &QAction::triggered, pacserver, [=]() { pacserver->editLocalPACFile(); });
    connect(editGFWListUserRule, &QAction::triggered, pacserver, [=]() { pacserver->editUserRule(); });
}

void StatusNotifier::onToggleMode(QAction *action)
{
#ifdef Q_OS_WIN
    QString configFile = QCoreApplication::applicationDirPath() + "/config.ini";
#else
    QDir configDir = QDir::homePath() + "/.config/trojan-qt5";
    QString configFile = configDir.absolutePath() + "/config.ini";
#endif
    ConfigHelper *conf = new ConfigHelper(configFile);

    SystemProxyHelper *sph = new SystemProxyHelper();
    if (action == disableModeAction) {
        sph->setSystemProxy(0);
        conf->setSystemProxySettings(false, false);
    } else if (action == pacModeAction) {
        sph->setSystemProxy(2);
        conf->setSystemProxySettings(true, true);
    } else if (action == globalModeAction) {
        sph->setSystemProxy(1);
        conf->setSystemProxySettings(false, true);
    }
}

void StatusNotifier::onToggleConnection()
{
    if (toggleTrojanAction->text() == tr("Turn Off Trojan")) {
        emit toggleConnection(false);
    }
    else {
        emit toggleConnection(true);
    }
}

void StatusNotifier::activate()
{
    if (!window->isVisible() || window->isMinimized()) {
        window->showNormal();
        window->activateWindow();
        window->raise();
        //show Dock Icon
        [NSApp setActivationPolicy: NSApplicationActivationPolicyRegular];
    } else {
        window->hide();
        //hide Dock Icon
        [NSApp setActivationPolicy: NSApplicationActivationPolicyProhibited];
    }
}

void StatusNotifier::showNotification(const QString &msg)
{
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

void StatusNotifier::changeIcon(bool started)
{
    if (started) {
        trojanQt5Action->setText(tr("Trojan: On"));
        toggleTrojanAction->setText(tr("Turn Off Trojan"));
        systray.setIcon(QIcon(":/icons/icons/trojan-qt5.png"));
    } else {
        trojanQt5Action->setText(tr("Trojan: Off"));
        toggleTrojanAction->setText(tr("Turn On Trojan"));
        systray.setIcon(QIcon(":/icons/icons/trojan-qt5-2.png"));
    }
}

void StatusNotifier::onWindowVisibleChanged(bool visible)
{
    minimiseRestoreAction->setText(visible ? tr("Minimise") : tr("Restore"));
}
