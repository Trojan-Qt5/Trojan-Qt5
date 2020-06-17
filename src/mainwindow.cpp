#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "connection.h"
#include "socks5editdialog.h"
#include "httpeditdialog.h"
#include "sseditdialog.h"
#include "ssreditdialog.h"
#include "vmesseditdialog.h"
#include "trojaneditdialog.h"
#include "snelleditdialog.h"
#include "urihelper.h"
#include "uriinputdialog.h"
#include "userrulesdialog.h"
#include "sharedialog.h"
#include "settingsdialog.h"
#include "qrcodecapturer.h"
#include "generalvalidator.h"
#include "midman.h"
#include "aboutdialog.h"
#include "logger.h"
#include "utils.h"
#include "QtAwesome.h"
#include "statusbar.h"

#include <QClipboard>
#include <QScrollBar>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <QLocalSocket>
#include <QStandardPaths>
#include <QUrl>
#include <QResource>

MainWindow::MainWindow(ConfigHelper *confHelper, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    configHelper(confHelper),
    instanceRunning(false)
{
    Q_ASSERT(configHelper);

    //initialize Trojan Logger
    initLog();

    initSingleInstance();

    //initalize Sparkle Updater
    initSparkle();

    ui->setupUi(this);

    //setup Settings menu
#ifndef Q_OS_DARWIN
    ui->menuSettings->addAction(ui->toolBar->toggleViewAction());
#endif

    //initialisation
    model = new ConnectionTableModel(this);
    configHelper->read(model);
    proxyModel = new ConnectionSortFilterProxyModel(this);
    proxyModel->setSourceModel(model);
    proxyModel->setSortRole(Qt::EditRole);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    proxyModel->setFilterKeyColumn(-1);//read from all columns
    ui->connectionView->setModel(proxyModel);
    ui->connectionView->setFocusPolicy(Qt::NoFocus);
    ui->connectionView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->connectionView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    ui->toolBar->setToolButtonStyle(static_cast<Qt::ToolButtonStyle>
                                    (configHelper->getGeneralSettings()["toolbarStyle"].toInt()));

    setupActionIcon();

    // setup statusbar
    m_statusBar = new StatusBar;
    setStatusBar(m_statusBar);
    QList<int> ports;
    ports.append(configHelper->getInboundSettings()["socks5LocalPort"].toInt());
    ports.append(configHelper->getInboundSettings()["httpLocalPort"].toInt());
    ports.append(configHelper->getInboundSettings()["pacLocalPort"].toInt());
    QList<QString> stats;
    stats.append("0 B");
    stats.append("0 B");
    stats.append("0 B");
    stats.append("0 B");
    m_statusBar->refresh(configHelper->getInboundSettings()["enableIpv6Support"].toBool() ? (configHelper->getInboundSettings()["shareOverLan"].toBool() ? "::" : "::1") : (configHelper->getInboundSettings()["shareOverLan"].toBool() ? "0.0.0.0" : "127.0.0.1"),
    ports,
    stats);

    sbMgr = new SubscribeManager(this, configHelper);
    notifier = new StatusNotifier(this, configHelper, sbMgr, this);

    connect(configHelper, &ConfigHelper::toolbarStyleChanged,
            ui->toolBar, &QToolBar::setToolButtonStyle);
    connect(model, &ConnectionTableModel::message,
            notifier, &StatusNotifier::showNotification);
    connect(model, &ConnectionTableModel::rowStatusChanged,
            this, &MainWindow::onConnectionStatusChanged);
    connect(model, &ConnectionTableModel::changeIcon,
            notifier, &StatusNotifier::changeIcon);
    connect(ui->actionSaveManually, &QAction::triggered,
            this, &MainWindow::onSaveManually);
    connect(ui->actionTestAllLatency, &QAction::triggered,
            model, &ConnectionTableModel::testAllLatency);
    connect(notifier, &StatusNotifier::toggleConnection,
            this, &MainWindow::onToggleConnection);
    connect(sbMgr, &SubscribeManager::addUri, this,
            &MainWindow::onAddURIFromSubscribe);

    //some UI changes accoding to config
    ui->toolBar->setVisible(configHelper->isShowToolbar());
    ui->actionShowFilterBar->setChecked(configHelper->isShowFilterBar());
    ui->menuBar->setNativeMenuBar(configHelper->getGeneralSettings()["nativeMenuBar"].toBool());

    ui->filterLineEdit->setObjectName("filterLineEdit");
    ui->toolBar->setFixedHeight(92);

    // set the minimum size
    this->setMinimumSize(825, 562);

    /** Reset the font. */
#if !defined(Q_OS_MAC)
    QFont font;
    font.setPixelSize(14);
    font.setBold(true);
    ui->menuBar->setFont(font);
#endif

    //Move to the center of the screen
    this->move(QApplication::desktop()->screen()->rect().center() -
               this->rect().center());

    //UI signals
    connect(ui->actionImportGUIJson, &QAction::triggered,
            this, &MainWindow::onImportGuiJson);
    connect(ui->actionImportConfigYaml, &QAction::triggered,
            this, &MainWindow::onImportConfigYaml);
    connect(ui->actionExportGUIJson, &QAction::triggered,
            this, &MainWindow::onExportGuiJson);
    connect(ui->actionExportShadowrocketJson, &QAction::triggered,
            this, &MainWindow::onExportShadowrocketJson);
    connect(ui->actionExportSubscribe, &QAction::triggered,
            this, &MainWindow::onExportTrojanSubscribe);
    connect(ui->actionQuit, &QAction::triggered, qApp, &QApplication::quit);
    connect(ui->actionAdd_SOCKS5_Manually, &QAction::triggered,
            this, [this]() { onAddManually("socks5"); });
    connect(ui->actionAdd_HTTP_Manually, &QAction::triggered,
            this, [this]() { onAddManually("http"); });
    connect(ui->actionAdd_SS_Manually, &QAction::triggered,
            this, [this]() { onAddManually("ss"); });
    connect(ui->actionAdd_SSR_Manually, &QAction::triggered,
            this, [this]() { onAddManually("ssr"); });
    connect(ui->actionAdd_Vmess_Manually, &QAction::triggered,
            this, [this]() { onAddManually("vmess"); });
    connect(ui->actionAdd_Trojan_Manually, &QAction::triggered,
            this, [this]() { onAddManually("trojan"); });
    connect(ui->actionAdd_Snell_Manually, &QAction::triggered,
            this, [this]() { onAddManually("snell"); });
    connect(ui->actionQRCode, &QAction::triggered,
            this, &MainWindow::onAddScreenQRCode);
    connect(ui->actionScanQRCodeCapturer, &QAction::triggered,
            this, &MainWindow::onAddScreenQRCodeCapturer);
    connect(ui->actionQRCodeFromFile, &QAction::triggered,
            this, &MainWindow::onAddQRCodeFile);
    connect(ui->actionURI, &QAction::triggered,
            this, &MainWindow::onAddFromURI);
    connect(ui->actionPasteBoardURI, &QAction::triggered,
            this, &MainWindow::onAddFromPasteBoardURI);
    connect(ui->actionFromConfigJson, &QAction::triggered,
            this, &MainWindow::onAddFromConfigJSON);
    connect(ui->actionFromShadowrocketJson, &QAction::triggered,
            this, &MainWindow::onAddFromShadowrocketJSON);
    connect(ui->actionDelete, &QAction::triggered, this, &MainWindow::onDelete);
    connect(ui->actionEdit, &QAction::triggered, this, &MainWindow::onEdit);
    connect(ui->actionShare, &QAction::triggered, this, &MainWindow::onShare);
    connect(ui->actionConnect, &QAction::triggered,
            this, &MainWindow::onConnect);
    connect(ui->actionForceConnect, &QAction::triggered,
            this, &MainWindow::onForceConnect);
    connect(ui->actionDisconnect, &QAction::triggered,
            this, &MainWindow::onDisconnect);
    connect(ui->actionTestLatency, &QAction::triggered,
            this, &MainWindow::onLatencyTest);
    connect(ui->actionClearTraffic, &QAction::triggered,
                this, &MainWindow::onClearTrafficStats);
    connect(ui->actionMoveUp, &QAction::triggered, this, &MainWindow::onMoveUp);
    connect(ui->actionMoveDown, &QAction::triggered,
            this, &MainWindow::onMoveDown);
    connect(ui->actionGeneralSettings, &QAction::triggered,
            this, &MainWindow::onGeneralSettings);
    connect(ui->actionUserRuleSerttings, &QAction::triggered,
            this, &MainWindow::onUserRuleSettings);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::onAbout);
    connect(ui->actionAboutQt, &QAction::triggered,
            qApp, &QApplication::aboutQt);
    connect(ui->actionCheckUpdate, &QAction::triggered,
            this, &MainWindow::onCheckUpdate);
    connect(ui->actionGuiLog, &QAction::triggered,
            this, &MainWindow::onGuiLog);
    connect(ui->actionCoreLog, &QAction::triggered,
            this, &MainWindow::onCoreLog);
    connect(ui->actionReportBug, &QAction::triggered,
            this, &MainWindow::onReportBug);
    connect(ui->actionShowFilterBar, &QAction::toggled,
            configHelper, &ConfigHelper::setShowFilterBar);
    connect(ui->actionShowFilterBar, &QAction::toggled,
            this, &MainWindow::onFilterToggled);
    connect(ui->toolBar, &QToolBar::visibilityChanged,
            configHelper, &ConfigHelper::setShowToolbar);
    connect(ui->filterLineEdit, &QLineEdit::textChanged,
            this, &MainWindow::onFilterTextChanged);

    connect(ui->connectionView, &QTableView::clicked,
            this, static_cast<void (MainWindow::*)(const QModelIndex&)>
            (&MainWindow::checkCurrentIndex));
    connect(ui->connectionView, &QTableView::activated,
            this, static_cast<void (MainWindow::*)(const QModelIndex&)>
            (&MainWindow::checkCurrentIndex));
    connect(ui->connectionView, &QTableView::doubleClicked,
            this, &MainWindow::onConnect);

    /* set custom context menu */
    ui->connectionView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->connectionView, &QTableView::customContextMenuRequested,
            this, &MainWindow::onCustomContextMenuRequested);

    checkCurrentIndex();

    // Restore mainWindow's geometry and state
    restoreGeometry(configHelper->getMainWindowGeometry());
    restoreState(configHelper->getMainWindowState());
    ui->connectionView->horizontalHeader()->restoreGeometry(configHelper->getTableGeometry());
    ui->connectionView->horizontalHeader()->restoreState(configHelper->getTableState());

    if (configHelper->isAutoUpdateSubscribes())
        sbMgr->updateAllSubscribesWithThread();
}

MainWindow::~MainWindow()
{
    configHelper->save(*model);
    configHelper->setTableGeometry(ui->connectionView->horizontalHeader()->saveGeometry());
    configHelper->setTableState(ui->connectionView->horizontalHeader()->saveState());
    configHelper->setMainWindowGeometry(saveGeometry());
    configHelper->setMainWindowState(saveState());

#if defined (Q_OS_WIN)
    win_sparkle_cleanup();
#endif

    // delete ui after everything in case it's deleted while still needed for
    // the functions written above
    delete ui;
}

const QUrl MainWindow::issueUrl =
        QUrl("https://t.me/TrojanQt5");

void MainWindow::startAutoStartConnections()
{
    configHelper->startAllAutoStart(*model);
}

QList<TQProfile> MainWindow::getAllServers()
{
    return model->getAllServers();
}

TQProfile MainWindow::getConnectedServer()
{
    return model->getConnectedServer();
}

void MainWindow::onToggleConnection(bool status)
{
    if (!status) {
        model->disconnectConnections();
    } else {
        int row = proxyModel->mapToSource(ui->connectionView->currentIndex()).row();
        if (row < 0)
            return;
        Connection *con = model->getItem(row)->getConnection();
        if (con->isValid()) {
            model->disconnectConnections();
            //configHelper->isEnableServerLoadBalance() ? configHelper->generateHaproxyConf(*model) : void();
            con->start();
            connect(con, &Connection::dataTrafficAvailable, this, &MainWindow::onStatusAvailable);
        }
    }
}

void MainWindow::onHandleDataFromUrlScheme(const QString &data)
{
    if (data.startsWith("ss://") ||
        data.startsWith("ssr://") ||
        data.startsWith("vmess://") ||
        data.startsWith("trojan://") ||
        data.startsWith("snell://")) {
        if (GeneralValidator::validateAll(data)) {
            Connection *newCon = new Connection(data, this);
            model->appendConnection(newCon);
            configHelper->save(*model);
         }
    } else if (data.startsWith("trojan-qt5://")) {
        if (data.split("add-connection?url=").length() > 1) {
            if (GeneralValidator::validateAll(data.split("add-subscribe?url=")[1])) {
                Connection *newCon = new Connection(data.split("add-subscribe?url=")[1], this);
                model->appendConnection(newCon);
                configHelper->save(*model);
             }
        } else if (data.split("add-subscribe?url=").length() > 1) {
            QString splitData = data.split("add-subscribe?url=")[1];
            QString url = QUrl::fromPercentEncoding(splitData.toUtf8().data()).toUtf8().data();
            QList<TQSubscribe> subscribes = configHelper->readSubscribes();
            TQSubscribe subscribe;
            subscribe.url = url;
            subscribes.append(subscribe);
            configHelper->saveSubscribes(subscribes);
            sbMgr->setUseProxy(false);
            sbMgr->updateAllSubscribesWithThread();
        }
    } else if (data.startsWith("felix://")) {
        QMessageBox::information(this, "Aku seneng Felix Wang", "I love you Felix Wang\nCoel 2019-2020");
    }
}

void MainWindow::onAddServerFromSystemTray(QString type)
{
    if (type == "manually")
        onAddManually("trojan");
    else if (type == "qrcode")
        onAddScreenQRCode();
    else if (type == "pasteboard")
        onAddFromPasteBoardURI();
}

void MainWindow::onAddURIFromSubscribe(TQProfile profile)
{
    Connection *newCon = new Connection(profile, this);

    bool dupResult = model->isDuplicated(newCon);
    bool existResult = model->isExisted(newCon);

    if (dupResult)
        return;

    if (existResult) {
        model->replace(newCon);
        return;
     }

    model->appendConnection(newCon);

    configHelper->save(*model);
}

void MainWindow::onImportGuiJson()
{
    QString file = QFileDialog::getOpenFileName(
                   this,
                   tr("Import Connections from gui-config.json"),
                   QString(),
                   "GUI Configuration (gui-config.json)");
    if (!file.isNull()) {
        configHelper->importGuiConfigJson(model, file);
        configHelper->save(*model);
    }
}

void MainWindow::onExportGuiJson()
{
    QString file = QFileDialog::getSaveFileName(
                   this,
                   tr("Export Connections as gui-config.json"),
                   QString("gui-config.json"),
                   "GUI Configuration (gui-config.json)");
    if (!file.isNull()) {
        configHelper->exportGuiConfigJson(*model, file);
    }
}

void MainWindow::onImportConfigYaml()
{
    QString file = QFileDialog::getOpenFileName(
                   this,
                   tr("Import Connections from config.yaml"),
                   QString(),
                   "Clash Configuration (config.yaml)");
    if (!file.isNull()) {
        configHelper->importConfigYaml(model, file);
        configHelper->save(*model);
    }
}

void MainWindow::onExportShadowrocketJson()
{
    QString file = QFileDialog::getSaveFileName(
                   this,
                   tr("Export Connections as shadowrocket.json"),
                   QString("shadowrocket.json"),
                   "Shadowrocket Configuration (shadowrocket.json)");
    if (!file.isNull()) {
        configHelper->exportShadowrocketJson(*model, file);
    }
}

void MainWindow::onExportTrojanSubscribe()
{
    QString file = QFileDialog::getSaveFileName(
                   this,
                   tr("Export Trojan Servers as subscribe.txt"),
                   QString("subscribe.txt"),
                   "Trojan Subscribe (subscribe.txt)");
    if (!file.isNull()) {
        configHelper->exportTrojanSubscribe(*model, file);
    }
}

void MainWindow::onSaveManually()
{
    configHelper->save(*model);
}

void MainWindow::onAddManually(QString type)
{
    Connection *newCon = new Connection;
    TQProfile p;
    p.type = type;
    newCon->setProfile(p);
    newProfile(newCon);
}

void MainWindow::onAddScreenQRCode()
{
    QString uri = QRCodeCapturer::scanEntireScreen();
    if (uri.isNull()) {
        QMessageBox::critical(
                    this,
                    tr("QR Code Not Found"),
                    tr("Can't find any QR code image that contains "
                       "valid URI on your screen(s)."));
        Logger::warning("Can't find any QR code image that contains valid URI on your screen");
    } else {
        Connection *newCon = new Connection(uri, this);
        newProfile(newCon);
    }
}

void MainWindow::onAddScreenQRCodeCapturer()
{
    QRCodeCapturer *capturer = new QRCodeCapturer(this);
    connect(capturer, &QRCodeCapturer::closed,
            capturer, &QRCodeCapturer::deleteLater);
    connect(capturer, &QRCodeCapturer::qrCodeFound,
            this, &MainWindow::onQRCodeCapturerResultFound,
            Qt::DirectConnection);
    capturer->show();
}

void MainWindow::onAddQRCodeFile()
{
    QString qrFile =
            QFileDialog::getOpenFileName(this,
                                         tr("Open QR Code Image File"),
                                         QString(),
                                         "Images (*.png *jpg *jpeg *xpm)");
    if (!qrFile.isNull()) {
        QImage img(qrFile);
        QString uri = URIHelper::decodeImage(img);
        if (uri.isNull()) {
            QMessageBox::critical(this,
                                  tr("QR Code Not Found"),
                                  tr("Can't find any QR code image that "
                                     "contains valid URI on your screen(s)."));
            Logger::warning("Can't find any QR code image that contains valid URI on your screen");
        } else {
            Connection *newCon = new Connection(uri, this);
            newProfile(newCon);
        }
    }
}

void MainWindow::onAddFromURI()
{
    URIInputDialog *inputDlg = new URIInputDialog(this);
    connect(inputDlg, &URIInputDialog::finished,
            inputDlg, &URIInputDialog::deleteLater);
    connect(inputDlg, &URIInputDialog::acceptedURI, [&](const QString &uri){
            Connection *newCon = new Connection(uri, this);
            newProfile(newCon);
    });
    inputDlg->exec();
}

void MainWindow::onAddFromPasteBoardURI()
{
    QClipboard *board = QApplication::clipboard();
    QString str = board->text();
    str = str.replace("\\r", "\r").replace("\\n", "\n").replace("\r\n", "\n");
    for (QString uri: str.split("\n")) {
        if (GeneralValidator::validateAll(uri)) {
            Connection *newCon = new Connection(uri, this);
            model->appendConnection(newCon);
            configHelper->save(*model);
         }
     }
}

void MainWindow::onAddFromConfigJSON()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Open config.json"),
                                                QString(), "JSON (*.json)");
    if (!file.isNull()) {
        Connection *con = configHelper->configJsonToConnection(file);
        if (con) {
            newProfile(con);
        }
    }
}

void MainWindow::onAddFromShadowrocketJSON()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Open shadowrocket.json"),
                                                QString(), "JSON (*.json)");
    if (!file.isNull()) {
        configHelper->importShadowrocketJson(model, file);
    }
}

void MainWindow::onDelete()
{
    for (int i = ui->connectionView->selectionModel()->selectedRows().size() - 1; i >= 0; i--)
        model->removeRow(proxyModel->mapToSource(ui->connectionView->selectionModel()->selectedRows()[i]).row());
    configHelper->save(*model);
    checkCurrentIndex();
}

void MainWindow::onEdit()
{
    editRow(proxyModel->mapToSource(ui->connectionView->currentIndex()).row());
}

void MainWindow::onShare()
{
    Connection *con = model->getItem(
                proxyModel->mapToSource(ui->connectionView->currentIndex()).
                row())->getConnection();

    QByteArray uri = con->getURI(con->getProfile().type);

    ShareDialog *shareDlg = new ShareDialog(uri, this);
    connect(shareDlg, &ShareDialog::finished,
            shareDlg, &ShareDialog::deleteLater);
    shareDlg->exec();
}

void MainWindow::onConnect()
{
    int row = proxyModel->mapToSource(ui->connectionView->currentIndex()).row();
    Connection *con = model->getItem(row)->getConnection();
    if (con->isValid()) {
        model->disconnectConnections();
        //configHelper->isEnableServerLoadBalance() ? configHelper->generateHaproxyConf(*model) : void();
        con->start();
        connect(con, &Connection::dataTrafficAvailable, this, &MainWindow::onStatusAvailable);
    } else {
        QMessageBox::critical(this, tr("Invalid"),
                              tr("The connection's profile is invalid!"));
        Logger::error(QString("Can't start server %1 because the profile is invalid").arg(con->getName()));
    }
}

void MainWindow::onForceConnect()
{
    int row = proxyModel->mapToSource(ui->connectionView->currentIndex()).row();
    Connection *con = model->getItem(row)->getConnection();
    if (con->isValid()) {
        model->disconnectConnections();
        //configHelper->isEnableServerLoadBalance() ? configHelper->generateHaproxyConf(*model) : void();
        con->start();
        connect(con, &Connection::dataTrafficAvailable, this, &MainWindow::onStatusAvailable);
    } else {
        QMessageBox::critical(this, tr("Invalid"),
                              tr("The connection's profile is invalid!"));
        Logger::error(QString("Can't start server %1 because the profile is invalid").arg(con->getName()));
    }
}

void MainWindow::onDisconnect()
{
    int row = proxyModel->mapToSource(ui->connectionView->currentIndex()).row();
    model->getItem(row)->getConnection()->stop();
}

void MainWindow::onToggleServerFromSystemTray(TQProfile profile)
{
    for (int i=0; i < ui->connectionView->model()->rowCount(); i++) {
        int row = proxyModel->mapToSource(ui->connectionView->model()->index(i,0)).row();
        TQProfile p = model->getItem(row)->getConnection()->getProfile();
        if (p.equals(profile)) {
            ui->connectionView->selectRow(i);
        }
    }
    checkCurrentIndex();
}

void MainWindow::onConnectionStatusChanged(const int row, const bool running)
{
    if (proxyModel->mapToSource(
                ui->connectionView->currentIndex()).row() == row) {
        ui->actionConnect->setEnabled(!running);
        ui->actionDisconnect->setEnabled(running);
        ui->actionDelete->setEnabled(!running);
    }
}

void MainWindow::onClearTrafficStats()
{
    for (int i = 0; i < ui->connectionView->selectionModel()->selectedRows().size(); i++)
        model->getItem(proxyModel->mapToSource(ui->connectionView->selectionModel()->selectedRows()[i]).row())->clearTraffic();
}

void MainWindow::onLatencyTest()
{
    for (int i = 0; i < ui->connectionView->selectionModel()->selectedRows().size(); i++)
        model->getItem(proxyModel->mapToSource(ui->connectionView->selectionModel()->selectedRows()[i]).row())->testLatency();
}

void MainWindow::onMoveUp()
{
    QModelIndex proxyIndex = ui->connectionView->currentIndex();
    int currentRow = proxyModel->mapToSource(proxyIndex).row();
    int targetRow = proxyModel->mapToSource(
                proxyModel->index(proxyIndex.row() - 1,
                                  proxyIndex.column(),
                                  proxyIndex.parent())
                                           ).row();
    model->move(currentRow, targetRow);
    checkCurrentIndex();
}

void MainWindow::onMoveDown()
{
    QModelIndex proxyIndex = ui->connectionView->currentIndex();
    int currentRow = proxyModel->mapToSource(proxyIndex).row();
    int targetRow = proxyModel->mapToSource(
                proxyModel->index(proxyIndex.row() + 1,
                                  proxyIndex.column(),
                                  proxyIndex.parent())
                                           ).row();
    model->move(currentRow, targetRow);
    checkCurrentIndex();
}

void MainWindow::onGeneralSettings()
{
    SettingsDialog *sDlg = new SettingsDialog(configHelper, this);
    connect(sDlg, &SettingsDialog::finished,
            sDlg, &SettingsDialog::deleteLater);
    if (sDlg->exec()) {
        configHelper->save(*model);
        configHelper->setStartAtLogin();
    }
}

void MainWindow::onUserRuleSettings()
{
    UserRulesDialog *userRuleDlg = new UserRulesDialog(this);
    connect(userRuleDlg, &UserRulesDialog::finished,
            userRuleDlg, &UserRulesDialog::deleteLater);
    userRuleDlg->exec();
}

void MainWindow::newProfile(Connection *newCon)
{
    QDialog *editDlg = new QDialog(this);
    if (newCon->getProfile().type == "socks5") {
        editDlg = new Socks5EditDialog(newCon, this);
        connect(editDlg, &Socks5EditDialog::finished, editDlg, &Socks5EditDialog::deleteLater);
    } else if (newCon->getProfile().type == "http") {
        editDlg = new HttpEditDialog(newCon, this);
        connect(editDlg, &HttpEditDialog::finished, editDlg, &HttpEditDialog::deleteLater);
    } else if (newCon->getProfile().type == "ss") {
        editDlg = new SSEditDialog(newCon, this);
        connect(editDlg, &SSEditDialog::finished, editDlg, &SSEditDialog::deleteLater);
    } else if (newCon->getProfile().type == "ssr") {
        editDlg = new SSREditDialog(newCon, this);
        connect(editDlg, &SSREditDialog::finished, editDlg, &SSREditDialog::deleteLater);
    } else if (newCon->getProfile().type == "vmess") {
        editDlg = new VmessEditDialog(newCon, this);
        connect(editDlg, &VmessEditDialog::finished, editDlg, &VmessEditDialog::deleteLater);
    } else if (newCon->getProfile().type == "trojan") {
        editDlg = new TrojanEditDialog(newCon, this);
        connect(editDlg, &TrojanEditDialog::finished, editDlg, &TrojanEditDialog::deleteLater);
    } else if (newCon->getProfile().type == "snell") {
        editDlg = new SnellEditDialog(newCon, this);
        connect(editDlg, &SnellEditDialog::finished, editDlg, &SnellEditDialog::deleteLater);
    }

    if (editDlg->exec()) {//accepted
        model->appendConnection(newCon);
        configHelper->save(*model);
    } else {
        newCon->deleteLater();
    }
}

void MainWindow::editRow(int row)
{
    Connection *con = model->getItem(row)->getConnection();

    QDialog *editDlg = new QDialog(this);
    if (con->getProfile().type == "socks5") {
        editDlg = new Socks5EditDialog(con, this);
        connect(editDlg, &Socks5EditDialog::finished, editDlg, &Socks5EditDialog::deleteLater);
    } else if (con->getProfile().type == "http") {
        editDlg = new HttpEditDialog(con, this);
        connect(editDlg, &HttpEditDialog::finished, editDlg, &HttpEditDialog::deleteLater);
    } else if (con->getProfile().type == "ss") {
        editDlg = new SSEditDialog(con, this);
        connect(editDlg, &SSEditDialog::finished, editDlg, &SSEditDialog::deleteLater);
    } else if (con->getProfile().type == "ssr") {
        editDlg = new SSREditDialog(con, this);
        connect(editDlg, &SSREditDialog::finished, editDlg, &SSREditDialog::deleteLater);
    } else if (con->getProfile().type == "vmess") {
        editDlg = new VmessEditDialog(con, this);
        connect(editDlg, &VmessEditDialog::finished, editDlg, &VmessEditDialog::deleteLater);
    } else if (con->getProfile().type == "trojan") {
        editDlg = new TrojanEditDialog(con, this);
        connect(editDlg, &TrojanEditDialog::finished, editDlg, &TrojanEditDialog::deleteLater);
    } else if (con->getProfile().type == "snell") {
        editDlg = new SnellEditDialog(con, this);
        connect(editDlg, &SnellEditDialog::finished, editDlg, &SnellEditDialog::deleteLater);
    }

    if (editDlg->exec()) {
        configHelper->save(*model);
    }
}

void MainWindow::checkCurrentIndex()
{
    checkCurrentIndex(ui->connectionView->currentIndex());
}

void MainWindow::checkCurrentIndex(const QModelIndex &_index)
{
    QModelIndex index = proxyModel->mapToSource(_index);
    const bool valid = index.isValid();
    ui->actionTestLatency->setEnabled(valid);
    ui->actionEdit->setEnabled(valid);
    ui->actionDelete->setEnabled(valid);
    ui->actionShare->setEnabled(valid);
    ui->actionMoveUp->setEnabled(valid ? _index.row() > 0 : false);
    ui->actionMoveDown->setEnabled(valid ?
                                   _index.row() < model->rowCount() - 1 :
                                   false);

    if (valid) {
        const bool &running =
                model->getItem(index.row())->getConnection()->isRunning();
        ui->actionConnect->setEnabled(!running);
        ui->actionForceConnect->setEnabled(!running);
        ui->actionDisconnect->setEnabled(running);
        ui->actionDelete->setEnabled(!running);
    } else {
        ui->actionConnect->setEnabled(false);
        ui->actionForceConnect->setEnabled(false);
        ui->actionDisconnect->setEnabled(false);
    }
}

void MainWindow::onAbout()
{
    AboutDialog *aboutDialog = new AboutDialog(this);
    aboutDialog->exec();
}

void MainWindow::onGuiLog()
{
    QString guiLog = Utils::getLogDir() + "/gui.log";
    QDesktopServices::openUrl(QUrl::fromLocalFile(guiLog));
}

void MainWindow::onCoreLog()
{
    QString trojanLog = Utils::getLogDir() + "/core.log";
    QDesktopServices::openUrl(QUrl::fromLocalFile(trojanLog));
}

void MainWindow::onReportBug()
{
    QDesktopServices::openUrl(issueUrl);
}

void MainWindow::onCustomContextMenuRequested(const QPoint &pos)
{
    this->checkCurrentIndex(ui->connectionView->indexAt(pos));
    ui->menuConnection->popup(ui->connectionView->viewport()->mapToGlobal(pos));
}

void MainWindow::onFilterToggled(bool show)
{
    if (show) {
        ui->filterLineEdit->setFocus();
    }
}

void MainWindow::onFilterTextChanged(const QString &text)
{
    proxyModel->setFilterWildcard(text);
}

void MainWindow::onQRCodeCapturerResultFound(const QString &uri)
{
    QRCodeCapturer* capturer = qobject_cast<QRCodeCapturer*>(sender());
    // Disconnect immediately to avoid duplicate signals
    disconnect(capturer, &QRCodeCapturer::qrCodeFound,
               this, &MainWindow::onQRCodeCapturerResultFound);
    Connection *newCon = new Connection(uri, this);
    newProfile(newCon);
}

void MainWindow::onCheckUpdate()
{
#if defined (Q_OS_WIN)
    win_sparkle_check_update_with_ui();
#elif defined (Q_OS_MAC)
    updater->checkForUpdates();
#endif

}

void MainWindow::hideEvent(QHideEvent *e)
{
    QMainWindow::hideEvent(e);
    notifier->onWindowVisibleChanged(false);
}

void MainWindow::showEvent(QShowEvent *e)
{
    QMainWindow::showEvent(e);
    notifier->onWindowVisibleChanged(true);
    this->setFocus();
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    if (e->spontaneous()) {
        e->ignore();
        hide();
    } else {
        QMainWindow::closeEvent(e);
    }
}

void MainWindow::onStatusAvailable(const quint64 &u, const quint64 &d)
{
    QList<int> ports;
    ports.append(configHelper->getInboundSettings()["socks5LocalPort"].toInt());
    ports.append(configHelper->getInboundSettings()["httpLocalPort"].toInt());
    ports.append(configHelper->getInboundSettings()["pacLocalPort"].toInt());
    QList<QString> stats;
    stats.append(Utils::bytesConvertor(d));
    stats.append(Utils::bytesConvertor(u));
    stats.append(Utils::bytesConvertor(MidMan::getConnection().getProfile().totalDownloadUsage));
    stats.append(Utils::bytesConvertor(MidMan::getConnection().getProfile().totalUploadUsage));
    m_statusBar->refresh(configHelper->getInboundSettings()["enableIpv6Support"].toBool() ? (configHelper->getInboundSettings()["shareOverLan"].toBool() ? "::" : "::1") : (configHelper->getInboundSettings()["shareOverLan"].toBool() ? "0.0.0.0" : "127.0.0.1"),
    ports,
    stats);
}

void MainWindow::setupActionIcon()
{

    QtAwesome* awesome = new QtAwesome(this);
    awesome->initFontAwesome();

    ui->actionConnect->setIcon(awesome->icon(fas::link));
    ui->actionDisconnect->setIcon(awesome->icon(fas::unlink));
    ui->actionEdit->setIcon(awesome->icon(fas::edit));
    ui->actionShare->setIcon(awesome->icon(fas::share));
    ui->actionTestLatency->setIcon(awesome->icon(fas::tachometer));
    ui->actionDelete->setIcon(awesome->icon(fas::trash));
    ui->actionMoveUp->setIcon(awesome->icon(fas::up));
    ui->actionMoveDown->setIcon(awesome->icon(fas::down));
    ui->actionImportGUIJson->setIcon(QIcon::fromTheme("document-import",
                                     QIcon::fromTheme("insert-text")));
    ui->actionImportConfigYaml->setIcon(QIcon::fromTheme("document-import",
                                     QIcon::fromTheme("insert-text")));
    ui->actionExportGUIJson->setIcon(QIcon::fromTheme("document-export",
                                     QIcon::fromTheme("document-save-as")));
    ui->actionExportShadowrocketJson->setIcon(QIcon::fromTheme("document-export",
                                              QIcon::fromTheme("document-save-as")));
    ui->actionExportSubscribe->setIcon(QIcon::fromTheme("document-export",
                                       QIcon::fromTheme("document-save-as")));
    ui->menuManually->setIcon(QIcon::fromTheme("edit-guides",
                                          QIcon::fromTheme("accessories-text-editor")));
    ui->actionAdd_SOCKS5_Manually->setIcon(QIcon::fromTheme("edit-guides",
                                QIcon::fromTheme("accessories-text-editor")));
    ui->actionAdd_HTTP_Manually->setIcon(QIcon::fromTheme("edit-guides",
                                QIcon::fromTheme("accessories-text-editor")));
    ui->actionAdd_SS_Manually->setIcon(QIcon::fromTheme("edit-guides",
                                QIcon::fromTheme("accessories-text-editor")));
    ui->actionAdd_SSR_Manually->setIcon(QIcon::fromTheme("edit-guides",
                                QIcon::fromTheme("accessories-text-editor")));
    ui->actionAdd_Vmess_Manually->setIcon(QIcon::fromTheme("edit-guides",
                                QIcon::fromTheme("accessories-text-editor")));
    ui->actionAdd_Trojan_Manually->setIcon(QIcon::fromTheme("edit-guides",
                                QIcon::fromTheme("accessories-text-editor")));
    ui->actionAdd_Snell_Manually->setIcon(QIcon::fromTheme("edit-guides",
                                QIcon::fromTheme("accessories-text-editor")));
    ui->actionAdd_NaiveProxy_Manually->setIcon(QIcon::fromTheme("edit-guides",
                                QIcon::fromTheme("accessories-text-editor")));
    ui->actionURI->setIcon(QIcon::fromTheme("text-field",
                           QIcon::fromTheme("insert-link")));
    ui->actionPasteBoardURI->setIcon(QIcon::fromTheme("text-field",
                           QIcon::fromTheme("insert-link")));
    ui->actionQRCode->setIcon(QIcon::fromTheme("edit-image-face-recognize",
                              QIcon::fromTheme("insert-image")));
    ui->actionScanQRCodeCapturer->setIcon(ui->actionQRCode->icon());
    ui->actionGeneralSettings->setIcon(QIcon::fromTheme("configure",
                                       QIcon::fromTheme("preferences-desktop")));
    ui->actionUserRuleSerttings->setIcon(QIcon::fromTheme("configure",
                                         QIcon::fromTheme("preferences-desktop")));
    ui->actionGuiLog->setIcon(QIcon::fromTheme("view-list-text",
                              QIcon::fromTheme("text-x-preview")));
    ui->actionCoreLog->setIcon(QIcon::fromTheme("view-list-text",
                                 QIcon::fromTheme("text-x-preview")));
    ui->actionReportBug->setIcon(QIcon::fromTheme("tools-report-bug",
                                 QIcon::fromTheme("help-faq")));
}

bool MainWindow::isInstanceRunning() const
{
    return instanceRunning;
}

void MainWindow::initSparkle()
{
#if defined (Q_OS_WIN)
    win_sparkle_set_appcast_url("https://raw.githubusercontent.com/Trojan-Qt5/Trojan-Qt5/master/resources/Appcast_Windows.xml");
    win_sparkle_set_app_details(reinterpret_cast<const wchar_t *>(QCoreApplication::organizationName().utf16()),
                                reinterpret_cast<const wchar_t *>(QCoreApplication::applicationName().utf16()),
                                reinterpret_cast<const wchar_t *>(QStringLiteral(APP_VERSION).utf16()));
    win_sparkle_set_dsa_pub_pem(reinterpret_cast<const char *>(QResource(":/pem/dsa_pub.pem").data()));
    win_sparkle_init();
#elif defined (Q_OS_MAC)
    CocoaInitializer initializer;
    updater = new SparkleAutoUpdater("https://raw.githubusercontent.com/Trojan-Qt5/Trojan-Qt5/master/resources/Appcast_macOS.xml");
#endif
}

void MainWindow::initLog()
{
    QDir path = Utils::getLogDir();
    if (!path.exists()) {
        path.mkpath(".");
    }
    QString guiLog = path.path() + "/gui.log";

    //Initialize the gui's log.
    Logger::init(guiLog);

}

void MainWindow::initSingleInstance()
{
    const QString serverName = QCoreApplication::applicationName();
    QLocalSocket socket;
    socket.connectToServer(serverName);
    if (socket.waitForConnected(500)) {
        instanceRunning = true;
        if (configHelper->getGeneralSettings()["onlyOneInstace"].toBool()) {
            Logger::warning("[Instance] An instance of trojan-qt5 is already running");
        }
        QByteArray username = qgetenv("USER");
        if (username.isEmpty()) {
            username = qgetenv("USERNAME");
        }
        socket.write(username);
        socket.waitForBytesWritten();
        return;
    }

    /* Can't connect to server, indicating it's the first instance of the user */
    instanceServer = new QLocalServer(this);
    instanceServer->setSocketOptions(QLocalServer::WorldAccessOption);
    connect(instanceServer, &QLocalServer::newConnection,
            this, &MainWindow::onSingleInstanceConnect);
    if (instanceServer->listen(serverName)) {
        /* Remove server in case of crashes */
        if (instanceServer->serverError() == QAbstractSocket::AddressInUseError &&
                QFile::exists(instanceServer->serverName())) {
            QFile::remove(instanceServer->serverName());
            instanceServer->listen(serverName);
        }
    }
}

void MainWindow::onSingleInstanceConnect()
{
    QLocalSocket *socket = instanceServer->nextPendingConnection();
    if (!socket) {
        return;
    }

    if (socket->waitForReadyRead(1000)) {
        QByteArray username = qgetenv("USER");
        if (username.isEmpty()) {
            username = qgetenv("USERNAME");
        }

        QByteArray recvUsername = socket->readAll();
        if (recvUsername == username) {
            // Only show the window if it's the same user
            show();
        } else {
            Logger::warning("[Instance] Another user is trying to run another instance of trojan-qt5");
        }
    }
    socket->deleteLater();
}
