#include "tqsubscribe.h"

TQSubscribe::TQSubscribe()
{
    url = "https://subscribe.example.com/trojan.txt";
    lastUpdateTime = 0;
}

QDataStream& operator << (QDataStream &out, const TQSubscribe &s)
{
    out << s.url << s.groupName << s.lastUpdateTime;
    return out;
}

QDataStream& operator >> (QDataStream &in, TQSubscribe &s)
{
    in >> s.url >> s.groupName >> s.lastUpdateTime;
    return in;
}
