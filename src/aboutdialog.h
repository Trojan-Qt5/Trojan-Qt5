#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>

#ifndef _TROJAN_QT5_BUILD_INFO_STR_
#define _TROJAN_QT5_BUILD_INFO_STR_ "Trojan-Qt5 build from local machine"
#endif

#define TROJAN_QT5_BUILD_INFO QString(_TROJAN_QT5_BUILD_INFO_STR_)
#define TROJAN_QT5_BUILD_EXTRA_INFO QString("Aku seneng felix")

namespace Ui {
class AboutDialog;
}

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = nullptr);
    ~AboutDialog();

private:
    Ui::AboutDialog *ui;
};

#endif // ABOUTDIALOG_H
