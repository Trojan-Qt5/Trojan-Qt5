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
    static Connection* getStatusConnection();

private:
    static Connection *connection;
    static Connection *statusConnection;

};

#endif // MIDMAN_H
