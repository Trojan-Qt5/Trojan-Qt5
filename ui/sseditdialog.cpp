#include "sseditdialog.h"
#include "ui_sseditdialog.h"

SSEditDialog::SSEditDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SSEditDialog)
{
    ui->setupUi(this);
}

SSEditDialog::~SSEditDialog()
{
    delete ui;
}
