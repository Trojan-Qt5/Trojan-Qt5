#ifndef USERRULESDIALOG_H
#define USERRULESDIALOG_H

#include <QDialog>

namespace Ui {
class UserRulesDialog;
}

class UserRulesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UserRulesDialog(QWidget *parent = nullptr);
    ~UserRulesDialog();

private slots:
    void onAccepted();

private:
    QString configDir;
    QString userRule;
    Ui::UserRulesDialog *ui;
};

#endif // USERRULESDIALOG_H
