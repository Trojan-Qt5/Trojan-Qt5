#include "clickablelabel.h"

#include <QUrl>
#include <QDesktopServices>

ClickableLabel::ClickableLabel(QWidget *parent) : QLabel(parent)
{

}

void ClickableLabel::setUrl(const QString &u)
{
    url = u;
}

void ClickableLabel::mousePressEvent(QMouseEvent *event)
{
    QDesktopServices::openUrl(QUrl(url));
}
