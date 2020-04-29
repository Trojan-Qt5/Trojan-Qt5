#ifndef PRIVILEGESHELPER_H
#define PRIVILEGESHELPER_H

#include <QObject>

class PrivilegesHelper: public QObject
{
    Q_OBJECT

public:
    PrivilegesHelper();

    static bool checkPrivileges();

    static void showWarning();
};

#endif // PRIVILEGESHELPER_H
