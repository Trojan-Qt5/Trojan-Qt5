#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QObject>
#include <QLabel>
#include <QMouseEvent>

class ClickableLabel: public QLabel
{
    Q_OBJECT
public:
    ClickableLabel(QWidget *parent = 0);
    void setUrl(const QString&);

private:
   void mousePressEvent(QMouseEvent* event);
   QString url;
};

#endif // CLICKABLELABEL_H
