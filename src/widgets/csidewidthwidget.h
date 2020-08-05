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

#ifndef CSIDEWIDTHWIDGET_H
#define CSIDEWIDTHWIDGET_H

#include <DFrame>
#include <QHBoxLayout>
#include <DWidget>
#include <DLabel>

DWIDGET_USE_NAMESPACE

//class DMenuComboBox;

class CSideWidthWidget : public DWidget
{
public:
    Q_OBJECT
public:
    explicit CSideWidthWidget(DWidget *parent = nullptr);

    // 更新线宽度函数
    void updateSideWidth();
    // 更新线宽度函数
    void setSideWidth(int width);
    /*
    * @bref: setMenuNoSelected 设置子菜单未选中
    * @param noSelect
    */
    void setMenuNoSelected(bool noSelect);

signals:
    void signalSideWidthChange();
    void signalSideWidthMenuShow();
    void signalSideWidthChoosed(int width);

    void sideWidthChanged(int width, bool preview = false);

public:
    void changeButtonTheme();

private:
//    DMenuComboBox *m_menuComboBox;// 线宽度选择下拉框
    DComboBox *m_menuComboBox;// 线宽度选择下拉框
    QHBoxLayout *m_layout;// 水平布局器
    quint8 m_comboxHeight; // 下拉框的高度
    DLabel *m_maskLable;

private:
    void initUI();
    void initConnection();

    /*
     * @bref: initLineWidthToCombox 初始化线宽度
    */
    void initLineWidthToCombox();
    /*
     * @bref: drawLinePixmap 绘制一张pixmap线
    */
    QPixmap drawLinePixmap(int lineWidth, QColor lineColor, int width, int height);
};

#endif // CSIDEWIDTHWIDGET_H
