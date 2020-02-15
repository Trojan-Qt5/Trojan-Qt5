#include "statusnotifier.h"
#include "mainwindow.h"
#include <QApplication>
#ifdef Q_OS_LINUX
#include <QDBusMessage>
#include <QDBusConnection>
#include <QDBusPendingCall>
#endif

StatusNotifier::StatusNotifier(MainWindow *w, bool startHiden, QObject *parent) :
    QObject(parent),
    window(w)
{
    systray.setIcon(QIcon(":/icons/icons/trojan-qt5-2.png"));
    systray.setToolTip(QString("Trojan-Qt5"));
    connect(&systray, &QSystemTrayIcon::activated, [this](QSystemTrayIcon::ActivationReason r) {
        if (r != QSystemTrayIcon::Context) {
            this->activate();
        }
    });
    minimiseRestoreAction = new QAction(startHiden ? tr("Restore") : tr("Minimise"), this);
    connect(minimiseRestoreAction, &QAction::triggered, this, &StatusNotifier::activate);
    systrayMenu.addAction(minimiseRestoreAction);
    systrayMenu.addAction(QIcon::fromTheme("application-exit", QIcon::fromTheme("exit")), tr("Quit"), qApp, SLOT(quit()));
    systray.setContextMenu(&systrayMenu);
    systray.show();
}

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
        systray.setIcon(QIcon(":/icons/icons/trojan-qt5.png"));
    } else {
        systray.setIcon(QIcon(":/icons/icons/trojan-qt5-2.png"));
    }
}

void StatusNotifier::onWindowVisibleChanged(bool visible)
{
    minimiseRestoreAction->setText(visible ? tr("Minimise") : tr("Restore"));
}
