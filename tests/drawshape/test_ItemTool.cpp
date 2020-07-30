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

#include "../getMainWindow.h"
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

#include <QDebug>
#include <QtTest>
#include <QTestEventList>

TEST(ItemTool, ItemTool)
{
    MainWindow *w = getMainWindow();
    w->hide();

    CCentralwidget *c = w->getCCentralwidget();
    CGraphicsView *view = c->getGraphicsView();

    int i = 0;
    while (!view && i++ < 50) {
        QTest::qWait(200);
        view = c->getGraphicsView();
    }

    if (view == nullptr) {
        qDebug() << __FILE__ << __LINE__ << "get CGraphicsView is nullptr.";
        return;
    }

    QMouseEvent *pressEvent = new QMouseEvent(QMouseEvent::MouseButtonPress
                                              , QPointF(10, 10), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QMouseEvent *moveEvent1 = new QMouseEvent(QMouseEvent::MouseButtonPress
                                              , QPointF(100, 100), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QMouseEvent *moveEvent2 = new QMouseEvent(QMouseEvent::MouseButtonPress
                                              , QPointF(200, 200), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QMouseEvent *releaseEvent = new QMouseEvent(QMouseEvent::MouseButtonRelease
                                                , QPointF(200, 200), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);

    /**
     * @brief slotShortCutRound　矩形按钮快捷键
     */
    c->getLeftToolBar()->slotShortCutRect();
//    QTestEventList e;
//    e.addMousePress(Qt::LeftButton, Qt::NoModifier, QPoint(100, 100), 10);
//    e.addMouseMove(QPoint(200, 200), 10);
//    e.addMouseRelease(Qt::LeftButton, Qt::NoModifier, QPoint(200, 200), 10);
//    e.simulate(view);
    view->mousePressEvent(pressEvent);
    view->mouseMoveEvent(moveEvent1);
    view->mouseMoveEvent(moveEvent2);
    view->mouseReleaseEvent(releaseEvent);

    /**
     * @brief slotShortCutRound　圆形按钮快捷键
     */
    c->getLeftToolBar()->slotShortCutRound();
    view->mousePressEvent(pressEvent);
    view->mouseMoveEvent(moveEvent1);
    view->mouseReleaseEvent(releaseEvent);

    /**
     * @brief slotShortCutTriangle　三角形按钮快捷键
     */
    c->getLeftToolBar()->slotShortCutTriangle();
    view->mousePressEvent(pressEvent);
    view->mouseMoveEvent(moveEvent1);
    view->mouseReleaseEvent(releaseEvent);

    /**
     * @brief slotShortCutPolygonalStar　星型按钮快捷键
     */
    c->getLeftToolBar()->slotShortCutPolygonalStar();
    view->mousePressEvent(pressEvent);
    view->mouseMoveEvent(moveEvent1);
    view->mouseReleaseEvent(releaseEvent);

    /**
     * @brief slotShortCutPolygon　多边形按钮快捷键
     */
    c->getLeftToolBar()->slotShortCutPolygon();
    view->mousePressEvent(pressEvent);
    view->mouseMoveEvent(moveEvent1);
    view->mouseReleaseEvent(releaseEvent);

    /**
     * @brief slotShortCutLine　线段按钮快捷键
     */
    c->getLeftToolBar()->slotShortCutLine();
    view->mousePressEvent(pressEvent);
    view->mouseMoveEvent(moveEvent1);
    view->mouseReleaseEvent(releaseEvent);

    /**
     * @brief slotShortCutPen　画笔按钮快捷键
     */
    c->getLeftToolBar()->slotShortCutPen();
    view->mousePressEvent(pressEvent);
    view->mouseMoveEvent(moveEvent1);
    view->mouseReleaseEvent(releaseEvent);

    /**
     * @brief slotShortCutText　文字按钮快捷键
     */
    c->getLeftToolBar()->slotShortCutText();
    view->mousePressEvent(pressEvent);
    view->mouseMoveEvent(moveEvent1);
    view->mouseReleaseEvent(releaseEvent);

    /**
     * @brief slotShortCutBlur　模糊按钮快捷键
     */
    c->getLeftToolBar()->slotShortCutBlur();
    view->mousePressEvent(pressEvent);
    view->mouseMoveEvent(moveEvent1);
    view->mouseReleaseEvent(releaseEvent);

    /**
     * @brief slotShortCutCut　裁剪按钮快捷键
     */
    c->getLeftToolBar()->slotShortCutCut();
    view->mousePressEvent(pressEvent);
    view->mouseMoveEvent(moveEvent1);
    view->mouseReleaseEvent(releaseEvent);
}
