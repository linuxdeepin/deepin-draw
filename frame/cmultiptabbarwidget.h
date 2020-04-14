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
#ifndef CMULTIPTABBARWIDGET_H
#define CMULTIPTABBARWIDGET_H

#include <DTabBar>
#include <DMenu>

DWIDGET_USE_NAMESPACE

/**
 * @description: 实现多标签拖拽选项栏
*/
class CMultipTabBarWidget : public DTabBar
{
    Q_OBJECT

public:
    CMultipTabBarWidget(QWidget *parent = nullptr);
    ~CMultipTabBarWidget();

    /**
     * @description: closeTabBarItem 关闭标签
     * @param: itemName 标签名字
    */
    void closeTabBarItem(QString itemName);

    /**
     * @description: setDefaultTabBarName 设置默认标签名字
     * @param: name 默认标签名字
    */
    void setDefaultTabBarName(QString name);

    /**
     * @description: checkTabBarNameIsExist 用于检查是否已经存在此标签，存在则跳转到该标签
     * @param: name 标签名字
    */
    bool tabBarNameIsExist(QString name);

    /**
     * @description: updateTabBarName 更新当前标签显示的名字
     * @param: oldName 原标签名字
     * @param: newName 新标签名字
    */
    void updateTabBarName(QString oldName, QString newName);

    /**
     * @description: getNextTabBarDefaultName 获取下一个默认标签名字
    */
    QString getNextTabBarDefaultName();

    /**
     * @description: setTabBarTooltipName 设置子标签的提示信息
     * @parma: tabName 标签名字
     * @parma： tooltip 提示信息
    */
    void setTabBarTooltipName(QString tabName, QString tooltip);

    /**
     * @description: setTabBarTooltipName 设置子标签的提示信息
     * @parma: index 标签下标索引
     * @parma： tooltip 提示信息
    */
    void setTabBarTooltipName(quint16 index, QString tooltip);

    /**
     * @description: setCurrentTabBarWithName 设置当前子标签通过名字
     * @parma: tabName 标签名字
    */
    void setCurrentTabBarWithName(QString tabName);

    /**
     * @description: getCurrentTabBarName 获取当前选中的标签名字
    */
    QString getCurrentTabBarName();

    /**
     * @description: getAllTabBarName 获取当前所有的标签名字
    */
    QStringList getAllTabBarName();

public slots:
    /**
     * @description: addTabBarItem 添加标签
     * @param: name 标签名字
    */
    void addTabBarItem(QString name = "");

signals:
    /**
     * @description: tabItemCloseRequested 关闭标签信号
     * 此信号发出后不会立即关闭标签，需要手动调用 closeTabBarItem 才会关闭
     * @param: itemName 标签名字
    */
    void signalTabItemCloseRequested(QString itemName);
    void signalTabItemsCloseRequested(QStringList itemNames);

    /**
     * @description: 新增加一个标签信号
     * @param:  itemName
     * @return: 无
    */
    void signalNewAddItem(QString itemName);

    /**
     * @description: 标签选择改变后当前选中的标签名字
     * @param:  itemName 改变对应的标签名字
     * @return: 无
    */
    void signalItemChanged(QString itemName);

protected:
    bool eventFilter(QObject *, QEvent *event) override;

private:
    void initConnection();

    quint32 m_tabbarWidth = 0; // tabbar的最小宽度
    QString m_defaultName; // 每个tabbar的默认名字前缀，后缀采用数字编号
    quint16 m_nameCounter; // 标签名字计数器

    QAction *m_closeOtherTabAction;
    QAction *m_closeTabAction;
    DMenu *m_rightMenu;
    int m_rightClickTab;
};

#endif // CMULTIPTABBARWIDGET_H
