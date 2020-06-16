#ifndef SSEDITDIALOG_H
#define SSEDITDIALOG_H

#include <QDialog>

#include "connection.h"

namespace Ui {
class SSEditDialog;
}

class SSEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SSEditDialog(Connection *_connection, QWidget *parent = 0);
    ~SSEditDialog();

private:
    Ui::SSEditDialog *ui;
    Connection *connection;

private slots:
    void save();
};

#endif // SSEDITDIALOG_H
