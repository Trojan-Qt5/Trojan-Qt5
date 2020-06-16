#include "eventfilter.h"

#include <QFileOpenEvent>

EventFilter::EventFilter(MainWindow *w) : window(w)
{
    connect(this, &EventFilter::handleData, window, &MainWindow::onHandleDataFromUrlScheme);
}

bool EventFilter::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::FileOpen)
    {
        QFileOpenEvent* fileEvent = static_cast<QFileOpenEvent*>(event);
        if (!fileEvent->url().isEmpty())
        {
            QString data = fileEvent->url().toString();
            emit handleData(data);
        }

        return false;

    } else {
        // standard event processing
        return QObject::eventFilter(obj, event);
    }
}
