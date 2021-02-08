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
#define protected public
#define private public
#include "cgraphicsview.h"
#include <qaction.h>
#undef protected
#undef private
#include "ccentralwidget.h"
#include "clefttoolbar.h"
#include "toptoolbar.h"
#include "cgraphicsview.h"
#include "cdrawscene.h"
#include "drawshape/cdrawparamsigleton.h"
#include "drawshape/drawItems/cgraphicsitemselectedmgr.h"
#include "application.h"

#include "crecttool.h"
#include "ccuttool.h"
#include "cellipsetool.h"
#include "cmasicotool.h"
#include "cpentool.h"
#include "cpolygonalstartool.h"
#include "cpolygontool.h"
#include "ctexttool.h"
#include "ctriangletool.h"

#include <DFloatingButton>
#include <DComboBox>
#include <dzoommenucombobox.h>
#include "cspinbox.h"

#include "cpictureitem.h"
#include "cgraphicsrectitem.h"
#include "cgraphicsellipseitem.h"
#include "cgraphicstriangleitem.h"
#include "cgraphicspolygonalstaritem.h"
#include "cgraphicspolygonitem.h"
#include "cgraphicslineitem.h"
#include "cgraphicspenitem.h"
#include "cgraphicstextitem.h"
#include "cgraphicscutitem.h"

#include <QDebug>
#include <DLineEdit>

#include "publicApi.h"

#if TEST_PEN_ITEM

TEST(PenItem, TestPenItemCreateView)
{
    createNewViewByShortcutKey();
}

TEST(PenItem, TestDrawPenItem)
{
    CGraphicsView *view = getCurView();
    ASSERT_NE(view, nullptr);
    CCentralwidget *c = getMainWindow()->getCCentralwidget();
    ASSERT_NE(c, nullptr);

    QToolButton *tool = nullptr;
    tool = c->getLeftToolBar()->findChild<QToolButton *>("Pencil tool button");
    ASSERT_NE(tool, nullptr);
    tool->clicked();

    DTestEventList e;
    int pointA = 100;
    int pointB = 300;
    int addedCount = view->drawScene()->getBzItems().count();
    e.addMouseMove(QPoint(pointA * 0.5, pointA * 0.8), 100);
    e.addMousePress(Qt::LeftButton, Qt::NoModifier, QPoint(pointA * 2, pointA * 2), 100);
    e.addMouseMove(QPoint(pointB * 1.7, pointB * 1.9), 100);
    e.addMouseMove(QPoint(pointB * 1.3, pointB * 1.6), 100);
    e.addMouseRelease(Qt::LeftButton, Qt::NoModifier, QPoint(pointB, pointB), 100);

    // 继续连续画线
    e.addMouseMove(QPoint(pointA + 200, pointA), 100);
    e.addMousePress(Qt::LeftButton, Qt::NoModifier, QPoint(pointA + 200, pointA), 100);
    e.addMouseMove(QPoint(pointB + 200, pointB), 100);
    e.addMouseRelease(Qt::LeftButton, Qt::NoModifier, QPoint(pointB + 200, pointB), 100);
    e.addKeyClick(Qt::Key_Escape);
    e.addDelay(100);

    e.simulate(view->viewport());
    ASSERT_EQ(view->drawScene()->getBzItems().count(), addedCount + 2);
    ASSERT_EQ(view->drawScene()->getBzItems().first()->type(), PenType);
}

TEST(PenItem, TestCopyPenItem)
{
    keyShortCutCopyItem();
}

TEST(PenItem, TestPenItemProperty)
{
    CGraphicsView *view = getCurView();
    ASSERT_NE(view, nullptr);
    CGraphicsPenItem *pen = dynamic_cast<CGraphicsPenItem *>(view->drawScene()->getBzItems().first());
    ASSERT_NE(pen, nullptr);

    view->drawScene()->selectItem(pen);
    setPenWidth(pen, 4);
    ASSERT_EQ(pen->pen().width(), 4);

    // Start Type
    DComboBox *typeCombox = drawApp->topToolbar()->findChild<DComboBox *>("Line start style combox");
    ASSERT_NE(typeCombox, nullptr);
    for (int i = 0; i < typeCombox->count(); i++) {
        ELineType defaultType = pen->getPenStartType();
        typeCombox->setCurrentIndex(i);
        QTest::qWait(100);
        ASSERT_EQ(pen->getPenStartType(), i);

        DTestEventList e;
        e.addKeyPress(Qt::Key_Z, Qt::ControlModifier, 100);
        e.simulate(view->viewport());
        ASSERT_EQ(pen->getPenStartType(), defaultType);
        e.clear();
        e.addKeyPress(Qt::Key_Y, Qt::ControlModifier, 100);
        e.simulate(view->viewport());
        ASSERT_EQ(pen->getPenStartType(), i);
    }

    // End Type
    typeCombox = drawApp->topToolbar()->findChild<DComboBox *>("Line end style combox");
    ASSERT_NE(typeCombox, nullptr);
    for (int i = 0; i < typeCombox->count(); i++) {
        ELineType defaultType = pen->getPenEndType();
        typeCombox->setCurrentIndex(i);
        QTest::qWait(100);
        ASSERT_EQ(pen->getPenEndType(), i);

        DTestEventList e;
        e.addKeyPress(Qt::Key_Z, Qt::ControlModifier, 100);
        e.simulate(view->viewport());
        ASSERT_EQ(pen->getPenEndType(), defaultType);
        e.clear();
        e.addKeyPress(Qt::Key_Y, Qt::ControlModifier, 100);
        e.simulate(view->viewport());
        ASSERT_EQ(pen->getPenEndType(), i);
    }
}

TEST(PenItem, TestItemAlignment)
{
    itemAlignment();
}

TEST(PenItem, TestRightClick)
{
    itemRightClick();
}

TEST(PenItem, TestResizePenItem)
{
    resizeItem();
}

TEST(PenItem, TestSelectAllPenItem)
{
    CGraphicsView *view = getCurView();
    ASSERT_NE(view, nullptr);

    // 全选图元
    DTestEventList e;
    e.addMouseMove(QPoint(20, 20), 100);
    e.addMousePress(Qt::LeftButton, Qt::NoModifier, QPoint(10, 10), 100);
    e.addMouseMove(QPoint(1800, 900), 100);
    e.addMouseRelease(Qt::LeftButton, Qt::NoModifier, QPoint(1000, 1000), 100);
    e.addKeyPress(Qt::Key_A, Qt::ControlModifier, 100);
    e.addKeyRelease(Qt::Key_A, Qt::ControlModifier, 100);
    e.simulate(view->viewport());

    // 水平等间距对齐
    view->m_itemsVEqulSpaceAlign->triggered(true);
    // 垂直等间距对齐
    view->m_itemsHEqulSpaceAlign->triggered(true);

    //滚轮事件
    QWheelEvent wheelevent(QPointF(1000, 1000), 100, Qt::MouseButton::NoButton, Qt::KeyboardModifier::ControlModifier);
    view->wheelEvent(&wheelevent);
    QWheelEvent wheelevent2(QPointF(1000, 1000), 100, Qt::MouseButton::NoButton, Qt::KeyboardModifier::NoModifier);
    view->wheelEvent(&wheelevent2);
    QWheelEvent wheelevent3(QPointF(1000, 1000), 100, Qt::MouseButton::NoButton, Qt::KeyboardModifier::ShiftModifier);
    view->wheelEvent(&wheelevent3);
}

TEST(PenItem, TestLayerChange)
{
    layerChange();
}

TEST(PenItem, TestGroupUngroup)
{
    groupUngroup();
}

TEST(PenItem, TestSavePenItemToFile)
{
    CGraphicsView *view = getCurView();
    ASSERT_NE(view, nullptr);
    CCentralwidget *c = getMainWindow()->getCCentralwidget();
    ASSERT_NE(c, nullptr);

    // save ddf file
    QString PenItemPath = QApplication::applicationDirPath() + "/test_pen.ddf";
    QFile file(PenItemPath);
    file.open(QIODevice::ReadWrite);
    file.close();
    view->getDrawParam()->setDdfSavePath(PenItemPath);
    c->slotSaveToDDF(true);
    QTest::qWait(100);

    QFileInfo info(PenItemPath);
    ASSERT_TRUE(info.exists());
}

TEST(PenItem, TestOpenPenItemFromFile)
{
    CGraphicsView *view = getCurView();
    ASSERT_NE(view, nullptr);

    // 打开保存绘制的 ddf
    QString PenItemPath = QApplication::applicationDirPath() + "/test_pen.ddf";

    QMimeData mimedata;
    QList<QUrl> li;
    li.append(QUrl(PenItemPath));
    mimedata.setUrls(li);

    const QPoint pos = view->viewport()->rect().center();
    QDragEnterEvent eEnter(pos, Qt::IgnoreAction, &mimedata, Qt::LeftButton, Qt::NoModifier);
    dApp->sendEvent(view->viewport(), &eEnter);

    QDropEvent e(pos, Qt::IgnoreAction, &mimedata, Qt::LeftButton, Qt::NoModifier);
    dApp->sendEvent(view->viewport(), &e);
    QTest::qWait(100);

    view = getCurView();
    ASSERT_NE(view, nullptr);
    int addedCount = view->drawScene()->getBzItems(view->drawScene()->items()).count();
    ASSERT_EQ(addedCount, 3/*true, addedCount == 4 ? true : false*/);
}

#endif
