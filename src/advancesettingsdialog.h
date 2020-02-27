#ifndef ADVANCESETTINGSDIALOG_H
#define ADVANCESETTINGSDIALOG_H

#include "confighelper.h"

#include <QDialog>

namespace Ui {
class AdvanceSettingsDialog;
}

class AdvanceSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AdvanceSettingsDialog(ConfigHelper *ch, QWidget *parent = nullptr);
    ~AdvanceSettingsDialog();

private:
    Ui::AdvanceSettingsDialog *ui;
    ConfigHelper *helper;

private slots:
    void onAccepted();
    void onChanged();
};

#endif // ADVANCESETTINGSDIALOG_H
