#include "trojangostruct.h"

QDataStream& operator << (QDataStream &out, const muxSettings &m)
{
    out << m.enable << m.muxConcurrency << m.muxIdleTimeout;
    return out;
}

QDataStream& operator >> (QDataStream &in, muxSettings &m)
{
    in >> m.enable >> m.muxConcurrency >> m.muxIdleTimeout;
    return in;
}

QDataStream& operator << (QDataStream &out, const websocketSettings &w)
{
    out << w.enable << w.path << w.hostname;
    return out;
}

QDataStream& operator >> (QDataStream &in, websocketSettings &w)
{
    in >> w.enable >> w.path >> w.hostname;
    return in;
}

QDataStream& operator << (QDataStream &out, const shadowsocksSettings &s)
{
    out << s.enable << s.method << s.password;
    return out;
}

QDataStream& operator >> (QDataStream &in, shadowsocksSettings &s)
{
    in >> s.enable >> s.method >> s.password;
    return in;
}

QDataStream& operator << (QDataStream &out, const transportPluginSettings &t)
{
    out << t.enable << t.type << t.command << t.arg << t.env << t.option;
    return out;
}

QDataStream& operator >> (QDataStream &in, transportPluginSettings &t)
{
    in >> t.enable >> t.type >> t.command >> t.arg >> t.env >> t.option;
    return in;
}

QDataStream& operator << (QDataStream &out, const TrojanGoSettings &t)
{
    out << t.mux << t.websocket << t.shadowsocks << t.transportPlugin;
    return out;
}

QDataStream& operator >> (QDataStream &in, TrojanGoSettings &t)
{
    in >> t.mux >> t.websocket >> t.shadowsocks >> t.transportPlugin;
    return in;
}
