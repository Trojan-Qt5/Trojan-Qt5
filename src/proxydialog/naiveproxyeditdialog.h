#ifndef NAIVEPROXYEDITDIALOG_H
#define NAIVEPROXYEDITDIALOG_H

#include <QDialog>

namespace Ui {
class NaiveProxyEditDialog;
}

class NaiveProxyEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NaiveProxyEditDialog(QWidget *parent = nullptr);
    ~NaiveProxyEditDialog();

private:
    Ui::NaiveProxyEditDialog *ui;
};

#endif // NAIVEPROXYEDITDIALOG_H
