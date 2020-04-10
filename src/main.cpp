#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>
#include <QLocale>
#include <QMessageBox>
#include <QDir>
#include <QCommandLineParser>
#include <QHttpServer>
#include <signal.h>
#include "mainwindow.h"
#include "confighelper.h"
#include "resourcehelper.h"
#include "logger.h"

#include "LetsMove/PFMoveApplication.h"

#ifdef Q_OS_MAC
#include <objc/objc.h>
#include <objc/message.h>
void setupDockClickHandler();
bool dockClickHandler(id self,SEL _cmd,...);
#endif

MainWindow *mainWindow = nullptr;

static void onSignalRecv(int sig)
{
    if (sig == SIGINT || sig == SIGTERM) {
        qApp->quit();
    } else {
        qWarning("Unhandled signal %d", sig);
    }
}

void setupApplication(QApplication &a)
{
    signal(SIGINT, onSignalRecv);
    signal(SIGTERM, onSignalRecv);

    QApplication::setFallbackSessionManagementEnabled(false);

    a.setApplicationName("trojan-qt5");
    a.setApplicationDisplayName("Trojan-Qt5");
    a.setApplicationVersion(APP_VERSION);

    // https://stackoverflow.com/questions/46143546/application-closes-when-its-hidden-and-i-close-a-modal-dialog
    a.setQuitOnLastWindowClosed(false); // prevent application quit when mainwindow is hidden and user closed dialog

#ifdef Q_OS_WIN
    if (QLocale::system().country() == QLocale::China) {
        a.setFont(QFont("Microsoft Yahei", 9, QFont::Normal, false));
    }
    else {
        a.setFont(QFont("Segoe UI", 9, QFont::Normal, false));
    }
#endif
#if defined(Q_OS_WIN) || defined(Q_OS_MAC)
    QIcon::setThemeName("Breeze");
#endif

    QTranslator *trojanqt5t = new QTranslator(&a);
    trojanqt5t->load(QString(":/i18n/trojan-qt5_%1").arg(QLocale::system().name()));
    a.installTranslator(trojanqt5t);
}

#if defined (Q_OS_MAC)
/**
 * @brief setupDockClickHandler
 * @ref https://stackoverflow.com/questions/15143369/qt-on-os-x-how-to-detect-clicking-the-app-dock-icon
 */
void setupDockClickHandler() {
    Class cls = objc_getClass("NSApplication");
    objc_object *appInst = (objc_object*)objc_msgSend((id)cls, sel_registerName("sharedApplication"));

    if(appInst != NULL) {
        objc_object* delegate = (objc_object*)objc_msgSend((id)appInst, sel_registerName("delegate"));
        Class delClass = (Class)objc_msgSend((id)delegate,  sel_registerName("class"));
        SEL shouldHandle = sel_registerName("applicationShouldHandleReopen:hasVisibleWindows:");
        if (class_getInstanceMethod(delClass, shouldHandle)) {
            if (class_replaceMethod(delClass, shouldHandle, (IMP)dockClickHandler, "B@:"))
                Logger::debug("Registered dock click handler (replaced original method");
            else
                Logger::warning("Failed to replace method for dock click handler");
        }
        else {
            if (class_addMethod(delClass, shouldHandle, (IMP)dockClickHandler,"B@:"))
                Logger::debug("Registered dock click handler");
            else
                Logger::warning("Failed to register dock click handler");
        }
    }
}

bool dockClickHandler(id self,SEL _cmd,...)
{
    Q_UNUSED(self);
    Q_UNUSED(_cmd);

    mainWindow->showNormal();
    mainWindow->activateWindow();
    mainWindow->raise();

    // Return NO (false) to suppress the default OS X actions
    return false;
}

#endif

int main(int argc, char *argv[])
{

    qRegisterMetaTypeStreamOperators<TQProfile>("TQProfile");
    qRegisterMetaTypeStreamOperators<TQSubscribe>("TQSubscribe");

    QApplication a(argc, argv);
    setupApplication(a);

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    QCommandLineOption configFileOption("c",
                                        "specify configuration file.",
                                        "config.ini");
    parser.addOption(configFileOption);
    parser.process(a);

    QString configFile = parser.value(configFileOption);
    if (configFile.isEmpty()) {
#ifdef Q_OS_WIN
        ResourceHelper::initPrivoxy();
        configFile = a.applicationDirPath() + "/config.ini";
#else
        QDir configDir = QDir::homePath() + "/.config/trojan-qt5";
        configFile = configDir.absolutePath() + "/config.ini";
        if (!configDir.exists()) {
            configDir.mkpath(configDir.absolutePath());
        }
#endif
    }

    ConfigHelper conf(configFile);

    MainWindow w(&conf);
    mainWindow = &w;

#if defined (Q_OS_MAC)
    setupDockClickHandler();
#endif

    if (conf.isOnlyOneInstance() && w.isInstanceRunning()) {
        return -1;
    }

    //we have to start QHttpServer in main otherwise it will not listen
    QHttpServer server;
#if defined(Q_OS_WIN)
    server.route("/<arg>", [](const QUrl &url) {
        QDir dir = QApplication::applicationDirPath() + "/pac";
        QHttpServerResponse response = QHttpServerResponse::fromFile(dir.path() + QString("/%1").arg(url.path()));
        response.addHeader("Server", "Trojan-Qt5");
        response.setHeader("Content-Type", "application/x-ns-proxy-autoconfig");
        response.addHeader("Connection", "Close");
        return response;
    });
#else
    server.route("/<arg>", [](const QUrl &url) {
        QDir configDir = QDir::homePath() + "/.config/trojan-qt5/pac";
        QHttpServerResponse response = QHttpServerResponse::fromFile(configDir.path() + QString("/%1").arg(url.path()));
        response.addHeader("Server", "Trojan-Qt5");
        response.setHeader("Content-Type", "application/x-ns-proxy-autoconfig");
        response.addHeader("Connection", "Close");
        return response;
    });
#endif
    server.listen(QHostAddress(conf.isEnableIpv6Support() ? (conf.isShareOverLan() ? "::" : "::1") : (conf.isShareOverLan() ? "0.0.0.0" : "127.0.0.1")), conf.getPACPort());

    //start all servers which were configured to start at startup
    w.startAutoStartConnections();

    if (!conf.isHideWindowOnStartup()) {
        w.show();
    }

#if defined (Q_OS_MAC)
    PFMoveToApplicationsFolderIfNecessary();
#endif

    return a.exec();
}
