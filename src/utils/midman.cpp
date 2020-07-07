#include "midman.h"

MidMan::MidMan()
{}

Connection* MidMan::connection = new Connection();

Connection* MidMan::statusConnection = new Connection(TQProfile());

MidMan::~MidMan()
{
    MidMan::connection = nullptr;
    MidMan::statusConnection = nullptr;
    delete MidMan::connection;
    delete MidMan::statusConnection;
}

void MidMan::setConnection(Connection *con)
{
    MidMan::connection = con;
}

Connection& MidMan::getConnection()
{
    return *MidMan::connection;
}

Connection *MidMan::getStatusConnection()
{
    return MidMan::statusConnection;
}
