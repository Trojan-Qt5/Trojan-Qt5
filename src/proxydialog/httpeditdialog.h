#ifndef HTTPEDITDIALOG_H
#define HTTPEDITDIALOG_H

#include <QDialog>

#include "connection.h"

namespace Ui {
class HttpEditDialog;
}

class HttpEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HttpEditDialog(Connection *_connection, QWidget *parent = 0);
    ~HttpEditDialog();

private:
    Ui::HttpEditDialog *ui;
    Connection *connection;

private slots:
    void save();
};

#endif // HTTPEDITDIALOG_H
