#include "vmesseditdialog.h"
#include "ui_vmesseditdialog.h"

VMessEditDialog::VMessEditDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VMessEditDialog)
{
    ui->setupUi(this);
}

VMessEditDialog::~VMessEditDialog()
{
    delete ui;
}
