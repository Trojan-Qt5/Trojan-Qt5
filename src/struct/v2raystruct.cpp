#include "v2raystruct.h"

QDataStream& operator << (QDataStream &out, const tcpSettings &t)
{
    out << t.type << t.request << t.response;
    return out;
}

QDataStream& operator >> (QDataStream &in, tcpSettings &t)
{
    in >> t.type >> t.request >> t.response;
    return in;
}

QDataStream& operator << (QDataStream &out, const httpSettings &h)
{
    out << h.host << h.path;
    return out;
}

QDataStream& operator >> (QDataStream &in, httpSettings &h)
{
    in >> h.host >> h.path;
    return in;
}

QDataStream& operator << (QDataStream &out, const WsHeader &w)
{
    out << w.key << w.value;
    return out;
}

QDataStream& operator >> (QDataStream &in, WsHeader &w)
{
    in >> w.key >> w.value;
    return in;
}

QDataStream& operator << (QDataStream &out, const wsSettings &w)
{
    out << w.header << w.path;
    return out;
}

QDataStream& operator >> (QDataStream &in, wsSettings &w)
{
    in >> w.header >> w.path;
    return in;
}

QDataStream& operator << (QDataStream &out, const kcpSettings &k)
{
    out << k.mtu << k.tti << k.congestion << k.uplinkCapacity << k.downlinkCapacity << k.readBufferSize << k.writeBufferSize << k.type << k.seed;
    return out;
}

QDataStream& operator >> (QDataStream &in, kcpSettings &k)
{
    in >> k.mtu >> k.tti >> k.congestion >> k.uplinkCapacity >> k.downlinkCapacity >> k.readBufferSize >> k.writeBufferSize >> k.type >> k.seed;
    return in;
}

QDataStream& operator << (QDataStream &out, const quicSettings &q)
{
    out << q.security << q.key << q.type;
    return out;
}

QDataStream& operator >> (QDataStream &in, quicSettings &q)
{
    in >> q.security >> q.key >> q.type;
    return in;
}

QDataStream& operator << (QDataStream &out, const tlsSettings &t)
{
    out << t.enable << t.allowInsecure << t.allowInsecureCiphers << t.serverName << t.alpn;
    return out;
}

QDataStream& operator >> (QDataStream &in, tlsSettings &t)
{
    in >> t.enable >> t.allowInsecure >> t.allowInsecureCiphers >> t.serverName >> t.alpn;
    return in;
}

QDataStream& operator << (QDataStream &out, const vmuxSettings &v)
{
    out << v.enable << v.muxConcurrency;
    return out;
}

QDataStream& operator >> (QDataStream &in, vmuxSettings &v)
{
    in >> v.enable >> v.muxConcurrency;
    return in;
}

QDataStream& operator << (QDataStream &out, const VmessSettings &v)
{
    out << v.network << v.tcp << v.http << v.ws << v.kcp << v.quic << v.tls << v.mux;
    return out;
}

QDataStream& operator >> (QDataStream &in, VmessSettings &v)
{
    in >> v.network >> v.tcp >> v.http >> v.ws >> v.kcp >> v.quic >> v.tls >> v.mux;
    return in;
}
