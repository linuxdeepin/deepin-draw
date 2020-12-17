///*
//* Copyright (C) 2019 ~ 2020 UnionTech Software Technology Co.,Ltd
//*
//* Author:     zhanghao<zhanghao@uniontech.com>
//* Maintainer: zhanghao<zhanghao@uniontech.com>
//* This program is free software: you can redistribute it and/or modify
//* it under the terms of the GNU General Public License as published by
//* the Free Software Foundation, either version 3 of the License, or
//* any later version.
//* This program is distributed in the hope that it will be useful,
//* but WITHOUT ANY WARRANTY; without even the implied warranty of
//* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//* GNU General Public License for more details.
//* You should have received a copy of the GNU General Public License
//* along with this program.  If not, see <http://www.gnu.org/licenses/>.
//*/

//#include <gtest/gtest.h>
//#include <gmock/gmock-matchers.h>

//#include "ccentralwidget.h"
//#include "clefttoolbar.h"
//#include "toptoolbar.h"
//#include "frame/cgraphicsview.h"
//#include "drawshape/cdrawscene.h"
//#include "drawshape/cdrawparamsigleton.h"
//#include "drawshape/drawItems/cgraphicsitemselectedmgr.h"
//#include "application.h"

//#include <QtTest>
//#include <QTestEventList>

//#include "../testItems/publicApi.h"

//TEST(ItemLayer, TestItemLayerCreateView)
//{
//    createNewViewByShortcutKey();
//}

//TEST(ItemLayer, TestItemLayerCreateItem)
//{
//    // [0] create item
//    CGraphicsView *view = getCurView();
//    ASSERT_NE(view, nullptr);
//    CCentralwidget *c = getMainWindow()->getCCentralwidget();
//    ASSERT_NE(c, nullptr);

//    QToolButton *tool = nullptr;
//    tool = c->getLeftToolBar()->findChild<QToolButton *>("RectangleTool");
//    ASSERT_NE(tool, nullptr);
//    tool->clicked();
//    createItemByMouse(view);
//    ASSERT_EQ(view->drawScene()->getBzItems().first()->type(), RectType);
//    tool->clicked();
//    createItemByMouse(view, false, QPoint(550, 300), QPoint(650, 450), false);
//    tool->clicked();
//    createItemByMouse(view, false, QPoint(600, 300), QPoint(700, 500), false);
//}

//TEST(ItemLayer, TestItemLayerUP)
//{
//    CGraphicsView *view = getCurView();
//    ASSERT_NE(view, nullptr);

//    QList<QGraphicsItem *> items = view->drawScene()->getBzItems();
//    ASSERT_EQ(items.count(), 3);

//    for (int i = 0; i < items.count(); i++) {
//        CGraphicsItem *pItem = dynamic_cast<CGraphicsItem *>(items.at(i));
//        int oldZvalue = pItem->zValue();
//        ASSERT_NE(pItem, nullptr);
//        view->drawScene()->clearMrSelection();
//        DTestEventList e;
//        e.addKeyPress(Qt::Key_BracketRight, Qt::ControlModifier, 50);
//        e.addKeyRelease(Qt::Key_BracketRight, Qt::ControlModifier, 50);
//        e.simulate(view->viewport());
//        ASSERT_EQ(pItem->zValue(), oldZvalue);
//        view->drawScene()->selectItem(pItem);
//        e.simulate(view->viewport());
//        ASSERT_EQ(pItem->zValue(), oldZvalue >= view->drawScene()->getMaxZValue() ? oldZvalue : oldZvalue + 1);
//    }
//}

//TEST(ItemLayer, TestItemLayerDown)
//{
//    CGraphicsView *view = getCurView();
//    ASSERT_NE(view, nullptr);

//    QList<QGraphicsItem *> items = view->drawScene()->getBzItems();
//    ASSERT_EQ(items.count(), 3);

//    for (int i = 0; i < items.count(); i++) {
//        QList<QGraphicsItem *> sortItems = items;
//        qSort(&sortItems.first(), &sortItems.last(), zValueSortASC);
//        int minZValue = sortItems.first()->zValue();

//        CGraphicsItem *pItem = dynamic_cast<CGraphicsItem *>(items.at(i));
//        int oldZvalue = pItem->zValue();
//        ASSERT_NE(pItem, nullptr);
//        view->drawScene()->clearMrSelection();
//        DTestEventList e;
//        e.addKeyPress(Qt::Key_BracketLeft, Qt::ControlModifier, 50);
//        e.addKeyRelease(Qt::Key_BracketLeft, Qt::ControlModifier, 50);
//        e.simulate(view->viewport());
//        ASSERT_EQ(pItem->zValue(), oldZvalue);

//        view->drawScene()->selectItem(pItem);
//        e.simulate(view->viewport());
//        ASSERT_EQ(pItem->zValue(), oldZvalue <= minZValue ? oldZvalue : oldZvalue - 1);
//    }
//}

//TEST(ItemLayer, TestItemSendToTop)
//{
//    CGraphicsView *view = getCurView();
//    ASSERT_NE(view, nullptr);

//    QList<QGraphicsItem *> items = view->drawScene()->getBzItems();
//    ASSERT_EQ(items.count(), 3);

//    for (int i = 0; i < items.count(); i++) {
//        CGraphicsItem *pItem = dynamic_cast<CGraphicsItem *>(items.at(i));
//        int oldZvalue = pItem->zValue();
//        int oldMaxZvalue = view->drawScene()->getMaxZValue();
//        ASSERT_NE(pItem, nullptr);
//        view->drawScene()->clearMrSelection();
//        DTestEventList e;
//        e.addKeyPress(Qt::Key_BracketRight, Qt::ControlModifier | Qt::ShiftModifier, 100);
//        e.addKeyRelease(Qt::Key_BracketRight, Qt::ControlModifier | Qt::ShiftModifier, 100);
//        e.simulate(view->viewport());
//        ASSERT_EQ(pItem->zValue(), oldZvalue);
//        view->drawScene()->selectItem(pItem);
//        e.simulate(view->viewport());
//        ASSERT_EQ(pItem->zValue(), oldZvalue >= oldMaxZvalue ? oldZvalue : oldMaxZvalue + 1);
//    }
//}

//TEST(ItemLayer, TestItemLayerSendToButtom)
//{
//    CGraphicsView *view = getCurView();
//    ASSERT_NE(view, nullptr);

//    QList<QGraphicsItem *> items = view->drawScene()->getBzItems();
//    ASSERT_EQ(items.count(), 3);

//    for (int i = 0; i < items.count(); i++) {
//        QList<QGraphicsItem *> sortItems = items;
//        qSort(&sortItems.first(), &sortItems.last(), zValueSortASC);
//        int minZValue = sortItems.first()->zValue();

//        CGraphicsItem *pItem = dynamic_cast<CGraphicsItem *>(items.at(i));
//        int oldZvalue = pItem->zValue();
//        ASSERT_NE(pItem, nullptr);
//        view->drawScene()->clearMrSelection();
//        DTestEventList e;
//        e.addKeyPress(Qt::Key_BracketLeft, Qt::ControlModifier | Qt::ShiftModifier, 50);
//        e.addKeyRelease(Qt::Key_BracketLeft, Qt::ControlModifier | Qt::ShiftModifier, 50);
//        e.simulate(view->viewport());
//        ASSERT_EQ(pItem->zValue(), oldZvalue);

//        view->drawScene()->selectItem(pItem);
//        e.simulate(view->viewport());
//        ASSERT_EQ(pItem->zValue(), oldZvalue <= minZValue ? oldZvalue : oldZvalue);
//    }
//}
