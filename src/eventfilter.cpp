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
            // Not call directly .toString to avoid getting strange data like: clash://0.1.226.63
            QString data = QString::fromLatin1(fileEvent->url().toEncoded().data());
            emit handleData(data);
        }

        return false;

    } else {
        // standard event processing
        return QObject::eventFilter(obj, event);
    }
}
