#include "snelleditdialog.h"
#include "ui_snelleditdialog.h"

SnellEditDialog::SnellEditDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SnellEditDialog)
{
    ui->setupUi(this);
}

SnellEditDialog::~SnellEditDialog()
{
    delete ui;
}
