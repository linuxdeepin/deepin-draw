/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     TanLang <tanlang@uniontech.com>
 *
 * Maintainer: TanLang <tanlang@uniontech.com>
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
#ifndef ORDERWIDGET_H
#define ORDERWIDGET_H

#include <DToolButton>

#include "attributewidget.h"

DWIDGET_USE_NAMESPACE

class Page;
class DrawBoard;
class PageScene;

class OrderWidget : public AttributeWgt
{
    Q_OBJECT
public:
    OrderWidget(DrawBoard *drawBoard, QWidget *parent = nullptr);
public slots:
    void updateAlignButtonStatus();
    void updateLayerButtonStatus();
    void updateButtonsStatus();
private:
    void initUi();
    void initConnect();
    DToolButton *createIconButton(const QString &icon, const QString &toolTip, const QString &accesibleName);
    void doAlignment(Qt::AlignmentFlag align);
private:
    DToolButton *m_oneLayerUp = nullptr;      //向上一层
    DToolButton *m_oneLayerDown = nullptr;    //向下一层
    DToolButton *m_bringToFront = nullptr;    //置于最顶层
    DToolButton *m_sendToback = nullptr;      //置于最底层

    DToolButton *m_LeftAlign = nullptr;     //左对齐
    DToolButton *m_HCenterAlign = nullptr;  //水平居中对齐
    DToolButton *m_RightAlign = nullptr;    //右对齐
    DToolButton *m_TopAlign = nullptr;      //顶对齐
    DToolButton *m_VCenterAlign = nullptr;  //垂直居中对齐
    DToolButton *m_BottomAlign = nullptr;   //底对齐

    DToolButton *m_HEqulSpaceAlign = nullptr; //水平等间距对齐
    DToolButton *m_VEqulSpaceAlign = nullptr; //垂直等间距对齐
    DrawBoard   *m_drawBoard = nullptr;
    Page        *m_currentPage = nullptr;
    PageScene   *m_currentScene = nullptr;
};

#endif // ORDERWIDGET_H
