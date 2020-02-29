#include "statusnotifier.h"
#include "mainwindow.h"
#include <QApplication>
#ifdef Q_OS_MAC
#include <Cocoa/Cocoa.h>
#endif
#ifdef Q_OS_LINUX
#include <QDBusMessage>
#include <QDBusConnection>
#include <QDBusPendingCall>
#endif

StatusNotifier::StatusNotifier(MainWindow *w, ConfigHelper *confHelper, QObject *parent) :
    QObject(parent),
    window(w),
    configHelper(confHelper)
{
    systray.setIcon(QIcon(":/icons/icons/trojan-qt5-2.png"));
    systray.setToolTip(QString("Trojan-Qt5"));
    minimiseRestoreAction = new QAction(configHelper->isHideWindowOnStartup() ? tr("Restore") : tr("Minimise"), this);
    connect(minimiseRestoreAction, &QAction::triggered, this, &StatusNotifier::activate);
    initActions();
    systrayMenu.addAction(minimiseRestoreAction);
    systrayMenu.addAction(QIcon::fromTheme("application-exit", QIcon::fromTheme("exit")), tr("Quit"), qApp, SLOT(quit()));
    systray.setContextMenu(&systrayMenu);
    systray.show();
}

void StatusNotifier::initActions()
{
    trojanQt5Action = new QAction("Trojan-Qt5: Off"); // for displaying the status
    trojanQt5Action->setEnabled(false);
    proxyModeActionGroup = new QActionGroup(this);
    proxyModeActionGroup->setExclusive(true);
    pacAction = new QAction(tr("PAC Mode"), proxyModeActionGroup);
    globalAction = new QAction(tr("Global Mode"), proxyModeActionGroup);
    manualAction = new QAction(tr("Manually Mode"), proxyModeActionGroup);
    pacAction->setCheckable(true);
    globalAction->setCheckable(true);
    manualAction->setCheckable(true);
    systrayMenu.addAction(trojanQt5Action);
    systrayMenu.addSeparator();
    systrayMenu.addAction(pacAction);
    systrayMenu.addAction(globalAction);
    systrayMenu.addAction(manualAction);
    systrayMenu.addSeparator();
    if (configHelper->isEnablePACMode() && configHelper->isAutoSetSystemProxy()) {
        pacAction->setChecked(true);
    } else if (configHelper->isAutoSetSystemProxy()) {
        globalAction->setChecked(true);
    } else {
        manualAction->setChecked(true);
    }
    connect(proxyModeActionGroup, SIGNAL(triggered(QAction*)), this, SLOT(toggleProxyMode(QAction*)));

}

void StatusNotifier::toggleProxyMode(QAction *action)
{
    if (action == pacAction) {
        configHelper->setProxyMode(true, true);
    } else if (action == globalAction) {
        configHelper->setProxyMode(true, false);
    } else {
        configHelper->setProxyMode(false, false);
    }
}

void StatusNotifier::activate()
{
    if (!window->isVisible() || window->isMinimized()) {
        window->showNormal();
        window->activateWindow();
        window->raise();
        /** Show Dock Icon. */
        [NSApp setActivationPolicy: NSApplicationActivationPolicyRegular];
    } else {
        window->hide();
        /** Hide Dock Icon. */
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
        trojanQt5Action->setText("Trojan-Qt5: On");
        systray.setIcon(QIcon(":/icons/icons/trojan-qt5.png"));
    } else {
        trojanQt5Action->setText("Trojan-Qt5: Off");
        systray.setIcon(QIcon(":/icons/icons/trojan-qt5-2.png"));
    }
}

void StatusNotifier::onWindowVisibleChanged(bool visible)
{
    minimiseRestoreAction->setText(visible ? tr("Minimise") : tr("Restore"));
}
