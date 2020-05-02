#ifndef VMESSEDITDIALOG_H
#define VMESSEDITDIALOG_H

#include <QDialog>

namespace Ui {
class VMessEditDialog;
}

class VMessEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit VMessEditDialog(QWidget *parent = nullptr);
    ~VMessEditDialog();

private:
    Ui::VMessEditDialog *ui;
};

#endif // VMESSEDITDIALOG_H
