#include "cmenu.h"
#include <DApplication>
CMenu::CMenu(QWidget *parent)
    : DMenu(parent)
{

}

CMenu::CMenu(const QString &title, QWidget *parent)
    : DMenu(title, parent)
{

}

void CMenu::hideEvent(QHideEvent *event)
{
    setActionEnableRecursive(this, true);
    DMenu::hideEvent(event);
}

void CMenu::setActionEnableRecursive(QMenu *pMenu, bool enable)
{
    //遍历菜单的action
    foreach (QAction *action, pMenu->actions()) {
        if (action->menu() != nullptr) {
            setActionEnableRecursive(action->menu(), enable);
        }
        action->setEnabled(enable);
    }
}

