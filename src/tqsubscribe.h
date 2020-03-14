#ifndef TQSUBSCRIBE_H
#define TQSUBSCRIBE_H

#include <QDataStream>

class TQSubscribe
{
public:
    TQSubscribe();

    QString url;
    QString groupName;
    quint64 lastUpdateTime;

};
Q_DECLARE_METATYPE(TQSubscribe)

QDataStream& operator << (QDataStream &out, const TQSubscribe &p);
QDataStream& operator >> (QDataStream &in, TQSubscribe &p);

#endif // TQSUBSCRIBE_H
