#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>

#define TROJAN_QT5_BUILD_INFO QString(_TROJAN_QT5_BUILD_INFO_STR_)
#define TROJAN_QT5_BUILD_EXTRA_INFO QString("Aku seneng Felix Wang")

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
