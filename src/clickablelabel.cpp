#include "clickablelabel.h"

#include <QUrl>
#include <QDesktopServices>

ClickableLabel::ClickableLabel(QWidget *parent) : QLabel(parent)
{

}

void ClickableLabel::mousePressEvent(QMouseEvent *event)
{
    QDesktopServices::openUrl(QUrl("https://rakuten-co-jp.club/register?aff=COELWU"));
}
