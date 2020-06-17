#include "userrulesdialog.h"
#include "ui_userrulesdialog.h"

#include <QCoreApplication>
#include <QDir>
#include <QTextStream>
#include <QTextCodec>

UserRulesDialog::UserRulesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UserRulesDialog)
{
#ifdef Q_OS_WIN
    configDir = QDir::toNativeSeparators(qApp->applicationDirPath()) + "\\pac";
#else
    configDir = QDir::homePath() + "/.config/trojan-qt5/pac";
#endif
    userRule = configDir + "/user-rule.txt";

    QFile file(userRule);
    file.open(QIODevice::ReadOnly);
    QTextStream in(&file);
    in.setCodec(QTextCodec::codecForName("utf-8"));
    ui->setupUi(this);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &UserRulesDialog::onAccepted);
    ui->textEdit->setText(in.readAll());
    file.close();
}

UserRulesDialog::~UserRulesDialog()
{
    delete ui;
}

void UserRulesDialog::onAccepted()
{
     QFile file(userRule);
     file.open(QIODevice::Truncate | QIODevice::ReadWrite);
     QTextStream out(&file);
     //must manually set utf-8 encoing otherwise the text will have garbled characters
     out.setCodec(QTextCodec::codecForName("utf-8"));
     out << ui->textEdit->document()->toPlainText().replace("\r\n", "\n");
     file.close();

     this->accept();
}
