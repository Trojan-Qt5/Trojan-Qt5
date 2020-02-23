#ifndef USERRULES_H
#define USERRULES_H

#include <QDialog>

namespace Ui {
class UserRules;
}

class UserRules : public QDialog
{
    Q_OBJECT

public:
    explicit UserRules(QWidget *parent = nullptr);
    ~UserRules();

private slots:
    void onAccepted();

private:
    QString configDir;
    QString userRule;
    Ui::UserRules *ui;
};

#endif // USERRULES_H
