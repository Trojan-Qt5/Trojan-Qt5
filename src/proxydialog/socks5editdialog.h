#ifndef SOCKS5EDITDIALOG_H
#define SOCKS5EDITDIALOG_H

#include <QDialog>

#include "connection.h"

namespace Ui {
class Socks5EditDialog;
}

class Socks5EditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit Socks5EditDialog(Connection *_connection, QWidget *parent = 0);
    ~Socks5EditDialog();

private:
    Ui::Socks5EditDialog *ui;
    Connection *connection;

private slots:
    void save();
};

#endif // SOCKS5EDITDIALOG_H
