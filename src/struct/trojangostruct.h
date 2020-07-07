#ifndef TROJANGOSTRUCT_H
#define TROJANGOSTRUCT_H

#include <QObject>
#include <QDataStream>

struct muxSettings {
    bool enable = false;
    int muxConcurrency = 8;
    int muxIdleTimeout = 60;
};

struct websocketSettings {
    bool enable = false;
    QString path;
    QString hostname;
};

struct shadowsocksSettings {
    bool enable = false;
    QString method = "aes-128-gcm";
    QString password;
};

struct transportPluginSettings{
    bool enable = false;
    QString type = "plaintext";
    QString command;
    QStringList arg;
    QStringList env;
    QString option;
};

struct TrojanGoSettings {
    muxSettings mux;
    websocketSettings websocket;
    shadowsocksSettings shadowsocks;
    transportPluginSettings transportPlugin;
};

QDataStream& operator << (QDataStream &out, const muxSettings &m);
QDataStream& operator >> (QDataStream &in, muxSettings &m);

QDataStream& operator << (QDataStream &out, const websocketSettings &w);
QDataStream& operator >> (QDataStream &in, websocketSettings &w);

QDataStream& operator << (QDataStream &out, const shadowsocksSettings &s);
QDataStream& operator >> (QDataStream &in, shadowsocksSettings &s);

QDataStream& operator << (QDataStream &out, const transportPluginSettings &t);
QDataStream& operator >> (QDataStream &in, transportPluginSettings &t);

QDataStream& operator << (QDataStream &out, const TrojanGoSettings &t);
QDataStream& operator >> (QDataStream &in, TrojanGoSettings &t);

#endif // TROJANGOSTRUCT_H
