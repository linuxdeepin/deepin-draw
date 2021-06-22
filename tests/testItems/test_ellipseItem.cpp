/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     Zhang Hao <zhanghao@uniontech.com>
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
#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#define protected public
#define private public
#include "cgraphicsview.h"
#include <qaction.h>
#include "cviewmanagement.h"
#undef protected
#undef private

#include "ccentralwidget.h"
#include "clefttoolbar.h"
#include "toptoolbar.h"
#include "frame/cgraphicsview.h"
#include "drawshape/cdrawscene.h"
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

#if TEST_ELLIPSE_ITEM

TEST(EllipseItem, TestEllipseItemCreateView)
{
    createNewViewByShortcutKey();
}

TEST(EllipseItem, TestDrawEllipseItem)
{
    CGraphicsView *view = getCurView();
    ASSERT_NE(view, nullptr);
    CCentralwidget *c = getMainWindow()->getCCentralwidget();
    ASSERT_NE(c, nullptr);

    QToolButton *tool = nullptr;
    tool = c->getLeftToolBar()->findChild<QToolButton *>("Ellipse tool button");
    ASSERT_NE(tool, nullptr);
    tool->clicked();

    int addedCount = view->drawScene()->getBzItems().count();
    createItemByMouse(view);
    ASSERT_EQ(view->drawScene()->getBzItems().count(), addedCount + 1);
    ASSERT_EQ(view->drawScene()->getBzItems().first()->type(), EllipseType);
}

TEST(EllipseItem, TestCopyEllipseItem)
{
    keyShortCutCopyItem();
}

TEST(EllipseItem, TestEllipseItemProperty)
{
    CGraphicsView *view = getCurView();
    ASSERT_NE(view, nullptr);
    CGraphicsItem *item = dynamic_cast<CGraphicsItem *>(view->drawScene()->getBzItems().first());

    // pen width
    setPenWidth(item, 4);
    ASSERT_EQ(item->pen().width(), 4);

    // stroke color
    QColor strokeColor(Qt::red);
    setStrokeColor(item, strokeColor);
    ASSERT_EQ(item->pen().color(), strokeColor);

    // brush color
    QColor brushColor(Qt::green);
    setBrushColor(item, brushColor);

}

TEST(EllipseItem, TestItemAlignment)
{
    itemAlignment();
}

TEST(EllipseItem, TestRightClick)
{
    itemRightClick();
}

TEST(EllipseItem, TestResizeEllipseItem)
{
    resizeItem();
}

TEST(EllipseItem, TestSelectAllEllipseItem)
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

TEST(EllipseItem, TestLayerChange)
{
    layerChange();
}

TEST(EllipseItem, TestGroupUngroup)
{
    groupUngroup();
}


TEST(EllipseItem, TestSaveEllipseItemToFile)
{
    CGraphicsView *view = getCurView();
    int addedCount = view->drawScene()->getBzItems(view->drawScene()->items()).count();
    qWarning() << "addedCount ============= " << addedCount;
    ASSERT_NE(view, nullptr);
    CCentralwidget *c = getMainWindow()->getCCentralwidget();
    ASSERT_NE(c, nullptr);

    // save ddf file
    QString EllipseItemPath = QApplication::applicationDirPath() + "/test_ellipse.ddf";
    QFile file(EllipseItemPath);
    file.open(QIODevice::ReadWrite);
    file.close();
    view->getDrawParam()->setDdfSavePath(EllipseItemPath);
    c->slotSaveToDDF(true);
    QTest::qWait(100);

    QFileInfo info(EllipseItemPath);
    ASSERT_TRUE(info.exists());
}

TEST(EllipseItem, TestOpenEllipseItemFromFile)
{

    CGraphicsView *view = getCurView();
    qWarning() << "viewviewviewviewviewviewviewviewviewviewview1 = " << view->getDrawParam()->viewName();
    ASSERT_NE(view, nullptr);

    // 打开保存绘制的 ddf
    QString EllipseItemPath = QApplication::applicationDirPath() + "/test_ellipse.ddf";

    QMimeData mimedata;
    QList<QUrl> li;
    li.append(QUrl(EllipseItemPath));
    mimedata.setUrls(li);

    const QPoint pos = view->viewport()->rect().center();
    QDragEnterEvent eEnter(pos, Qt::IgnoreAction, &mimedata, Qt::LeftButton, Qt::NoModifier);
    dApp->sendEvent(view->viewport(), &eEnter);

    QDropEvent e(pos, Qt::IgnoreAction, &mimedata, Qt::LeftButton, Qt::NoModifier);
    dApp->sendEvent(view->viewport(), &e);
    //QTest::qWait(300);
    (void)QTest::qWaitFor([ = ]() {return (view != getCurView() && getCurView()->drawScene()->getBzItems().count());});

    auto ddfView = getCurView();
    qWarning() << "viewviewviewviewviewviewviewviewviewviewview2 = " << ddfView->getDrawParam()->viewName();
    ASSERT_NE(ddfView, nullptr);
    int addedCount = ddfView->drawScene()->getBzItems().count();
    ASSERT_EQ(addedCount, 2);
}

#endif
