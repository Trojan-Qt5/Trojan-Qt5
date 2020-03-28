#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include <QPushButton>
#include <QMessageBox>

SettingsDialog::SettingsDialog(ConfigHelper *ch, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog),
    helper(ch)
{
    ui->setupUi(this);

    helper->readGeneralSettings();

    ui->toolbarStyleComboBox->setCurrentIndex(helper->getToolbarStyle());
    ui->hideCheckBox->setChecked(helper->isHideWindowOnStartup());
    ui->startAtLoginCheckbox->setChecked(helper->isStartAtLogin());
    ui->oneInstanceCheckBox->setChecked(helper->isOnlyOneInstance());
    ui->availabilityCheckBox->setChecked(helper->isCheckPortAvailability());
    ui->enableNotificationCheckBox->setChecked(helper->isEnableNotification());
    ui->hideDockIconCheckBox->setChecked(helper->isHideDockIcon());
    ui->nativeMenuBarCheckBox->setChecked(helper->isNativeMenuBar());

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &SettingsDialog::onAccepted);
    connect(ui->toolbarStyleComboBox, &QComboBox::currentTextChanged, this, &SettingsDialog::onChanged);
    connect(ui->hideCheckBox, &QCheckBox::stateChanged, this, &SettingsDialog::onChanged);
    connect(ui->startAtLoginCheckbox, &QCheckBox::stateChanged, this, &SettingsDialog::onChanged);
    connect(ui->oneInstanceCheckBox, &QCheckBox::stateChanged, this, &SettingsDialog::onChanged);
    connect(ui->availabilityCheckBox, &QCheckBox::stateChanged, this, &SettingsDialog::onChanged);
    connect(ui->enableNotificationCheckBox, &QCheckBox::stateChanged, this, &SettingsDialog::onChanged);
    connect(ui->hideDockIconCheckBox, &QCheckBox::stateChanged, this, &SettingsDialog::onChanged);
    connect(ui->nativeMenuBarCheckBox, &QCheckBox::stateChanged, this, &SettingsDialog::onChanged);

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

    this->adjustSize();
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::onAccepted()
{
#if defined (Q_OS_MAC)
    // this will result in no title bar
    if (ui->hideDockIconCheckBox->isChecked() && ui->nativeMenuBarCheckBox->isChecked()) {
        QMessageBox::critical(this, tr("Invalid"),
                              tr("You can not hide dock Icon and use native menu bar at the same time"));
        return;
    }
#endif

    helper->setGeneralSettings(ui->toolbarStyleComboBox->currentIndex(),
                               ui->hideCheckBox->isChecked(),
                               ui->startAtLoginCheckbox->isChecked(),
                               ui->oneInstanceCheckBox->isChecked(),
                               ui->availabilityCheckBox->isChecked(),
                               ui->enableNotificationCheckBox->isChecked(),
                               ui->hideDockIconCheckBox->isChecked(),
                               ui->nativeMenuBarCheckBox->isChecked());
    this->accept();
}

void SettingsDialog::onChanged()
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}
