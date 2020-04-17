#ifndef MIDMAN_H
#define MIDMAN_H

#include "connection.h"

class MidMan
{

public:
    MidMan();
    ~MidMan();

    static void setConnection(Connection*);
    static Connection& getConnection();

private:
    static Connection *connection;

};

#endif // MIDMAN_H
