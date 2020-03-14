#ifndef SUBSCRIBEDIALOG_H
#define SUBSCRIBEDIALOG_H

#include "confighelper.h"
#include "tqsubscribe.h"

#include <QDialog>
#include <QStringListModel>
#include <QStandardItemModel>

namespace Ui {
class SubscribeDialog;
}

class SubscribeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SubscribeDialog(ConfigHelper *ch, QWidget *parent = nullptr);
    ~SubscribeDialog();

    void UpdateList();
    void SetSelectIndex(int index);
    void UpdateSelected(int index);
    void SaveSelected(int index);

    QStringListModel *Model;
    QStandardItemModel *ItemModel;
    QList<TQSubscribe> subscribes;

    int _old_select_index;

public slots:
    void onAccepted();
    void onClicked(QModelIndex index);
    void onAdd();
    void onDelete();

private:
    Ui::SubscribeDialog *ui;
    ConfigHelper *helper;

};

#endif // SUBSCRIBEDIALOG_H
