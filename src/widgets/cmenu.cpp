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

void CMenu::setVisible(bool visible)
{
    //解决快捷键失效的BUG
    if (!visible) {
        foreach (QAction *action, this->actions()) {
            action->setEnabled(true);
        }
    }
    DMenu::setVisible(visible);
}

void CMenu::enterEvent(QEvent *event)
{
    Q_UNUSED(event)
    //qApp->setOverrideCursor(QCursor(Qt::ArrowCursor));
    DMenu::enterEvent(event);
}
