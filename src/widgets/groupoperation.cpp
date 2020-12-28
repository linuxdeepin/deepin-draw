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
    openGroup->setObjectName("openGroup");
    openGroup->setFixedSize(36, 36);
    openGroup->setIconSize(QSize(30, 30));
    openGroup->setContentsMargins(0, 0, 0, 0);
    //组合按钮
    groupButton = new DIconButton(nullptr);
    groupButton->setIcon(QIcon::fromTheme("menu_group_normal"));
    groupButton->setObjectName("openGroup");
    groupButton->setFixedSize(36, 36);
    groupButton->setIconSize(QSize(20, 20));
    groupButton->setContentsMargins(0, 0, 0, 0);
    //释放组合按钮
    unGroupButton = new DIconButton(nullptr);
    unGroupButton->setIcon(QIcon::fromTheme("menu_ungroup_normal"));
    unGroupButton->setObjectName("openGroup");
    unGroupButton->setFixedSize(36, 36);
    unGroupButton->setIconSize(QSize(20, 20));
    unGroupButton->setContentsMargins(0, 0, 0, 0);

    //设置布局管理
    setLayout(getLayout());

    connect(groupButton, &DIconButton::clicked, this, &GroupOperation::creatGroupButton);
    connect(unGroupButton, &DIconButton::clicked, this, &GroupOperation::cancelGroupButton);

    dApp->installEventFilter(this);
}

void GroupOperation::setMode(bool mode)
{
    QHBoxLayout *layout = getLayout();

    CDrawScene *currScene = CManageViewSigleton::GetInstance()->getCurView()->drawScene();
    groupButton->setEnabled(currScene->isGroupable());
    unGroupButton->setEnabled(currScene->isUnGroupable());

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
    CDrawScene *currScene = CManageViewSigleton::GetInstance()->getCurView()->drawScene();
    if (currScene != nullptr) {
        CGraphicsItem *pBaseItem = currScene->selectGroup()->getLogicFirst();
        currScene->creatGroup(QList<CGraphicsItem *>(), CGraphicsItemGroup::ENormalGroup,
                              true, pBaseItem, true);
    }
}

void GroupOperation::cancelGroupButton()
{
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

        QPoint btnPos = openGroup->mapToGlobal(QPoint(0, 0));
        QPoint pos(btnPos.x() + openGroup->width() + 188,
                   btnPos.y() + openGroup->height());

        QPoint movPos = this->parentWidget()->mapFromGlobal(pos);
        getExpansionPanel()->move(movPos);
        getExpansionPanel()->show();

        CDrawScene *currScene = CManageViewSigleton::GetInstance()->getCurView()->drawScene();
        getExpansionPanel()->getGroupButton()->setEnabled(currScene->isGroupable());
        getExpansionPanel()->getUngroupButton()->setEnabled(currScene->isUnGroupable());
    }
}

void GroupOperation::clearUi()
{
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
    if (layout == nullptr) {
        layout = new QHBoxLayout;
    }
    return  layout;
}

ExpansionPanel *GroupOperation::getExpansionPanel()
{
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

