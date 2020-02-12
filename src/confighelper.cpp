#include "confighelper.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>

ConfigHelper::ConfigHelper(const QString &configuration, QObject *parent) :
    QObject(parent),
    configFile(configuration)
{
    settings = new QSettings(configFile, QSettings::IniFormat, this);
    readGeneralSettings();
}

const QString ConfigHelper::profilePrefix = "Profile";

void ConfigHelper::save(const ConnectionTableModel &model)
{
    int size = model.rowCount();
    settings->beginWriteArray(profilePrefix);
    for (int i = 0; i < size; ++i) {
        settings->setArrayIndex(i);
        Connection *con = model.getItem(i)->getConnection();
        QVariant value = QVariant::fromValue<TQProfile>(con->getProfile());
        settings->setValue("TQProfile", value);
    }
    settings->endArray();

    settings->setValue("ToolbarStyle", QVariant(toolbarStyle));
    settings->setValue("HideWindowOnStartup", QVariant(hideWindowOnStartup));
    settings->setValue("StartAtLogin", QVariant(startAtLogin));
    settings->setValue("OnlyOneInstance", QVariant(onlyOneInstace));
    settings->setValue("ShowToolbar", QVariant(showToolbar));
    settings->setValue("ShowFilterBar", QVariant(showFilterBar));
    settings->setValue("NativeMenuBar", QVariant(nativeMenuBar));
    settings->setValue("ConfigVersion", QVariant(2.6));
}

void ConfigHelper::importGuiConfigJson(ConnectionTableModel *model, const QString &file)
{
    QFile JSONFile(file);
    JSONFile.open(QIODevice::ReadOnly | QIODevice::Text);
    if (!JSONFile.isOpen()) {
        qCritical() << "Error: cannot open " << file;
        return;
    }
    if(!JSONFile.isReadable()) {
        qCritical() << "Error: cannot read " << file;
        return;
    }

    QJsonParseError pe;
    QJsonDocument JSONDoc = QJsonDocument::fromJson(JSONFile.readAll(), &pe);
    JSONFile.close();
    if (pe.error != QJsonParseError::NoError) {
        qCritical() << pe.errorString();
    }
    if (JSONDoc.isEmpty()) {
        qCritical() << "JSON Document" << file << "is empty!";
        return;
    }
    QJsonObject JSONObj = JSONDoc.object();
    QJsonArray CONFArray = JSONObj["configs"].toArray();
    if (CONFArray.isEmpty()) {
        qWarning() << "configs in " << file << " is empty.";
        return;
    }

    for (QJsonArray::iterator it = CONFArray.begin(); it != CONFArray.end(); ++it) {
        QJsonObject json = (*it).toObject();
        TQProfile p;
        if (!json["server_port"].isString()) {
            /*
             * shadowsocks-csharp uses integers to store ports directly.
             */
            p.name = json["remarks"].toString();
            p.serverPort = json["server_port"].toInt();
            //shadowsocks-csharp has only global local port (all profiles use the same port)
            p.localPort = JSONObj["localPort"].toInt();
            if (JSONObj["shareOverLan"].toBool()) {
                /*
                 * it can only configure share over LAN or not (also a global value)
                 * which is basically 0.0.0.0 or 127.0.0.1 (which is the default)
                 */
                p.localAddress = QString("0.0.0.0");
            }
        } else {
            /*
             * Otherwise, the gui-config is from legacy shadowsocks-qt5 (v0.x)
             */
            p.name = json["profile"].toString();
            p.serverPort = json["server_port"].toString().toUShort();
            p.localAddress = json["local_address"].toString();
            p.localPort = json["local_port"].toString().toUShort();
        }
        p.serverAddress = json["server"].toString();
        p.password = json["password"].toString();
        Connection *con = new Connection(p, this);
        model->appendConnection(con);
    }
}

void ConfigHelper::exportGuiConfigJson(const ConnectionTableModel &model, const QString &file)
{
    QJsonArray confArray;
    int size = model.rowCount();
    for (int i = 0; i < size; ++i) {
        Connection *con = model.getItem(i)->getConnection();
        QJsonObject json;
        json["remarks"] = QJsonValue(con->profile.name);
        json["server"] = QJsonValue(con->profile.serverAddress);
        json["server_port"] = QJsonValue(con->profile.serverPort);
        json["verify"] = QJsonValue(con->profile.verifyCertificate);
        json["password"] = QJsonValue(con->profile.password);
        json["tcp_fast_open"] = QJsonValue(con->profile.tcpFastOpen);
        confArray.append(QJsonValue(json));
    }

    QJsonObject JSONObj;
    JSONObj["configs"] = QJsonValue(confArray);
    JSONObj["localPort"] = QJsonValue(1080);
    JSONObj["shareOverLan"] = QJsonValue(false);

    QJsonDocument JSONDoc(JSONObj);

    QFile JSONFile(file);
    JSONFile.open(QIODevice::WriteOnly | QIODevice::Text);
    if (!JSONFile.isOpen()) {
        qCritical() << "Error: cannot open " << file;
        return;
    }
    if(!JSONFile.isWritable()) {
        qCritical() << "Error: cannot write into " << file;
        return;
    }

    JSONFile.write(JSONDoc.toJson());
    JSONFile.close();
}

Connection* ConfigHelper::configJsonToConnection(const QString &file)
{
    QFile JSONFile(file);
    JSONFile.open(QIODevice::ReadOnly | QIODevice::Text);
    if (!JSONFile.isOpen()) {
        qCritical() << "Error: cannot open " << file;
    }
    if(!JSONFile.isReadable()) {
        qCritical() << "Error: cannot read " << file;
    }

    QJsonParseError pe;
    QJsonDocument JSONDoc = QJsonDocument::fromJson(JSONFile.readAll(), &pe);
    JSONFile.close();
    if (pe.error != QJsonParseError::NoError) {
        qCritical() << pe.errorString();
    }
    if (JSONDoc.isEmpty()) {
        qCritical() << "JSON Document" << file << "is empty!";
        return nullptr;
    }
    QJsonObject configObj = JSONDoc.object();
    TQProfile p;
    p.serverAddress = configObj["server"].toString();
    p.serverPort = configObj["server_port"].toInt();
    p.localAddress = configObj["local_address"].toString();
    p.localPort = configObj["local_port"].toInt();
    p.password = configObj["password"].toString();
    Connection *con = new Connection(p, this);
    return con;
}

void ConfigHelper::connectionToJson(TQProfile &profile)
{
    QJsonObject configObj;
    configObj["run_type"] = "client";
    configObj["local_addr"] = profile.localAddress;
    configObj["local_port"] = profile.localPort;
    configObj["remote_addr"] = profile.serverAddress;
    configObj["remote_port"] = profile.serverPort;
    QJsonArray passwordArray;
    passwordArray.append(profile.password);
    configObj["password"] = QJsonValue(passwordArray);
    configObj["log_level"] = "1";
    QJsonObject ssl;
    ssl["verify"] = profile.verifyCertificate;
    ssl["verify_hostname"] = true;
    ssl["cert"] = "";
    ssl["cipher"] = "ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES128-GCM-SHA256:ECDHE-ECDSA-CHACHA20-POLY1305:ECDHE-RSA-CHACHA20-POLY1305:ECDHE-ECDSA-AES256-GCM-SHA384:ECDHE-RSA-AES256-GCM-SHA384:ECDHE-ECDSA-AES256-SHA:ECDHE-ECDSA-AES128-SHA:ECDHE-RSA-AES128-SHA:ECDHE-RSA-AES256-SHA:DHE-RSA-AES128-SHA:DHE-RSA-AES256-SHA:AES128-SHA:AES256-SHA:DES-CBC3-SHA";
    ssl["cipher_tls13"] = "TLS_AES_128_GCM_SHA256:TLS_CHACHA20_POLY1305_SHA256:TLS_AES_256_GCM_SHA384";
    ssl["sni"] = "";
    QJsonArray alpnArray;
    alpnArray.append("h2");
    alpnArray.append("http/1.1");
    ssl["alpn"] = QJsonValue(alpnArray);
    ssl["reuse_session"] = true;
    ssl["session_ticket"] = false;
    ssl["curves"] = "";
    configObj["ssl"] = QJsonValue(ssl);
    QJsonObject tcp;
    tcp["no_delay"] = true;
    tcp["keep_alive"] = true;
    tcp["reuse_port"] = false;
    tcp["fast_open"] = profile.tcpFastOpen;
    tcp["fast_open_qlen"] = 20;
    configObj["tcp"] = QJsonValue(tcp);

    QJsonDocument JSONDoc(configObj);

#ifdef Q_OS_WIN
        QString file = QCoreApplication::applicationDirPath() + "/config.json";
#else
        QDir configDir = QDir::homePath() + "/.config/trojan-qt5";
        QString file = configDir.absolutePath() + "/config.json";
#endif

    QFile JSONFile(file);
    JSONFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
    if (!JSONFile.isOpen()) {
        qCritical() << "Error: cannot open " << file;
        return;
    }
    if(!JSONFile.isWritable()) {
        qCritical() << "Error: cannot write into " << file;
        return;
    }

    JSONFile.write(JSONDoc.toJson());
    JSONFile.close();

}

void ConfigHelper::generatePrivoxyConf(TQProfile &profile)
{
    QString filecontent = QString("listen-address [%1]:%2\n"
                                  "toggle 0\n"
                                  "show-on-task-bar 0\n"
                                  "activity-animation 0\n"
                                  "forward-socks5 / [%3]:%4 .\n"
                                  "hide-console\n")
                                  .arg(profile.localAddress)
                                  .arg(QString::number(profile.localHttpPort))
                                  .arg(profile.localAddress)
                                  .arg(QString::number(profile.localPort));
#ifdef Q_OS_WIN
        QString file = QCoreApplication::applicationDirPath() + "/privoxy/privoxy.conf";
#else
        QDir configDir = QDir::homePath() + "/.config/trojan-qt5";
        QString file = configDir.absolutePath() + "/privoxy.conf";
#endif
    QFile privoxyConf(file);
    privoxyConf.open(QIODevice::WriteOnly | QIODevice::Text |QIODevice::Truncate);
    if (!privoxyConf.isOpen()) {
        qCritical() << "Error: cannot open " << file;
        return;
    }
    if(!privoxyConf.isWritable()) {
        qCritical() << "Error: cannot write into " << file;
        return;
    }
    privoxyConf.write(filecontent.toUtf8());
    privoxyConf.close();
}

int ConfigHelper::getToolbarStyle() const
{
    return toolbarStyle;
}

bool ConfigHelper::isHideWindowOnStartup() const
{
    return hideWindowOnStartup;
}

bool ConfigHelper::isStartAtLogin() const
{
    return startAtLogin;
}

bool ConfigHelper::isOnlyOneInstance() const
{
    return onlyOneInstace;
}

bool ConfigHelper::isShowToolbar() const
{
    return showToolbar;
}

bool ConfigHelper::isShowFilterBar() const
{
    return showFilterBar;
}

bool ConfigHelper::isNativeMenuBar() const
{
    return nativeMenuBar;
}

void ConfigHelper::setGeneralSettings(int ts, bool hide, bool sal, bool oneInstance, bool nativeMB)
{
    if (toolbarStyle != ts) {
        emit toolbarStyleChanged(static_cast<Qt::ToolButtonStyle>(ts));
    }
    toolbarStyle = ts;
    hideWindowOnStartup = hide;
    startAtLogin = sal;
    onlyOneInstace = oneInstance;
    nativeMenuBar = nativeMB;
}

void ConfigHelper::setShowToolbar(bool show)
{
    showToolbar = show;
}

void ConfigHelper::setShowFilterBar(bool show)
{
    showFilterBar = show;
}

void ConfigHelper::read(ConnectionTableModel *model)
{
    qreal configVer = settings->value("ConfigVersion", QVariant(2.4)).toReal();
    int size = settings->beginReadArray(profilePrefix);
    for (int i = 0; i < size; ++i) {
        settings->setArrayIndex(i);
        QVariant value = settings->value("TQProfile");
        TQProfile profile = value.value<TQProfile>();
        checkProfileDataUsageReset(profile);
        Connection *con = new Connection(profile, this);
        model->appendConnection(con);
    }
    settings->endArray();
    readGeneralSettings();
}

void ConfigHelper::readGeneralSettings()
{
    toolbarStyle = settings->value("ToolbarStyle", QVariant(4)).toInt();
    startAtLogin = settings->value("StartAtLogin").toBool();
    hideWindowOnStartup = settings->value("HideWindowOnStartup").toBool();
    onlyOneInstace = settings->value("OnlyOneInstance", QVariant(true)).toBool();
    showToolbar = settings->value("ShowToolbar", QVariant(true)).toBool();
    showFilterBar = settings->value("ShowFilterBar", QVariant(true)).toBool();
    nativeMenuBar = settings->value("NativeMenuBar", QVariant(false)).toBool();
}

void ConfigHelper::checkProfileDataUsageReset(TQProfile &profile)
{
    QDate currentDate = QDate::currentDate();
    if (profile.nextResetDate.isNull()){//invalid if the config.ini is old
        //the default reset day is 1 of every month
        profile.nextResetDate = QDate(currentDate.year(), currentDate.month(), 1);
        qDebug() << "config.ini upgraded from old version";
        profile.totalUsage += profile.currentUsage;//we used to use sent and received
    }

    if (profile.nextResetDate < currentDate) {//not <= because that'd casue multiple reset on this day
        profile.currentUsage = 0;
        while (profile.nextResetDate <= currentDate) {
            profile.nextResetDate = profile.nextResetDate.addMonths(1);
        }
    }
}

void ConfigHelper::startAllAutoStart(const ConnectionTableModel& model)
{
    int size = model.rowCount();
    for (int i = 0; i < size; ++i) {
        Connection *con = model.getItem(i)->getConnection();
        if (con->profile.autoStart) {
            //con->start();
        }
    }
}

void ConfigHelper::setStartAtLogin()
{
    QString applicationName = "Trojan-Qt5";
    QString applicationFilePath = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
#if defined(Q_OS_WIN)
    QSettings settings("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
#elif defined(Q_OS_LINUX)
    QFile file(QDir::homePath() + "/.config/autostart/shadowsocks-qt5.desktop");
    QString fileContent(
            "[Desktop Entry]\n"
            "Name=%1\n"
            "Exec=%2\n"
            "Type=Application\n"
            "Terminal=false\n"
            "X-GNOME-Autostart-enabled=true\n");
#elif defined(Q_OS_MAC)
    QFile file(QDir::homePath() + "/Library/LaunchAgents/org.shadowsocks.shadowsocks-qt5.launcher.plist");
    QString fileContent(
            "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n"
            "<plist version=\"1.0\">\n"
            "<dict>\n"
            "  <key>Label</key>\n"
            "  <string>org.shadowsocks.shadowsocks-qt5.launcher</string>\n"
            "  <key>LimitLoadToSessionType</key>\n"
            "  <string>Aqua</string>\n"
            "  <key>ProgramArguments</key>\n"
            "  <array>\n"
            "    <string>%2</string>\n"
            "  </array>\n"
            "  <key>RunAtLoad</key>\n"
            "  <true/>\n"
            "  <key>StandardErrorPath</key>\n"
            "  <string>/dev/null</string>\n"
            "  <key>StandardOutPath</key>\n"
            "  <string>/dev/null</string>\n"
            "</dict>\n"
            "</plist>\n");
#else
    QFile file;
    QString fileContent;
#endif

    if (this->isStartAtLogin()) {
        // Create start up item
    #if defined(Q_OS_WIN)
        settings.setValue(applicationName, applicationFilePath);
    #else
        fileContent.replace("%1", applicationName);
        fileContent.replace("%2", applicationFilePath);
        if ( file.open(QIODevice::WriteOnly) ) {
            file.write(fileContent.toUtf8());
            file.close();
        }
    #endif
    } else {
        // Delete start up item
        #if defined(Q_OS_WIN)
            settings.remove(applicationName);
        #else
            if ( file.exists() ) {
                file.remove();
            }
        #endif
    }
}

QByteArray ConfigHelper::getMainWindowGeometry() const
{
    return settings->value("MainWindowGeometry").toByteArray();
}

QByteArray ConfigHelper::getMainWindowState() const
{
    return settings->value("MainWindowState").toByteArray();
}

QByteArray ConfigHelper::getTableGeometry() const
{
    return settings->value("MainTableGeometry").toByteArray();
}

QByteArray ConfigHelper::getTableState() const
{
    return settings->value("MainTableState").toByteArray();
}

void ConfigHelper::setMainWindowGeometry(const QByteArray &geometry)
{
    settings->setValue("MainWindowGeometry", QVariant(geometry));
}

void ConfigHelper::setMainWindowState(const QByteArray &state)
{
    settings->setValue("MainWindowState", QVariant(state));
}

void ConfigHelper::setTableGeometry(const QByteArray &geometry)
{
    settings->setValue("MainTableGeometry", QVariant(geometry));
}

void ConfigHelper::setTableState(const QByteArray &state)
{
    settings->setValue("MainTableState", QVariant(state));
}
