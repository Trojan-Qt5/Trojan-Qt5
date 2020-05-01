#ifndef SNELLEDITDIALOG_H
#define SNELLEDITDIALOG_H

#include <QDialog>

namespace Ui {
class SnellEditDialog;
}

class SnellEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SnellEditDialog(QWidget *parent = nullptr);
    ~SnellEditDialog();

private:
    Ui::SnellEditDialog *ui;
};

#endif // SNELLEDITDIALOG_H
