#ifndef CONNECTIONSORTFILTERPROXYMODEL_H
#define CONNECTIONSORTFILTERPROXYMODEL_H

#include <QObject>
#include <QSortFilterProxyModel>

class ConnectionSortFilterProxyModel: public QSortFilterProxyModel
{
    Q_OBJECT

public:
    ConnectionSortFilterProxyModel(QObject *parent = 0);

protected:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
};

#endif // CONNECTIONSORTFILTERPROXYMODEL_H
