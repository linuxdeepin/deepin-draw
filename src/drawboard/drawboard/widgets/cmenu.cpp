// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "cmenu.h"
CMenu::CMenu(QWidget *parent)
    : QMenu(parent)
{

}

CMenu::CMenu(const QString &title, QWidget *parent)
    : QMenu(title, parent)
{

}

void CMenu::hideEvent(QHideEvent *event)
{
    //删除setActionEnableRecursive(this, true)，当菜单再次弹出会影响其正确性
    QMenu::hideEvent(event);
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

