#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>
#include <QLocale>
#include <QMessageBox>
#include <QDebug>
#include <QDir>
#include <QCommandLineParser>
#include <QHttpServer>
#include <signal.h>
#include "mainwindow.h"
#include "confighelper.h"
#include "resourcehelper.h"

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
    trojanqt5t->load(QLocale::system(), "trojan-qt5", "_", ":/i18n");
    a.installTranslator(trojanqt5t);
}

int main(int argc, char *argv[])
{

    qRegisterMetaTypeStreamOperators<TQProfile>("TQProfile");

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

    if (conf.isOnlyOneInstance() && w.isInstanceRunning()) {
        return -1;
    }

    /** We have to start QHttpServer in main otherwise it will not listen. */
    QHttpServer server;
#if defined(Q_OS_WIN)
    server.route("/<arg>", [](const QUrl &url) {
        QString dir = QApplication::applicationDirPath() + "/pac";
        return QHttpServerResponse::fromFile(dir.path() + QString("/%1").arg(url.path()));
    });
#else
    server.route("/<arg>", [](const QUrl &url) {
        QDir configDir = QDir::homePath() + "/.config/trojan-qt5/pac";
        return QHttpServerResponse::fromFile(configDir.path() + QString("/%1").arg(url.path()));
    });
#endif
    server.listen(QHostAddress::Any, 8070);

    if (!conf.isHideWindowOnStartup()) {
        w.show();
    }

    return a.exec();
}
