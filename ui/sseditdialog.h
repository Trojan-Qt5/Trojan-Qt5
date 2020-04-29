#ifndef SSEDITDIALOG_H
#define SSEDITDIALOG_H

#include <QDialog>

namespace Ui {
class SSEditDialog;
}

class SSEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SSEditDialog(QWidget *parent = nullptr);
    ~SSEditDialog();

private:
    Ui::SSEditDialog *ui;
};

#endif // SSEDITDIALOG_H
