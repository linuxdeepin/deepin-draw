#include "cmenu.h"
#include <DApplication>
CMenu::CMenu(QWidget *parent)
    : DMenu(parent)
{

}

CMenu::CMenu(const QString &title, QWidget *parent)
    : DMenu (title, parent)
{

}

void CMenu::enterEvent(QEvent *event)
{
    Q_UNUSED(event)
    //qApp->setOverrideCursor(QCursor(Qt::ArrowCursor));
    DMenu::enterEvent(event);
}
