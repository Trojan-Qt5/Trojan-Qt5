#ifndef VMESSEDITDIALOG_H
#define VMESSEDITDIALOG_H

#include <QDialog>
#include "connection.h"
#include "streamwidget.h"

namespace Ui {
class VmessEditDialog;
}

class VmessEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit VmessEditDialog(Connection *_connection, QWidget *parent = 0);
    ~VmessEditDialog();

private:
    Ui::VmessEditDialog *ui;
    Connection *connection;
    StreamWidget *streamWidget;

private slots:
    void save();
};

#endif // VMESSEDITDIALOG_H
