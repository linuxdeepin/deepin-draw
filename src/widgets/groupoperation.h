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
#ifndef GROUPOPERATION_H
#define GROUPOPERATION_H

#include "expansionpanel.h"
#include "seperatorline.h"

#include <DIconButton>
#include <QHBoxLayout>
#include <DWidget>

class GroupOperation : public QWidget
{
    Q_OBJECT
public:
    /**
     * @brief   GroupOperation 构造函数
     */
    explicit GroupOperation(QWidget *parent = nullptr);

    /**
     * @brief   setMode 设置组合操作的显示模式
     * @param   mode值为true显示组合、释放按钮，false显示下拉按钮
     * @param   line值为true显示分割线，false隐藏分割线
     */
    void setMode(bool mode, bool line = true);

    /**
     * @brief   creatGroupButton 图元组合函数
     */
    void creatGroupButton();

    /**
     * @brief   cancelGroupButton 图元释放组合函数
     */
    void cancelGroupButton();

    /**
     * @brief   showExpansionPanel 文字属性栏下拉框显示
     */
    void showExpansionPanel();

    /**
     * @brief   clearUi 进行UI清除，重新布局
     */
    void clearUi();

    /**
     * @brief   getLayout 布局管理
     */
    QHBoxLayout *getLayout();

    /**
     * @brief   getExpansionPanel 扩展面板
     */
    ExpansionPanel *getExpansionPanel();

protected:
    /**
     * @brief   eventFilter 事件过滤
     */
    bool eventFilter(QObject *o, QEvent *e) override;

private:
    //下拉按钮
    DIconButton *openGroup = nullptr;
    //组合
    DIconButton *groupButton = nullptr;
    //释放组合
    DIconButton *unGroupButton = nullptr;
    //扩展面板
    ExpansionPanel *panel = nullptr;
    //布局管理
    QHBoxLayout *layout   = nullptr;
    //分割线
    SeperatorLine *sepLine = nullptr;
};

#endif // GROUPOPERATION_H
