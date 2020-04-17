#include "midman.h"

MidMan::MidMan()
{}

MidMan::~MidMan()
{
    delete MidMan::connection;
    MidMan::connection = nullptr;
}

Connection* MidMan::connection = new Connection();

void MidMan::setConnection(Connection *con)
{
    connection = con;
}

Connection& MidMan::getConnection()
{
    return *connection;
}
