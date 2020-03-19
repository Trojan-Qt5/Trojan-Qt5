#include "connectionsortfilterproxymodel.h"

#include <QDate>
#include <QDebug>

ConnectionSortFilterProxyModel::ConnectionSortFilterProxyModel(QObject *parent) : QSortFilterProxyModel(parent)
{}

bool ConnectionSortFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    QVariant leftData = sourceModel()->data(left);
    QVariant rightData = sourceModel()->data(right);

    //process lastUse Time
    if (leftData.type() == QVariant::Date) {
        return leftData.toDate() < rightData.toDate();
    }

    //process latency
    if (left.column() == 3) {
        float leftLatency = 0;
        float rightLatency = 0;
        if (leftData.toString().endsWith("ms")) {
            leftLatency = leftData.toString().replace("ms", "").toFloat();
        } else if (leftData.toString().endsWith("s")) {
            leftLatency = leftData.toString().replace("s", "").toFloat() * 1000;
        } else {
            leftLatency = 999999;
        }
        if (rightData.toString().endsWith("ms")) {
            rightLatency = rightData.toString().replace("ms", "").toFloat();
        } else if (rightData.toString().endsWith("s")) {
            rightLatency = rightData.toString().replace("s", "").toFloat() * 1000;
        } else {
            rightLatency = 999999;
        }
        return leftLatency < rightLatency;
    }

    int c;

    if (isSortLocaleAware()) {
        c = leftData.toString().localeAwareCompare(rightData.toString());
        if (c < 0)
            return true;
        else if (c > 0)
            return false;
    } else {
        c = leftData.toString().compare(rightData.toString(), sortCaseSensitivity());
        if (c < 0)
            return true;
        else if (c > 0)
            return false;
    }

}
