#ifndef SNELLEDITDIALOG_H
#define SNELLEDITDIALOG_H

#include <QDialog>

#include "connection.h"

namespace Ui {
class SnellEditDialog;
}

class SnellEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SnellEditDialog(Connection *_connection, QWidget *parent = 0);
    ~SnellEditDialog();

    void save();

private:
    Ui::SnellEditDialog *ui;
    Connection *connection;
};

#endif // SNELLEDITDIALOG_H
