#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "advancesettingsdialog.h"
#include "connection.h"
#include "editdialog.h"
#include "urihelper.h"
#include "uriinputdialog.h"
#include "userrules.h"
#include "sharedialog.h"
#include "settingsdialog.h"
#include "qrcodecapturer.h"
#include "trojanvalidator.h"

#include <boost/version.hpp>
#include <openssl/opensslv.h>
#include "core/version.h"
#include "qrencode.h"
#if defined (Q_OS_WIN)
#define VERSION "3.0.28 Binary"
#else
#include "privoxy/config.h"
#endif
#include "core/log.h"
#include "logger.h"
#include "QtAwesome.h"

#include <QClipboard>
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
    ui->toolBar->setToolButtonStyle(static_cast<Qt::ToolButtonStyle>
                                    (configHelper->getToolbarStyle()));

    setupActionIcon();

    sbMgr = new SubscribeManager(configHelper, this);
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
    ui->menuBar->setNativeMenuBar(configHelper->isNativeMenuBar());

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
    connect(ui->actionExportGUIJson, &QAction::triggered,
            this, &MainWindow::onExportGuiJson);
    connect(ui->actionExportShadowrocketJson, &QAction::triggered,
            this, &MainWindow::onExportShadowrocketJson);
    connect(ui->actionQuit, &QAction::triggered, qApp, &QApplication::quit);
    connect(ui->actionManually, &QAction::triggered,
            this, &MainWindow::onAddManually);
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
    connect(ui->actionMoveUp, &QAction::triggered, this, &MainWindow::onMoveUp);
    connect(ui->actionMoveDown, &QAction::triggered,
            this, &MainWindow::onMoveDown);
    connect(ui->actionGeneralSettings, &QAction::triggered,
            this, &MainWindow::onGeneralSettings);
    connect(ui->actionAdvanceSettings, &QAction::triggered,
            this, &MainWindow::onAdvanceSettings);
    connect(ui->actionUserRuleSerttings, &QAction::triggered,
            this, &MainWindow::onUserRuleSettings);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::onAbout);
    connect(ui->actionAboutQt, &QAction::triggered,
            qApp, &QApplication::aboutQt);
    connect(ui->actionCheckUpdate, &QAction::triggered,
            this, &MainWindow::onCheckUpdate);
    connect(ui->actionGuiLog, &QAction::triggered,
            this, &MainWindow::onGuiLog);
    connect(ui->actionTrojanLog, &QAction::triggered,
            this, &MainWindow::onTrojanLog);
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
            this, &MainWindow::onEdit);

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
        QUrl("https://github.com/TheWanderingCoel/Trojan-Qt5/issues");

void MainWindow::startAutoStartConnections()
{
    configHelper->startAllAutoStart(*model);
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
            con->start();
        }
    }
}

void MainWindow::onAddURIFromSubscribe(QString uri)
{
    Connection *newCon = new Connection(uri, this);
    if (!model->isDuplicate(newCon)) {
        model->appendConnection(newCon);
        configHelper->save(*model);
    }
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

void MainWindow::onSaveManually()
{
    configHelper->save(*model);
}

void MainWindow::onAddManually()
{
    Connection *newCon = new Connection;
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
    for (QString uri: str.split("\\r\\n")) {
        if (TrojanValidator::validate(uri)) {
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
    if (model->removeRow(proxyModel->mapToSource(
                         ui->connectionView->currentIndex()).row())) {
        configHelper->save(*model);
    }
    checkCurrentIndex();
}

void MainWindow::onEdit()
{
    editRow(proxyModel->mapToSource(ui->connectionView->currentIndex()).row());
}

void MainWindow::onShare()
{
    QByteArray uri = model->getItem(
                proxyModel->mapToSource(ui->connectionView->currentIndex()).
                row())->getConnection()->getURI();
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
        con->start();
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
        con->start();
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

void MainWindow::onConnectionStatusChanged(const int row, const bool running)
{
    if (proxyModel->mapToSource(
                ui->connectionView->currentIndex()).row() == row) {
        ui->actionConnect->setEnabled(!running);
        ui->actionDisconnect->setEnabled(running);
        ui->actionDelete->setEnabled(!running);
    }
}

void MainWindow::onLatencyTest()
{
    model->getItem(proxyModel->mapToSource(ui->connectionView->currentIndex()).
                   row())->testLatency();
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

void MainWindow::onAdvanceSettings()
{
    AdvanceSettingsDialog *sDlg = new AdvanceSettingsDialog(configHelper, this);
    connect(sDlg, &AdvanceSettingsDialog::finished,
            sDlg, &AdvanceSettingsDialog::deleteLater);
    if (sDlg->exec()) {
        configHelper->save(*model);
    }
}

void MainWindow::onUserRuleSettings()
{
    UserRules *userRule = new UserRules(this);
    connect(userRule, &UserRules::finished,
            userRule, &UserRules::deleteLater);
    userRule->exec();
}

void MainWindow::newProfile(Connection *newCon)
{
    EditDialog *editDlg = new EditDialog(newCon, this);
    connect(editDlg, &EditDialog::finished, editDlg, &EditDialog::deleteLater);
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
    EditDialog *editDlg = new EditDialog(con, this);
    connect(editDlg, &EditDialog::finished, editDlg, &EditDialog::deleteLater);
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
    QString text = QString("<h1>Trojan-Qt5</h1><p><b>Version %1</b><br/>"
            "Using trojan %2, Privoxy %3<br>"
            "Boost %4, %5<br>"
            "LibQREncode %6</p>"
            "<p>Copyright © 2014-2018 Symeon Huang "
            "(<a href='https://twitter.com/librehat'>"
            "@librehat</a>)</p>"
            "<p>Copyright © 2019-2020 TheWanderingCoel "
            "(<a href='https://github.com/TheWanderingCoel'>"
            "@TheWanderingCoel</a>)</p>"
            "<p>License: <a href='http://www.gnu.org/licenses/gpl.html'>"
            "GNU General Public License Version 3</a><br />"
            "Project Hosted at "
            "<a href='https://github.com/TheWanderingCoel/Trojan-Qt5'>"
            "GitHub</a></p>")
            .arg(QStringLiteral(APP_VERSION))
            .arg(QString::fromStdString(Version::get_version()))
            .arg(QStringLiteral(VERSION))
            .arg(QStringLiteral(BOOST_LIB_VERSION))
            .arg(QStringLiteral(OPENSSL_VERSION_TEXT))
            .arg(QRcode_APIVersionString());
    QMessageBox::about(this, tr("About"), text);
}

void MainWindow::onGuiLog()
{
    QDir path = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/Library/Logs/Trojan-Qt5";
    QString guiLog = path.path() + "/gui.log";
    QDesktopServices::openUrl(QUrl::fromLocalFile(guiLog));
}

void MainWindow::onTrojanLog()
{
    QDir path = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/Library/Logs/Trojan-Qt5";
    QString trojanLog = path.path() + "/trojan.log";
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
    ui->actionExportGUIJson->setIcon(QIcon::fromTheme("document-export",
                                     QIcon::fromTheme("document-save-as")));
    ui->actionExportShadowrocketJson->setIcon(QIcon::fromTheme("document-export",
                                              QIcon::fromTheme("document-save-as")));
    ui->actionManually->setIcon(QIcon::fromTheme("edit-guides",
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
    ui->actionAdvanceSettings->setIcon(QIcon::fromTheme("configure",
                                       QIcon::fromTheme("preferences-desktop")));
    ui->actionUserRuleSerttings->setIcon(QIcon::fromTheme("configure",
                                         QIcon::fromTheme("preferences-desktop")));
    ui->actionGuiLog->setIcon(QIcon::fromTheme("view-list-text",
                              QIcon::fromTheme("text-x-preview")));
    ui->actionTrojanLog->setIcon(QIcon::fromTheme("view-list-text",
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
    win_sparkle_set_appcast_url("https://raw.githubusercontent.com/TheWanderingCoel/Trojan-Qt5/master/resources/Appcast_Windows.xml");
    win_sparkle_set_app_details(reinterpret_cast<const wchar_t *>(QCoreApplication::organizationName().utf16()),
                                reinterpret_cast<const wchar_t *>(QCoreApplication::applicationName().utf16()),
                                reinterpret_cast<const wchar_t *>(QStringLiteral(VERSION).utf16()));
    win_sparkle_set_dsa_pub_pem(reinterpret_cast<const char *>(QResource(":/pem/dsa_pub.pem").data()));
    win_sparkle_init();
#elif defined (Q_OS_MAC)
    CocoaInitializer initializer;
    updater = new SparkleAutoUpdater("https://www.crystalidea.com/update/fancontrol.xml.gz");
#endif
}

void MainWindow::initLog()
{
    QDir path = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/Library/Logs/Trojan-Qt5";
    if (!path.exists()) {
        path.mkpath(".");
    }
    QString guiLog = path.path() + "/gui.log";
    QString trojanLog = path.path() + "/trojan.log";

    /** Initialize the gui's log. */
    Logger::init(guiLog);

    /** Redirect Trojan's log to our logfile. */
    Log::redirect(trojanLog.toLocal8Bit().data());

}

void MainWindow::initSingleInstance()
{
    const QString serverName = QCoreApplication::applicationName();
    QLocalSocket socket;
    socket.connectToServer(serverName);
    if (socket.waitForConnected(500)) {
        instanceRunning = true;
        if (configHelper->isOnlyOneInstance()) {
            qWarning() << "An instance of trojan-qt5 is already running";
            Logger::warning("An instance of trojan-qt5 is already running");
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
            qWarning("Another user is trying to run another instance of trojan-qt5");
            Logger::warning("Another user is trying to run another instance of trojan-qt5");
        }
    }
    socket->deleteLater();
}
