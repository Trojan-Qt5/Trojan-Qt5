/**
  * @brief Subscribe Dialog
  * @ref https://github.com/shadowsocksrr/shadowsocksr-csharp/blob/master/shadowsocks-csharp/View/SubscribeForm.cs
  *
**/
#include "subscribedialog.h"
#include "ui_subscribedialog.h"

SubscribeDialog::SubscribeDialog(ConfigHelper *ch, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SubscribeDialog),
    helper(ch)
{
    ui->setupUi(this);

    connect(ui->addButton, &QPushButton::clicked, this, &SubscribeDialog::onAdd);
    connect(ui->deleteButton, &QPushButton::clicked, this, &SubscribeDialog::onDelete);
    connect(ui->confirmButton, &QPushButton::clicked, this, &SubscribeDialog::onAccepted);
    connect(ui->listView, SIGNAL(clicked(QModelIndex)), this, SLOT(onClicked(QModelIndex)));

    ItemModel = new QStandardItemModel(this);
    subscribes = helper->readSubscribes();

    int select_index = 0;
    UpdateList();
    UpdateSelected(select_index);
    SetSelectIndex(select_index);

    if (subscribes.size() == 0)
    {
        ui->urlLineEdit->setDisabled(true);
    }
    else
    {
        ui->urlLineEdit->setEnabled(true);
    }

    ui->autoUpdateCheckBox->setChecked(helper->isAutoUpdateSubscribes());
}

SubscribeDialog::~SubscribeDialog()
{
    delete ui;
}

void SubscribeDialog::UpdateList()
{
    ItemModel->clear();
    for (int i=0; i< subscribes.size(); ++i) {
        TQSubscribe sb = subscribes[i];
        QString str = (sb.groupName.isEmpty() ? "    " : sb.groupName + " - ") + sb.url;
        QStandardItem *item = new QStandardItem(str);
        ItemModel->appendRow(item);
        ui->listView->setModel(ItemModel);
    }
}

void SubscribeDialog::onClicked(QModelIndex index)
{
    int select_index = index.row();
    if (_old_select_index == select_index)
        return;

    SaveSelected(_old_select_index);
    UpdateList();
    UpdateSelected(select_index);
    SetSelectIndex(select_index);
}

void SubscribeDialog::SetSelectIndex(int index)
{
     if (index >= 0 && index < subscribes.size()) {
        ui->listView->setCurrentIndex(ui->listView->model()->index(index, 0));
     }
}

void SubscribeDialog::UpdateSelected(int index)
{
    if (index >= 0 && index < subscribes.size())
    {
        TQSubscribe sb = subscribes[index];
        ui->urlLineEdit->setText(sb.url);
        ui->groupNameLineEdit->setText(sb.groupName);
        _old_select_index = index;
        if (sb.lastUpdateTime != 0)
        {
            QDateTime now;
            now.setTime_t(sb.lastUpdateTime + QDateTime::fromString("1970-01-01T00:00:00").toTime_t());
            ui->recentUpdateLineEdit->setText(now.toString(Qt::SystemLocaleShortDate));
        }
        else
        {
            ui->recentUpdateLineEdit->setText("(｢･ω･)｢");
        }
    }

}

void SubscribeDialog::SaveSelected(int index)
{
    if (index >= 0 && index < subscribes.size())
    {
        TQSubscribe sb = subscribes[index];
        if (sb.url != ui->urlLineEdit->text())
        {
            sb.url = ui->urlLineEdit->text();
            sb.groupName = ui->groupNameLineEdit->text();
            sb.lastUpdateTime = 0;
        } else if (sb.groupName != ui->groupNameLineEdit->text())
        {
            sb.groupName = ui->groupNameLineEdit->text();
        }
        subscribes[index] = sb;
    }
}

void SubscribeDialog::onAdd()
{
    SaveSelected(_old_select_index);
    int select_index = subscribes.size();
    if (_old_select_index >= 0 && _old_select_index < subscribes.size())
    {
        subscribes.insert(select_index, TQSubscribe());
    }
    else
    {
        subscribes.append(TQSubscribe());
    }
    UpdateList();
    UpdateSelected(select_index);
    SetSelectIndex(select_index);

    ui->urlLineEdit->setEnabled(true);
}

void SubscribeDialog::onDelete()
{
    int select_index = ui->listView->currentIndex().row();
    if (select_index >= 0 && select_index < subscribes.size())
    {
        subscribes.removeAt(select_index);
        if (select_index >= 0 && select_index < subscribes.size())
        {
            select_index = subscribes.size() - 1;
        }
        UpdateList();
        UpdateSelected(select_index);
        SetSelectIndex(select_index);
    }
    if (subscribes.size() == 0)
    {
        ui->urlLineEdit->setDisabled(true);
    }
}

void SubscribeDialog::onAccepted()
{
    int select_index = ui->listView->currentIndex().row();
    SaveSelected(select_index);
    helper->saveSubscribes(subscribes);

    helper->setAutoUpdateSubscribes(ui->autoUpdateCheckBox->isChecked());

    this->accept();
}
