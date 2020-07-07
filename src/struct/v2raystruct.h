#ifndef V2RAYSTRUCT_H
#define V2RAYSTRUCT_H

#include <QObject>
#include <QDataStream>

struct tcpSettings {
    QString type = "none";
    QString request = "{\n    \"version\": \"1.1\",\n    \"method\": \"GET\",\n    \"path\": [],\n    \"headers\": {}\n}";
    QString response = "{\n    \"version\": \"1.1\",\n    \"status\": \"200\",\n    \"reason\": \"OK\",\n    \"headers\": {}\n}";
};

struct httpSettings {
    QStringList host;
    QString path;
};

struct WsHeader {
    QString key;
    QString value;
};

struct wsSettings {
    QList<WsHeader> header;
    QString path;
};

struct kcpSettings {
    int mtu = 1460;
    int tti = 20;
    int uplinkCapacity = 5;
    bool congestion = false;
    int downlinkCapacity = 20;
    int readBufferSize = 1;
    int writeBufferSize = 1;
    QString type;
    QString seed;
};

struct quicSettings {
    QString security;
    QString key;
    QString type;
};

struct tlsSettings {
    bool enable = false;
    bool allowInsecure = false;
    bool allowInsecureCiphers = false;
    QString serverName;
    QStringList alpn;
};

struct vmuxSettings {
    bool enable = false;
    int muxConcurrency = 8;
};

struct VmessSettings {
    QString network = "tcp";
    tcpSettings tcp;
    httpSettings http;
    wsSettings ws;
    kcpSettings kcp;
    quicSettings quic;
    tlsSettings tls;
    vmuxSettings mux;
};

QDataStream& operator << (QDataStream &out, const tcpSettings &t);
QDataStream& operator >> (QDataStream &in, tcpSettings &t);

QDataStream& operator << (QDataStream &out, const httpSettings &h);
QDataStream& operator >> (QDataStream &in, httpSettings &h);

QDataStream& operator << (QDataStream &out, const WsHeader &w);
QDataStream& operator >> (QDataStream &in, WsHeader &w);

QDataStream& operator << (QDataStream &out, const wsSettings &w);
QDataStream& operator >> (QDataStream &in, wsSettings &w);

QDataStream& operator << (QDataStream &out, const kcpSettings &k);
QDataStream& operator >> (QDataStream &in, kcpSettings &k);

QDataStream& operator << (QDataStream &out, const quicSettings &q);
QDataStream& operator >> (QDataStream &in, quicSettings &q);

QDataStream& operator << (QDataStream &out, const tlsSettings &t);
QDataStream& operator >> (QDataStream &in, tlsSettings &t);

QDataStream& operator << (QDataStream &out, const vmuxSettings &v);
QDataStream& operator >> (QDataStream &in, vmuxSettings &v);

QDataStream& operator << (QDataStream &out, const VmessSettings &v);
QDataStream& operator >> (QDataStream &in, VmessSettings &v);

#endif // V2RAYSTRUCT_H
