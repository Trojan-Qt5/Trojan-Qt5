#include "connectiontablemodel.h"
#include "confighelper.h"

ConnectionTableModel::ConnectionTableModel(QObject *parent) :
    QAbstractTableModel(parent)
{}

ConnectionTableModel::~ConnectionTableModel()
{}

ConnectionItem *ConnectionTableModel::getItem(const int &row) const
{
    return items.at(row);
}

int ConnectionTableModel::rowCount(const QModelIndex &) const
{
    return items.count();
}

int ConnectionTableModel::columnCount(const QModelIndex &) const
{
    return ConnectionItem::columnCount();
}

QVariant ConnectionTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    ConnectionItem *item = getItem(index.row());
    return item->data(index.column(), role);
}

QVariant ConnectionTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Vertical || role != Qt::DisplayRole) {
        return QVariant();
    }

    switch (section) {
    case 0:
        return QVariant(tr("Name"));
    case 1:
        return QVariant(tr("Type"));
    case 2:
        return QVariant(tr("Server"));
    case 3:
        return QVariant(tr("Status"));
    case 4:
        return QVariant(tr("Latency"));
    case 5:
        return QVariant(tr("Term Usage"));
    case 6:
        return QVariant(tr("Total Usage"));
    case 7:
        return QVariant(tr("Reset Date"));
    case 8:
        return QVariant(tr("Last Used"));
    default:
        return QVariant();
    }
}

QModelIndex ConnectionTableModel::index(int row, int column, const QModelIndex &) const
{
    if (row < 0 || row >= items.size()) {
        return QModelIndex();
    } else {
        ConnectionItem* item = items.at(row);
        return createIndex(row, column, item);//column is ignored (all columns have the same effect)
    }
}

bool ConnectionTableModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (row < 0 || count <= 0 || count + row > items.count()) {
        return false;
    }
    beginRemoveRows(parent, row, row + count - 1);
    items.erase(items.begin() + row, items.begin() + row + count);
    endRemoveRows();
    return true;
}

bool ConnectionTableModel::move(int row, int target, const QModelIndex &parent)
{
    if (row < 0 || row >= rowCount() || target < 0 || target >= rowCount() || row == target) {
        return false;
    }

    //http://doc.qt.io/qt-5/qabstractitemmodel.html#beginMoveRows
    int movTarget = target;
    if (target - row > 0) {
        movTarget++;
    }
    beginMoveRows(parent, row, row, parent, movTarget);
    items.move(row, target);
    endMoveRows();
    return true;
}

bool ConnectionTableModel::appendConnection(Connection *con, const QModelIndex &parent)
{
    ConnectionItem* newItem = new ConnectionItem(con, this);
    connect(newItem, &ConnectionItem::message, this, &ConnectionTableModel::message);
    connect(newItem, &ConnectionItem::stateChanged, this, &ConnectionTableModel::onConnectionStateChanged);
    connect(newItem, &ConnectionItem::latencyChanged, this, &ConnectionTableModel::onConnectionLatencyChanged);
    connect(newItem, &ConnectionItem::dataUsageChanged, this, &ConnectionTableModel::onConnectionDataUsageChanged);
    beginInsertRows(parent, items.count(), items.count());
    items.append(newItem);
    endInsertRows();
    return true;
}

void ConnectionTableModel::disconnectConnections()
{
    for (auto &i : items) {
        Connection *con = i->getConnection();
        if (con->isRunning()) {
            con->stop();
        }
    }
}

void ConnectionTableModel::connectConnections(TQProfile profile)
{
    for (auto &i : items) {
        Connection *con = i->getConnection();
        if (con->isValid()) {
            if (con->getProfile().equals(profile)) {
                disconnectConnections();
                con->start();
            }
        }
    }
}

/**
 * @brief ConnectionTableModel::isDuplicate
 * @ref https://github.com/qinyuhang/ShadowsocksX-NG-R/blob/81acc0cfeb514ab1f7eaad858e364144a2735a9e/ShadowsocksX-NG/ServerProfileManager.swift#L105-L120
 * @param newCon connection
 * @return duplicate or not
 */
bool ConnectionTableModel::isDuplicated(Connection *newCon)
{
    for (auto &i : items) {
        Connection *con = i->getConnection();
        if (con->getProfile().serverAddress == newCon->getProfile().serverAddress
                && con->getProfile().serverPort == newCon->getProfile().serverPort
                && con->getProfile().password == newCon->getProfile().password
                && con->getProfile().name == newCon->getProfile().name)
            return true;
    }
    return false;
}

/**
 * @brief ConnectionTableModel::isExisted
 * @ref https://github.com/qinyuhang/ShadowsocksX-NG-R/blob/81acc0cfeb514ab1f7eaad858e364144a2735a9e/ShadowsocksX-NG/ServerProfileManager.swift#L95-L103
 * @param newCon connection
 * @return exist or not
 */
bool ConnectionTableModel::isExisted(Connection *newCon)
{
    for (auto &i : items) {
        Connection *con = i->getConnection();
        if (con->getProfile().serverAddress == newCon->getProfile().serverAddress &&
            con->getProfile().serverPort == newCon->getProfile().serverPort &&
            ConfigHelper::exportVmessSettings(con->getProfile().vmessSettings) == ConfigHelper::exportVmessSettings(newCon->getProfile().vmessSettings))
            return true;
    }
    return false;
}

void ConnectionTableModel::replace(Connection *newCon)
{
    for (auto &i : items) {
        Connection *con = i->getConnection();
        if (con->getProfile().serverAddress == newCon->getProfile().serverAddress &&
            con->getProfile().serverPort == newCon->getProfile().serverPort) {
            TQProfile p;
            p.type = newCon->getProfile().type;
            p.serverAddress = newCon->getProfile().serverAddress;
            p.serverPort = newCon->getProfile().serverPort;
            p.password = newCon->getProfile().password;
            p.method = newCon->getProfile().method;
            p.uuid = newCon->getProfile().uuid;
            p.alterID = newCon->getProfile().alterID;
            p.protocol = newCon->getProfile().method;
            p.protocolParam = newCon->getProfile().protocolParam;
            p.obfs = newCon->getProfile().obfs;
            p.obfsParam = newCon->getProfile().obfsParam;
            p.name = newCon->getProfile().name;
            p.vmessSettings = newCon->getProfile().vmessSettings;
            p.group = newCon->getProfile().group;
            con->setProfile(p);
        }

    }
}

void ConnectionTableModel::testAllLatency()
{
    for (auto &i : items) {
        i->testLatency();
    }
}

QList<TQProfile> ConnectionTableModel::getAllServers()
{
    QList<TQProfile> servers;

    for (auto &i : items) {
        servers.append(i->getConnection()->getProfile());
    }

    return servers;
}

TQProfile ConnectionTableModel::getConnectedServer()
{
    for (auto &i : items) {
        Connection *con = i->getConnection();
        if (con->isRunning()) {
            return con->getProfile();
        }
    }

    return TQProfile();
}

void ConnectionTableModel::onConnectionStateChanged(bool running)
{
    ConnectionItem *item = qobject_cast<ConnectionItem*>(sender());
    int row = items.indexOf(item);
    emit dataChanged(this->index(row, 0),
                     this->index(row, ConnectionItem::columnCount() - 1));
    emit rowStatusChanged(row, running);
    emit changeIcon(running);
}

void ConnectionTableModel::onConnectionLatencyChanged()
{
    ConnectionItem *item = qobject_cast<ConnectionItem*>(sender());
    int row = items.indexOf(item);
    emit dataChanged(this->index(row, 3), this->index(row, 3));
}

void ConnectionTableModel::onConnectionDataUsageChanged()
{
    ConnectionItem *item = qobject_cast<ConnectionItem*>(sender());
    int row = items.indexOf(item);
    emit dataChanged(this->index(row, 4), this->index(row, 5));
}
