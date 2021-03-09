/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     Ji XiangLong <jixianglong@uniontech.com>
 *
 * Maintainer: WangYu <wangyu@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "groupoperation.h"
#include "expansionpanel.h"
#include "cdrawscene.h"
#include "cgraphicsitemselectedmgr.h"
#include "cviewmanagement.h"
#include "cgraphicsview.h"

#include <DIconButton>
#include <DWidget>
#include <QHBoxLayout>
#include <application.h>

GroupOperation::GroupOperation(QWidget *parent)
    : QWidget(parent)
{
    //下拉按钮
    openGroup = new DIconButton(nullptr);
    openGroup->setIcon(QIcon::fromTheme("icon_open_normal"));
    drawApp->setWidgetAccesibleName(openGroup, "openGroup");
    openGroup->setFixedSize(36, 36);
    openGroup->setIconSize(QSize(30, 30));
    openGroup->setContentsMargins(0, 0, 0, 0);
    //组合按钮
    groupButton = new DIconButton(nullptr);
    groupButton->setIcon(QIcon::fromTheme("menu_group_normal"));
    drawApp->setWidgetAccesibleName(groupButton, "groupButton");
    groupButton->setFixedSize(36, 36);
    groupButton->setIconSize(QSize(20, 20));
    groupButton->setContentsMargins(0, 0, 0, 0);
    //释放组合按钮
    unGroupButton = new DIconButton(nullptr);
    unGroupButton->setIcon(QIcon::fromTheme("menu_ungroup_normal"));
    drawApp->setWidgetAccesibleName(unGroupButton, "unGroupButton");
    unGroupButton->setFixedSize(36, 36);
    unGroupButton->setIconSize(QSize(20, 20));
    unGroupButton->setContentsMargins(0, 0, 0, 0);

    //分割线
    sepLine = new SeperatorLine(nullptr);

    //设置布局管理
    setLayout(getLayout());

    connect(groupButton, &DIconButton::clicked, this, &GroupOperation::creatGroupButton);
    connect(unGroupButton, &DIconButton::clicked, this, &GroupOperation::cancelGroupButton);

    //dApp的事件，本控件查看过滤
    dApp->installEventFilter(this);
}

void GroupOperation::setMode(bool mode, bool line)
{
    QHBoxLayout *layout = getLayout();
    //获取当前场景的scene
    CDrawScene *currScene = CManageViewSigleton::GetInstance()->getCurView()->drawScene();
    groupButton->setEnabled(currScene->isGroupable());
    unGroupButton->setEnabled(currScene->isUnGroupable());

    //分割线
    if (line) {
        layout->addWidget(sepLine);
        sepLine->show();
    } else {
        sepLine->hide();
    }

    //根据显示模式，显示不同的UI布局
    if (mode) {
        layout->addWidget(groupButton);
        layout->addSpacing(5);
        layout->addWidget(unGroupButton);

        groupButton->show();
        unGroupButton->show();
    } else {
        layout->addWidget(openGroup);
        openGroup->show();
    }
}

void GroupOperation::creatGroupButton()
{
    // 进行图元组合
    CDrawScene *currScene = CManageViewSigleton::GetInstance()->getCurView()->drawScene();
    if (currScene != nullptr) {
        CGraphicsItem *pBaseItem = currScene->selectGroup()->getLogicFirst();
        currScene->creatGroup(QList<CGraphicsItem *>(), CGraphicsItemGroup::ENormalGroup,
                              true, pBaseItem, true);
    }
}

void GroupOperation::cancelGroupButton()
{
    //进行组合图元释放
    CDrawScene *currScene = CManageViewSigleton::GetInstance()->getCurView()->drawScene();
    if (currScene != nullptr) {
        currScene->cancelGroup(nullptr, true);
    }
}

void GroupOperation::showExpansionPanel()
{
    //扩展面板显示时，再次点击隐藏
    if (getExpansionPanel()->isVisible()) {
        getExpansionPanel()->hide();
    } else {
        //计算文字的下拉菜单的显示位置
        QPoint btnPos = openGroup->mapToGlobal(QPoint(0, 0));
        QPoint pos(btnPos.x() + openGroup->width() + 188,
                   btnPos.y() + openGroup->height());

        QPoint movPos = this->parentWidget()->mapFromGlobal(pos);
        getExpansionPanel()->move(movPos);
        getExpansionPanel()->show();

        CDrawScene *currScene = CManageViewSigleton::GetInstance()->getCurView()->drawScene();
        getExpansionPanel()->changeTheme();
        getExpansionPanel()->getGroupButton()->setEnabled(currScene->isGroupable());
        getExpansionPanel()->getUngroupButton()->setEnabled(currScene->isUnGroupable());
    }
}

void GroupOperation::clearUi()
{
    //清空UI布局
    openGroup->hide();
    groupButton->hide();
    unGroupButton->hide();
    getExpansionPanel()->hide();

    //清理原先的布局内的控件
    QHBoxLayout *pLay = getLayout();
    for (int i = 0; i < pLay->count();) {
        pLay->takeAt(i);
    }
}

QHBoxLayout *GroupOperation::getLayout()
{
    //返回布局管理器
    if (layout == nullptr) {
        layout = new QHBoxLayout;
    }
    return  layout;
}

ExpansionPanel *GroupOperation::getExpansionPanel()
{
    //创建属性栏下拉菜单的控件
    if (panel == nullptr) {
        panel = new ExpansionPanel(drawApp->topMainWindowWidget());
        panel->setFixedSize(182, 99);

        //扩展面板
        connect(openGroup, &DIconButton::clicked, this, &GroupOperation::showExpansionPanel);

        //组合和释放组合
        connect(panel, &ExpansionPanel::signalItemGroup, this, &GroupOperation::creatGroupButton);
        connect(panel, &ExpansionPanel::signalItemgUngroup, this, &GroupOperation::cancelGroupButton);

    }
    return  panel;
}

bool GroupOperation::eventFilter(QObject *o, QEvent *e)
{
    //进行事件过滤，实现窗口效果和主菜单一致
    if (panel != nullptr && !panel->isHidden() && o->isWidgetType()) {
        if (e->type() == QEvent::MouseButtonPress || e->type() == QEvent::TouchBegin) {
            panel->hide();
            if (o == openGroup) {
                return true;
            }
        }
    }
    return QWidget::eventFilter(o, e);
}

