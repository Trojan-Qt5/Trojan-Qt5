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

private:
   void mousePressEvent(QMouseEvent* event);
};

#endif // CLICKABLELABEL_H
