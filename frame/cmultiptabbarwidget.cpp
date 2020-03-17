/*
* Copyright (C) 2019 ~ 2020 Deepin Technology Co., Ltd.
*
* Author: Zhang Hao<zhanghao@uniontech.com>
*
* Maintainer: Zhang Hao <zhanghao@uniontech.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include "cmultiptabbarwidget.h"

#include <QMouseEvent>
#include <QApplication>
#include <QStyleFactory>
#include <QGuiApplication>
#include <QDebug>

CMultipTabBarWidget::CMultipTabBarWidget(QWidget *parent)
    : DTabBar(parent)
    , m_tabbarWidth(1360)
    , m_defaultName("Unnamed")
    , m_nameCounter(1)
{
    this->setMovable(true);
    this->setTabsClosable(true);
    this->setVisibleAddButton(true);
    this->setElideMode(Qt::ElideMiddle);
    this->setFocusPolicy(Qt::NoFocus);
    this->setExpanding(true);
//    this->setUsesScrollButtons(true);

    m_rightClickTab = -1;
    installEventFilter(this);


    initConnection();
}

CMultipTabBarWidget::~CMultipTabBarWidget()
{

}

void CMultipTabBarWidget::closeTabBarItem(QString itemName)
{
    for (int i=0; i < this->count(); i++) {
        if(itemName == this->tabText(i)){
            this->removeTab(i);
        }
    }
}

void CMultipTabBarWidget::setDefaultTabBarName(QString name)
{
    m_defaultName = name;
}

void CMultipTabBarWidget::addTabBarItem(QString name)
{
    if(name.isEmpty()){
        emit this->tabAddRequested();
        return;
    }

    this->addTab(name);

    emit signalNewAddItem(name);
    updateTabWidth();

    for(int i = 0; i < this->count(); i++){
        if(name == this->tabText(i)){
            this->setCurrentIndex(i);
        }
    }
}

void CMultipTabBarWidget::updateTabWidth()
{
    int tabWidth;
    if (count() != 0) {
        tabWidth = (width() - 40) / count();
        for (int i = 0; i < count(); i++) {
            if (tabWidth < 140) {
                setUsesScrollButtons(true);
                // 此处设置最小高度为37是为了能够在resize的时候进行重绘
                setTabMinimumSize(i, QSize(140, 37));
            } else {
                setUsesScrollButtons(false);
                setTabMinimumSize(i, QSize(tabWidth, 37));
            }
        }
    }
}

void CMultipTabBarWidget::initConnection()
{
    // [0] 连接点击 + 按钮响应事件
    connect(this,&DTabBar::tabAddRequested,this,[=](){
        // 组装标签名字
        m_nameCounter++;
        QString divName = m_defaultName + QString::number(m_nameCounter);

        // 如果已经存在标签则显示存在的标签
        if(tabBarNameIsExist(divName)){
            m_nameCounter--;
            return ;
        }

        addTabBarItem(divName);
    });

    // [1] 连接点击关闭响应事件
    connect(this,&DTabBar::tabCloseRequested,this,[=](int index){
        QString closeItemName = this->tabText(index);
        emit signalTabItemCloseRequested(closeItemName);
    });

    // [2] 连接标签选择改变响应事件
    connect(this,&DTabBar::currentChanged,this,[=](int index){
        QString currentName = this->tabText(index);
        emit signalItemChanged(currentName);
    });
}

bool CMultipTabBarWidget::tabBarNameIsExist(QString name)
{
    for (int i = 0; i < this->count(); i++) {
        QString tempName = this->tabText(i);
        // 此判断用于文件是否已经修改
        if(tempName.startsWith("* ") && tempName.endsWith(" ")) {
            tempName = tempName.replace(0,2,"");
            tempName = tempName.replace(tempName.lastIndexOf(" "),1,"");
        }

        if(tempName == name) {
            this->setCurrentIndex(i);
            return true;
        }
    }
    return false;
}

void CMultipTabBarWidget::updateTabBarName(QString oldName, QString newName)
{
    for (int i = 0; i < this->count(); i++) {
        if(this->tabText(i)==oldName){
            this->setTabText(i,newName);
            return;
        }
    }
}

QString CMultipTabBarWidget::getNextTabBarDefaultName()
{
    m_nameCounter++;
    QString divName = m_defaultName + QString::number(m_nameCounter);
    m_nameCounter--;
    return divName;
}

void CMultipTabBarWidget::setTabBarTooltipName(QString tabName, QString tooltip)
{
    for(int i = 0; i < this->count(); i++){
        if(tabName == this->tabText(i)){
            setTabBarTooltipName(static_cast<quint16>(i),tooltip);
        }
    }
}

void CMultipTabBarWidget::setTabBarTooltipName(quint16 index, QString tooltip)
{
    this->setTabToolTip(index,tooltip);
}

void CMultipTabBarWidget::setCurrentTabBarWithName(QString tabName)
{
    for(int i = 0; i < this->count(); i++){
        if(tabName == this->tabText(i)){
            this->setCurrentIndex(i);
        }
    }
}

void CMultipTabBarWidget::resizeEvent(QResizeEvent *event)
{
    DTabBar::resizeEvent(event);
    updateTabWidth();
    this->update();
    this->resize(width(),36);
}

bool CMultipTabBarWidget::eventFilter(QObject *, QEvent *event)
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

                    connect(m_closeTabAction, &QAction::triggered, this, [=] {
                        // 跳转到需要关闭的标签
                        this->setCurrentIndex(m_rightClickTab);

                        emit signalTabItemCloseRequested(this->tabText(m_rightClickTab));
                    });

                    connect(m_closeOtherTabAction, &QAction::triggered, this, [=] {
                        QStringList closeNames;
                        for(int i = 0; i < this->count(); i++) {
                            if(m_rightClickTab == i){
                                continue;
                            }
                            closeNames << this->tabText(i);
                        }
                        emit signalTabItemsCloseRequested(closeNames);
                    });

                    m_rightMenu->addAction(m_closeTabAction);
                    m_rightMenu->addAction(m_closeOtherTabAction);

                    m_rightMenu->exec(mapToGlobal(position));

                    return true;
                }
            }
        } else {
            event->accept();
        }
        return false;
}

