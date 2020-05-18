#ifndef EVENTFILTER_H
#define EVENTFILTER_H

#include <QObject>
#include "mainwindow.h"

class EventFilter: public QObject
{
    Q_OBJECT

public:
    EventFilter(MainWindow *w);
    bool eventFilter(QObject* obj, QEvent* event);

signals:
    void handleData(const QString &);

private:
    MainWindow *window;
};

#endif // EVENTFILTER_H
