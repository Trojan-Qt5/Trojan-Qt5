#include "connectionsortfilterproxymodel.h"

#include <QDate>

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
    if (left.column() == 4) {
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

    //process traffic
    if (left.column() == 5 || left.column() == 6) {
        double leftTraffict = 0;
        double rightTraffict = 0;
        if (leftData.toString().endsWith("KiB")) {
            leftTraffict = leftData.toString().replace("KiB", "").toFloat() * 1024;
        } else if (leftData.toString().endsWith("MiB")) {
            leftTraffict = leftData.toString().replace("MiB", "").toFloat() * 1024 * 1024;
        } else if (leftData.toString().endsWith("GiB")) {
            leftTraffict = leftData.toString().replace("GiB", "").toFloat() * 1024 * 1024 * 1024;
        } else if (leftData.toString().endsWith("TiB")) {
            leftTraffict = leftData.toString().replace("TiB", "").toFloat() * 1024 * 1024 * 1024 * 1024;
        } else if (leftData.toString().endsWith("PiB")) {
            leftTraffict = leftData.toString().replace("PiB", "").toFloat() * 1024 * 1024 * 1024 * 1024 * 1024;
        } else if (leftData.toString().endsWith("EiB")) {
            leftTraffict = leftData.toString().replace("EiB", "").toFloat() * 1024 * 1024 * 1024 * 1024 * 1024 * 1024;
        } else if (leftData.toString().endsWith("ZiB")) {
            leftTraffict = leftData.toString().replace("ZiB", "").toFloat() * 1024 * 1024 * 1024 * 1024 * 1024 * 1024 * 1024;
        } else if (leftData.toString().endsWith("YiB")) {
            leftTraffict = leftData.toString().replace("YiB", "").toFloat() * 1024 * 1024 * 1024 * 1024 * 1024 * 1024 * 1024 * 1024;
        } else if (leftData.toString().endsWith("B")) {
            leftTraffict = leftData.toString().replace("B", "").toFloat();
        }

        if (rightData.toString().endsWith("KiB")) {
            rightTraffict = rightData.toString().replace("KiB", "").toFloat() * 1024;
        } else if (rightData.toString().endsWith("MiB")) {
            rightTraffict = rightData.toString().replace("MiB", "").toFloat() * 1024 * 1024;
        } else if (rightData.toString().endsWith("GiB")) {
            rightTraffict = rightData.toString().replace("GiB", "").toFloat() * 1024 * 1024 * 1024;
        } else if (rightData.toString().endsWith("TiB")) {
            rightTraffict = rightData.toString().replace("TiB", "").toFloat() * 1024 * 1024 * 1024 * 1024;
        } else if (rightData.toString().endsWith("PiB")) {
            rightTraffict = rightData.toString().replace("PiB", "").toFloat() * 1024 * 1024 * 1024 * 1024 * 1024;
        } else if (rightData.toString().endsWith("EiB")) {
            rightTraffict = rightData.toString().replace("EiB", "").toFloat() * 1024 * 1024 * 1024 * 1024 * 1024 * 1024;
        } else if (rightData.toString().endsWith("ZiB")) {
            rightTraffict = rightData.toString().replace("ZiB", "").toFloat() * 1024 * 1024 * 1024 * 1024 * 1024 * 1024 * 1024;
        } else if (rightData.toString().endsWith("YiB")) {
            rightTraffict = rightData.toString().replace("YiB", "").toFloat() * 1024 * 1024 * 1024 * 1024 * 1024 * 1024 * 1024 * 1024;
        } else if (rightData.toString().endsWith("B")) {
            rightTraffict = rightData.toString().replace("B", "").toFloat();
        }
        return leftTraffict < rightTraffict;
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

    return false;
}
