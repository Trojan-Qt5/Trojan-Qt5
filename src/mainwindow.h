#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSortFilterProxyModel>
#include <QLocalServer>
#include "connectiontablemodel.h"
#include "confighelper.h"
#include "statusnotifier.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(ConfigHelper *confHelper, QWidget *parent = 0);
    ~MainWindow();

    bool isInstanceRunning() const;

private:
    Ui::MainWindow *ui;

    ConnectionTableModel *model;
    QSortFilterProxyModel *proxyModel;
    ConfigHelper *configHelper;
    StatusNotifier *notifier;

    QLocalServer* instanceServer;
    bool instanceRunning;
    void initSingleInstance();

    void newProfile(Connection *);
    void editRow(int row);
    void checkCurrentIndex();
    void setupActionIcon();

    static const QUrl issueUrl;

private slots:
    void onImportGuiJson();
    void onExportGuiJson();
    void onSaveManually();
    void onAddManually();
    void onAddScreenQRCode();
    void onAddScreenQRCodeCapturer();
    void onAddQRCodeFile();
    void onAddFromURI();
    void onAddFromConfigJSON();
    void onDelete();
    void onEdit();
    void onShare();
    void onConnect();
    void onForceConnect();
    void onDisconnect();
    void onConnectionStatusChanged(const int row, const bool running);
    void onLatencyTest();
    void onMoveUp();
    void onMoveDown();
    void onGeneralSettings();
    void checkCurrentIndex(const QModelIndex &index);
    void onAbout();
    void onReportBug();
    void onCustomContextMenuRequested(const QPoint &pos);
    void onFilterToggled(bool);
    void onFilterTextChanged(const QString &text);
    void onQRCodeCapturerResultFound(const QString &uri);
    void onSingleInstanceConnect();
};
#endif // MAINWINDOW_H
