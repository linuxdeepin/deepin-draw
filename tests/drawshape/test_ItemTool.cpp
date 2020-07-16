/*
* Copyright (C) 2019 ~ 2020 UnionTech Software Technology Co.,Ltd
*
* Author:     zhanghao<zhanghao@uniontech.com>
* Maintainer: zhanghao<zhanghao@uniontech.com>
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include "mainwindow.h"
#include "ccentralwidget.h"
#include "clefttoolbar.h"
#include "frame/cgraphicsview.h"

#include "crecttool.h"
#include "ccuttool.h"
#include "cellipsetool.h"
#include "cmasicotool.h"
#include "cpentool.h"
#include "cpolygonalstartool.h"
#include "cpolygontool.h"
#include "ctexttool.h"
#include "ctriangletool.h"

TEST(ItemTool, ItemTool)
{
    MainWindow *w = new MainWindow;
    w->hide();

    CCentralwidget *c = w->getCCentralwidget();
    CGraphicsView *view = c->getGraphicsView();

    QMouseEvent *pressEvent = new QMouseEvent(QMouseEvent::MouseButtonPress
                                              , QPointF(10, 10), Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
    QMouseEvent *moveEvent = new QMouseEvent(QMouseEvent::MouseButtonPress
                                             , QPointF(100, 100), Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
    QMouseEvent *releaseEvent = new QMouseEvent(QMouseEvent::MouseButtonPress
                                                , QPointF(100, 100), Qt::LeftButton, Qt::NoButton, Qt::NoModifier);

    /**
     * @brief slotShortCutRound　矩形按钮快捷键
     */
    c->getLeftToolBar()->slotShortCutRect();
    view->mousePressEvent(pressEvent);
    view->mouseMoveEvent(moveEvent);
    view->mouseReleaseEvent(releaseEvent);

    /**
     * @brief slotShortCutRound　圆形按钮快捷键
     */
    c->getLeftToolBar()->slotShortCutRound();
    view->mousePressEvent(pressEvent);
    view->mouseMoveEvent(moveEvent);
    view->mouseReleaseEvent(releaseEvent);

    /**
     * @brief slotShortCutTriangle　三角形按钮快捷键
     */
    c->getLeftToolBar()->slotShortCutTriangle();
    view->mousePressEvent(pressEvent);
    view->mouseMoveEvent(moveEvent);
    view->mouseReleaseEvent(releaseEvent);

    /**
     * @brief slotShortCutPolygonalStar　星型按钮快捷键
     */
    c->getLeftToolBar()->slotShortCutPolygonalStar();
    view->mousePressEvent(pressEvent);
    view->mouseMoveEvent(moveEvent);
    view->mouseReleaseEvent(releaseEvent);

    /**
     * @brief slotShortCutPolygon　多边形按钮快捷键
     */
    c->getLeftToolBar()->slotShortCutPolygon();
    view->mousePressEvent(pressEvent);
    view->mouseMoveEvent(moveEvent);
    view->mouseReleaseEvent(releaseEvent);

    /**
     * @brief slotShortCutLine　线段按钮快捷键
     */
    c->getLeftToolBar()->slotShortCutLine();
    view->mousePressEvent(pressEvent);
    view->mouseMoveEvent(moveEvent);
    view->mouseReleaseEvent(releaseEvent);

    /**
     * @brief slotShortCutPen　画笔按钮快捷键
     */
    c->getLeftToolBar()->slotShortCutPen();
    view->mousePressEvent(pressEvent);
    view->mouseMoveEvent(moveEvent);
    view->mouseReleaseEvent(releaseEvent);

    /**
     * @brief slotShortCutText　文字按钮快捷键
     */
    c->getLeftToolBar()->slotShortCutText();
    view->mousePressEvent(pressEvent);
    view->mouseMoveEvent(moveEvent);
    view->mouseReleaseEvent(releaseEvent);

    /**
     * @brief slotShortCutBlur　模糊按钮快捷键
     */
    c->getLeftToolBar()->slotShortCutBlur();
    view->mousePressEvent(pressEvent);
    view->mouseMoveEvent(moveEvent);
    view->mouseReleaseEvent(releaseEvent);

    /**
     * @brief slotShortCutCut　裁剪按钮快捷键
     */
    c->getLeftToolBar()->slotShortCutCut();

    view->mousePressEvent(pressEvent);
    view->mouseMoveEvent(moveEvent);
    view->mouseReleaseEvent(releaseEvent);
}
