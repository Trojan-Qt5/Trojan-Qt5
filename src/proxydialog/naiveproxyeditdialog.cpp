#include "naiveproxyeditdialog.h"
#include "ui_naiveproxyeditdialog.h"

NaiveProxyEditDialog::NaiveProxyEditDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NaiveProxyEditDialog)
{
    ui->setupUi(this);
}

NaiveProxyEditDialog::~NaiveProxyEditDialog()
{
    delete ui;
}
