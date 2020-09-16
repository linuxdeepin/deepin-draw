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
#ifndef MIANWINDOW_H
#define MIANWINDOW_H

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include "mainwindow.h"
#include "ccentralwidget.h"
#include "cgraphicsview.h"
#include "application.h"
#include "cgraphicsview.h"
#include "clefttoolbar.h"
#include "toptoolbar.h"
#include "cdrawscene.h"

#include "cpictureitem.h"
#include "cgraphicsrectitem.h"
#include "cgraphicsellipseitem.h"
#include "cgraphicstriangleitem.h"
#include "cgraphicspolygonalstaritem.h"
#include "cgraphicspolygonitem.h"
#include "cgraphicslineitem.h"
#include "cgraphicspenitem.h"
#include "cgraphicstextitem.h"
#include "cgraphicsmasicoitem.h"
#include "cgraphicscutitem.h"
#include "cgraphicsitemselectedmgr.h"

#include <DFloatingButton>
#include <DComboBox>
#include <dzoommenucombobox.h>
#include "cspinbox.h"
#include "bordercolorbutton.h"
#include "bigcolorbutton.h"

#include <QtTest>
#include <QTestEventList>

#define ON true
#define OFF false
#define TEST_PICTURE_ITEM ON
#define TEST_RECT_ITEM ON
#define TEST_ELLIPSE_ITEM ON
#define TEST_TRIANGLE_ITEM ON
#define TEST_START_ITEM ON
#define TEST_POLYGON_ITEM ON
#define TEST_LINE_ITEM ON
#define TEST_PEN_ITEM ON
#define TEST_TEXT_ITEM ON
#define TEST_BLUR_ITEM ON
#define TEST_CUT_ITEM ON
#define TEST_SCANLE_SCENCE ON

static MainWindow *getMainWindow()
{
    if (dApp->topMainWindow() == nullptr) {
        dApp->showMainWindow(QStringList());
        dApp->topMainWindow()->showMaximized();
    }
    return dApp->topMainWindow();
}

static CGraphicsView *getCurView()
{
    if (getMainWindow() != nullptr) {
        return dApp->topMainWindow()->getCCentralwidget()->getGraphicsView();
    }
    return nullptr;
}

inline void setPenWidth(int width)
{
    DComboBox *sideComBox = dApp->topToolbar()->findChild<DComboBox *>("SideWidth");
    // pen width 0 1 2 4 8 10 px
    if (width == 0 || width == 1 || width == 2) {
        sideComBox->setCurrentIndex(width);
    } else if (width == 4) {
        sideComBox->setCurrentIndex(3);
    } else if (width == 8 || width == 10) {
        sideComBox->setCurrentIndex(width / 2);
    }
    QTest::qWait(100);
}

inline void setStrokeColor(QColor color)
{
    BorderColorButton *stroke = dApp->topToolbar()->findChild<BorderColorButton *>("StrokeColorBtn");
    stroke->setColor(color);
    QTest::qWait(100);
}

inline void setBrushColor(QColor color)
{
    BigColorButton *brush = dApp->topToolbar()->findChild<BigColorButton *>("BrushColorBtn");
    brush->setColor(color);
    QTest::qWait(100);
}

inline void resizeItem()
{
    CGraphicsView *view = getCurView();
    ASSERT_NE(view, nullptr);

    CGraphicsItem *pItem = dynamic_cast<CGraphicsItem *>(view->drawScene()->getBzItems().first());
    ASSERT_NE(pItem, nullptr);

    view->drawScene()->clearMrSelection();
    view->drawScene()->selectItem(pItem);

    QVector<CSizeHandleRect *> handles = view->drawScene()->getItemsMgr()->handleNodes();

    // 普通拉伸
    for (int i = 0; i < handles.size(); ++i) {
        CSizeHandleRect *pNode = handles[i];
        QPoint posInView = view->mapFromScene(pNode->mapToScene(pNode->boundingRect().center()));
        QTestEventList e;
        e.addMouseMove(posInView, 100);
        e.addMousePress(Qt::LeftButton, Qt::ShiftModifier, posInView, 100);
        e.addMouseMove(posInView + QPoint(50, 50), 100);
        e.addMouseRelease(Qt::LeftButton, Qt::ShiftModifier, posInView + QPoint(50, 50), 100);
        e.simulate(view->viewport());
    }

    // 全选拉伸
    view->slotOnSelectAll();
    handles = view->drawScene()->getItemsMgr()->handleNodes();
    for (int i = 0; i < handles.size(); ++i) {
        CSizeHandleRect *pNode = handles[i];
        QPoint posInView = view->mapFromScene(pNode->mapToScene(pNode->boundingRect().center()));
        QTestEventList e;
        e.addMouseMove(posInView, 100);
        e.addMousePress(Qt::LeftButton, Qt::ShiftModifier, posInView, 100);
        e.addMouseMove(posInView + QPoint(50, 50), 100);
        e.addMouseRelease(Qt::LeftButton, Qt::ShiftModifier, posInView + QPoint(50, 50), 100);
        e.simulate(view->viewport());
    }

    // note: 等比拉伸(alt,shift)按住拉伸会失效,move 过程中没有按键按下事件
}

inline void createItemByMouse(CGraphicsView *view, bool altCopyItem = false, QPoint topLeft = QPoint(200, 100), QPoint bottomRight = QPoint(400, 300))
{
    QTestEventList e;
    e.clear();
    e.addMouseMove(topLeft, 100);
    e.addKeyPress(Qt::Key_Shift, Qt::NoModifier, 100);
    e.addMousePress(Qt::LeftButton, Qt::NoModifier, topLeft, 100);
    e.addMouseMove(bottomRight / 2, 100);
    e.addKeyRelease(Qt::Key_Shift, Qt::NoModifier, 100);
    e.addMouseMove(bottomRight, 100);
    e.addMouseRelease(Qt::LeftButton, Qt::NoModifier, bottomRight, 100);
    e.simulate(view->viewport());

    // alt move copy item will not sucess,because move event has no modifier
    if (altCopyItem) {
        e.clear();
        QPoint p = bottomRight - QPoint(30, 30);
        e.addMouseMove(p, 100);
        e.addKeyPress(Qt::Key_Alt, Qt::AltModifier, 100);
        e.addMousePress(Qt::LeftButton, Qt::AltModifier, p, 100);
        e.addMouseMove(bottomRight + QPoint(50, 50), 100);
        e.addMouseRelease(Qt::LeftButton, Qt::AltModifier, bottomRight + QPoint(50, 50), 100);
        e.addKeyRelease(Qt::Key_Alt, Qt::AltModifier, 100);
        e.simulate(view->viewport());
    }
}

inline void createItemWithkeyShift(CGraphicsView *view, bool altCopyItem = true, QPoint startPos = QPoint(200, 100)
                                                                                                   , QPoint endPos = QPoint(400, 300))
{
    QPoint topRight = startPos + QPoint(100, -50);
    QPoint topLeft = startPos + QPoint(-100, -50);
    QPoint bottomLeft = startPos + QPoint(-100, 50);
    QTestEventList e;
    e.clear();
    e.addMouseMove(topLeft, 100);
    e.addKeyPress(Qt::Key_Shift, Qt::NoModifier, 100);
    e.addMousePress(Qt::LeftButton, Qt::NoModifier, topLeft, 100);
    e.addMouseMove(topRight, 300);
    e.addMouseMove(topLeft, 300);
    e.addMouseMove(bottomLeft, 300);
    e.addMouseMove(endPos, 300);
    e.addMouseRelease(Qt::LeftButton, Qt::NoModifier, endPos, 100);
    e.addKeyRelease(Qt::Key_Shift, Qt::NoModifier, 100);
    e.simulate(view->viewport());

    if (altCopyItem) {
        e.clear();
        e.addMouseMove(endPos / 2, 100);
        e.addMousePress(Qt::LeftButton, Qt::AltModifier, endPos / 2, 100);
        e.addMouseMove(endPos + QPoint(50, 50), 100);
        e.addMouseRelease(Qt::LeftButton, Qt::AltModifier, endPos + QPoint(50, 50), 100);
        e.simulate(view->viewport());
    }
}
inline void keyShortCutCopyItem()
{
    CGraphicsView *view = getCurView();
    ASSERT_NE(view, nullptr);

    CGraphicsItem *pItem = dynamic_cast<CGraphicsItem *>(view->drawScene()->getBzItems().first());
    ASSERT_NE(pItem, nullptr);

    view->drawScene()->clearMrSelection();
    view->drawScene()->selectItem(pItem);

    int addedCount = view->drawScene()->getBzItems().count();
    QTestEventList e;
    e.addKeyPress(Qt::Key_C, Qt::ControlModifier, 100);
    e.addKeyPress(Qt::Key_V, Qt::ControlModifier, 100);
    e.simulate(view->viewport());
    ASSERT_EQ(view->drawScene()->getBzItems().count(), addedCount + 1);
}

inline void selectAllItem()
{
    CGraphicsView *view = getCurView();
    ASSERT_NE(view, nullptr);

    QTestEventList e;
    e.addMouseMove(QPoint(10, 10), 100);
    e.addMousePress(Qt::LeftButton, Qt::NoModifier, QPoint(10, 10), 100);
    e.addMouseMove(QPoint(1000, 1000), 100);
    e.addMouseRelease(Qt::LeftButton, Qt::NoModifier, QPoint(1000, 1000), 100);
    e.addKeyPress(Qt::Key_A, Qt::ControlModifier, 100);
    e.addKeyRelease(Qt::Key_A, Qt::ControlModifier, 100);
    e.simulate(view->viewport());
}

#endif // MIANWINDOW_H
