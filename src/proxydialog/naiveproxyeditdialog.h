#ifndef NAIVEPROXYEDITDIALOG_H
#define NAIVEPROXYEDITDIALOG_H

#include <QDialog>

#include "connection.h"

namespace Ui {
class NaiveProxyEditDialog;
}

class NaiveProxyEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NaiveProxyEditDialog(Connection *_connection, QWidget *parent = 0);
    ~NaiveProxyEditDialog();

private:
    Ui::NaiveProxyEditDialog *ui;
    Connection *connection;

private slots:
    void save();
};

#endif // NAIVEPROXYEDITDIALOG_H
