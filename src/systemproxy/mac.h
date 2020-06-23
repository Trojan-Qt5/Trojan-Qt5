#ifndef MAC_H
#define MAC_H

#include <QJsonArray>

class Mac {
public:
    Mac();
    ~Mac();

    static QJsonArray networkServicesList();
};

#endif // MAC_H
