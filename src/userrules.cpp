#include "userrules.h"
#include "ui_userrules.h"

#include <QCoreApplication>
#include <QDir>
#include <QTextStream>
#include <QTextCodec>

UserRules::UserRules(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UserRules)
{
#ifdef Q_OS_WIN
    configDir = QDir::toNativeSeparators(QCoreApplication::applicationDirPath()) + "\\pac";
#else
    configDir = QDir::homePath() + "/.config/trojan-qt5/pac";
#endif
    userRule = configDir + "/user-rule.txt";

    QFile file(userRule);
    file.open(QIODevice::ReadOnly);

    ui->setupUi(this);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &UserRules::onAccepted);
    ui->textEdit->setText(file.readAll());
    file.close();
}

UserRules::~UserRules()
{
    delete ui;
}

void UserRules::onAccepted()
{
     QFile file(userRule);
     file.open(QIODevice::Truncate | QIODevice::ReadWrite);
     QTextStream out(&file);
     //must manually set utf-8 encoing otherwise the text will have garbled characters
     out.setCodec(QTextCodec::codecForName("utf-8"));
     out << ui->textEdit->document()->toPlainText();
     file.close();

     this->accept();
}
