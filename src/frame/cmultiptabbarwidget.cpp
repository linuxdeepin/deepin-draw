/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     Zhang Hao<zhanghao@uniontech.com>
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
#include "cmultiptabbarwidget.h"

#include <QMouseEvent>
#include <QApplication>
#include <QStyleFactory>
#include <QGuiApplication>
#include <QDebug>
#include "drawshape/cdrawparamsigleton.h"
#include "cviewmanagement.h"
#include "application.h"

const QSize TabBarMiniSize = QSize(220, 36);

CMultipTabBarWidget::CMultipTabBarWidget(QWidget *parent)
    : DTabBar(parent)
    , m_tabbarWidth(1360)
    , m_defaultName("Unnamed")
    , m_nameCounter(1)
{
    drawApp->setWidgetAccesibleName(this, "MultipTabBarWidget");
    this->setMovable(true);
    this->setTabsClosable(true);
    this->setVisibleAddButton(true);
    this->setElideMode(Qt::ElideMiddle);
    this->setFocusPolicy(Qt::NoFocus);
    this->setExpanding(true);
    this->setEnabledEmbedStyle(true);

    m_rightClickTab = -1;
    installEventFilter(this);


    initConnection();
}

CMultipTabBarWidget::~CMultipTabBarWidget()
{

}

//void CMultipTabBarWidget::closeTabBarItem(QString itemName)
//{
//    for (int i = 0; i < this->count(); i++) {
//        if (itemName == this->tabText(i)) {
//            this->removeTab(i);
//        }
//    }
//}

void CMultipTabBarWidget::closeTabBarItemByUUID(QString uuid)
{
    Q_UNUSED(uuid)
    for (int i = 0; i < this->count(); i++) {
        if (uuid == this->tabData(i).toString()) {
            this->removeTab(i);
        }
    }
}

void CMultipTabBarWidget::setDefaultTabBarName(const QString &name)
{
    m_defaultName = name;
}

void CMultipTabBarWidget::addTabBarItem(QString name, const QString &uuid, bool emitNewScene)
{
    if (name.isEmpty()) {
        emit this->tabAddRequested();
        return;
    }

    // 添加一个标签页之前如果是空的那么要再次发送页面变化的信号(以保证uuid能获取从而获得到正确的view场景，比如程序刚启动初始化的情形)
    bool isEmptyBeforAdded = (this->count() == 0);

    int index = this->addTab(name);

    setTabData(index, uuid);

    //设置悬停提示消息
    setTabBarTooltipName(uuid, name);

    if (emitNewScene)
        emit signalNewAddItem(name, uuid);

    //保证uuid能获取到
    if (isEmptyBeforAdded) {
        emit currentChanged(index);
    }

    this->setCurrentIndex(index);
    this->setTabMinimumSize(index, TabBarMiniSize);
}

void CMultipTabBarWidget::initConnection()
{
    // [0] 连接点击 + 按钮响应事件
    connect(this, &DTabBar::tabAddRequested, this, [ = ]() {
        // 组装标签名字
        m_nameCounter++;
        QString divName = m_defaultName + QString::number(m_nameCounter);

//        // 如果已经存在标签则显示存在的标签
//        if (tabBarNameIsExist(divName)) {
//            m_nameCounter--;
//            return ;
//        }

        QString uuid = CDrawParamSigleton::creatUUID();
        addTabBarItem(divName, uuid);
    });

    // [1] 连接点击关闭响应事件
    connect(this, &DTabBar::tabCloseRequested, this, [ = ](int index) {
        QString closeItemName = this->tabText(index);
        QString closeItemUUID = this->tabData(index).toString();
        emit signalTabItemCloseRequested(closeItemName, closeItemUUID);
    });

    // [2] 连接标签选择改变响应事件
    connect(this, &DTabBar::currentChanged, this, [ = ](int index) {
        QString currentName = this->tabText(index);
        QString closeItemUUID = this->tabData(index).toString();
        emit signalItemChanged(currentName, closeItemUUID);
    });

    // [3] 连接新增加标签页设置到当前新增加标签
    connect(this, &DTabBar::tabIsInserted, this, [ = ](int index) {
        this->setCurrentIndex(index);
    }, Qt::QueuedConnection);
}

//bool CMultipTabBarWidget::tabBarNameIsExist(QString name)
//{
//    for (int i = 0; i < this->count(); i++) {
//        QString tempName = this->tabText(i);
//        // 此判断用于文件是否已经修改
//        if (tempName.startsWith("* ") && tempName.endsWith(" ")) {
//            tempName = tempName.replace(0, 2, "");
//            tempName = tempName.replace(tempName.lastIndexOf(" "), 1, "");
//        }

//        if (tempName == name) {
//            this->setCurrentIndex(i);
//            return true;
//        }
//    }
//    return false;
//}

bool CMultipTabBarWidget::IsFileOpened(QString file)
{
    return CManageViewSigleton::GetInstance()->isDdfFileOpened(file);
}

void CMultipTabBarWidget::updateTabBarName(const QString &uuid, QString newName)
{
    for (int i = 0; i < this->count(); i++) {
        if (this->tabData(i).toString() == uuid) {
            this->setTabText(i, newName);
            return;
        }
    }
}

//QString CMultipTabBarWidget::getNextTabBarDefaultName()
//{
//    m_nameCounter++;
//    QString divName = m_defaultName + QString::number(m_nameCounter);
//    m_nameCounter--;
//    return divName;
//}

void CMultipTabBarWidget::setTabBarTooltipName(const QString &uuid, QString tooltip)
{
    for (int i = 0; i < this->count(); i++) {
        if (uuid == this->tabData(i).toString()) {
            setTabBarTooltipName(static_cast<quint16>(i), tooltip);
        }
    }
}

void CMultipTabBarWidget::setTabBarTooltipName(quint16 index, QString tooltip)
{
    this->setTabToolTip(index, tooltip);
}

void CMultipTabBarWidget::setCurrentTabBarWithName(const QString &tabName)
{
    for (int i = 0; i < this->count(); i++) {
        if (tabName == this->tabText(i)) {
            this->setCurrentIndex(i);
        }
    }
}

void CMultipTabBarWidget::setCurrentTabBarWithUUID(const QString &uuid)
{
    for (int i = 0; i < this->count(); i++) {
        if (uuid == this->tabData(i).toString()) {
            this->setCurrentIndex(i);
        }
    }
}

//QString CMultipTabBarWidget::getCurrentTabBarName()
//{
//    return this->tabText(this->currentIndex());
//}

//QString CMultipTabBarWidget::getCurrentTabBarUUID()
//{
//    return this->tabData(this->currentIndex()).toString();
//}

QStringList CMultipTabBarWidget::getAllTabBarName()
{
    QStringList names;
    for (int i = 0; i < this->count(); i++) {
        names << this->tabText(i);
    }
    return names;
}

QStringList CMultipTabBarWidget::getAllTabBarUUID()
{
    QStringList uuids;
    for (int i = 0; i < this->count(); i++) {
        uuids << this->tabData(i).toString();
    }
    return uuids;
}

void CMultipTabBarWidget::clearHoverState()
{
    QHoverEvent hoverE(QEvent::HoverLeave, QPointF(-1, -1), QPointF(-1, -1));
    QWidget *tab = this->childAt(this->rect().center());
    if (tab != nullptr)
        qApp->sendEvent(tab, &hoverE);
}

bool CMultipTabBarWidget::eventFilter(QObject *o, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);

        if (mouseEvent->button() == Qt::RightButton) {
            QPoint position = mouseEvent->pos();
            m_rightClickTab = -1;

            // 判断当前是否找到该标签
            for (int i = 0; i < count(); i++) {
                if (this->tabRect(i).contains(position)) {
                    m_rightClickTab = i;
                    break;
                }
            }

            // popup right menu on tab.
            if (m_rightClickTab >= 0) {
                m_rightMenu = new DMenu;

                m_closeTabAction = new QAction(tr("Close tab"), this);
                m_closeOtherTabAction = new QAction(tr("Close other tabs"), this);

                connect(m_closeTabAction, &QAction::triggered, this, [ = ] {
                    // 跳转到需要关闭的标签
                    this->setCurrentIndex(m_rightClickTab);

                    emit signalTabItemCloseRequested(this->tabText(m_rightClickTab), this->tabData(m_rightClickTab).toString());
                });

                connect(m_closeOtherTabAction, &QAction::triggered, this, [ = ] {
                    QStringList closeNames;
                    QStringList closeUUIDs;
                    for (int i = 0; i < this->count(); i++)
                    {
                        if (m_rightClickTab == i) {
                            continue;
                        }
                        closeNames << this->tabText(i);
                        closeUUIDs << this->tabData(i).toString();
                    }
                    emit signalTabItemsCloseRequested(closeNames, closeUUIDs);
                });

                m_rightMenu->addAction(m_closeTabAction);
                m_rightMenu->addAction(m_closeOtherTabAction);

                m_rightMenu->exec(mapToGlobal(position));

                //return true;
            }
        }
    } /*else {
        event->accept();
    }*/
    //return false;
    return DTabBar::eventFilter(o, event);
}

