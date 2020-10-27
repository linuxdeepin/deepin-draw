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
#include "cgraphicsmasicoitem.h"
#include "cgraphicscutitem.h"

#include <QDebug>
#include <QtTest>
#include <QTestEventList>
#include <DLineEdit>

#include "publicApi.h"

#if TEST_POLYGON_ITEM

TEST(PolygonItem, TestPolygonItemCreateView)
{
    createNewViewByShortcutKey();
}

TEST(PolygonItem, TestDrawPolygonItem)
{
    CGraphicsView *view = getCurView();
    ASSERT_NE(view, nullptr);
    CCentralwidget *c = getMainWindow()->getCCentralwidget();
    ASSERT_NE(c, nullptr);

    QToolButton *tool = nullptr;
    tool = c->getLeftToolBar()->findChild<QToolButton *>("PolygonTool");
    ASSERT_NE(tool, nullptr);
    tool->clicked();

    int addedCount = view->drawScene()->getBzItems().count();
    createItemByMouse(view);
    ASSERT_EQ(view->drawScene()->getBzItems().count(), addedCount + 1);
    ASSERT_EQ(view->drawScene()->getBzItems().first()->type(), PolygonType);
}

TEST(PolygonItem, TestCopyPolygonItem)
{
    keyShortCutCopyItem();
}

TEST(PolygonItem, TestPolygonItemProperty)
{
    CGraphicsView *view = getCurView();
    ASSERT_NE(view, nullptr);
    CGraphicsPolygonItem *polygon  = dynamic_cast<CGraphicsPolygonItem *>(view->drawScene()->getBzItems().first());
    ASSERT_NE(polygon, nullptr);

    // pen width
    setPenWidth(polygon, 4);
    ASSERT_EQ(polygon->pen().width(), 4);

    // stroke color
    QColor strokeColor(Qt::red);
    setStrokeColor(polygon, strokeColor);
    ASSERT_EQ(polygon->pen().color(), strokeColor);

    // brush color
    QColor brushColor(Qt::green);
    setBrushColor(polygon, brushColor);

    // Polygon Side
    int defaultPoints = polygon->nPointsCount();
    CSpinBox *sp = dApp->topToolbar()->findChild<CSpinBox *>("PolygonSideNumber");
    ASSERT_NE(sp, nullptr);
    int value = sp->value() + 3;
    sp->setValue(value);
    QTest::qWait(100);
    ASSERT_EQ(polygon->nPointsCount(), sp->value());

    QTestEventList e;
    e.addKeyPress(Qt::Key_Z, Qt::ControlModifier, 100);
    e.simulate(view->viewport());
    ASSERT_EQ(polygon->nPointsCount(), defaultPoints);
    e.clear();
    e.addKeyPress(Qt::Key_Y, Qt::ControlModifier, 100);
    e.simulate(view->viewport());
    ASSERT_EQ(polygon->nPointsCount(), value);
}

TEST(PolygonItem, TestResizePolygonItem)
{
    resizeItem();
}

TEST(PolygonItem, TestSelectAllPolygonItem)
{
    selectAllItem();
}

TEST(PolygonItem, TestSavePolygonItemToFile)
{
    CGraphicsView *view = getCurView();
    ASSERT_NE(view, nullptr);
    CCentralwidget *c = getMainWindow()->getCCentralwidget();
    ASSERT_NE(c, nullptr);

    // save ddf file
    QString PolygonItemPath = QApplication::applicationDirPath() + "/test_polygon.ddf";
    QFile file(PolygonItemPath);
    file.open(QIODevice::ReadWrite);
    file.close();
    view->getDrawParam()->setDdfSavePath(PolygonItemPath);
    c->slotSaveToDDF(true);
    QTest::qWait(100);

    QFileInfo info(PolygonItemPath);
    ASSERT_TRUE(info.exists());
}

TEST(PolygonItem, TestOpenPolygonItemFromFile)
{
    CGraphicsView *view = getCurView();
    ASSERT_NE(view, nullptr);

    // 打开保存绘制的 ddf
    QString PolygonItemPath = QApplication::applicationDirPath() + "/test_polygon.ddf";

    QMimeData mimedata;
    QList<QUrl> li;
    li.append(QUrl(PolygonItemPath));
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
    ASSERT_EQ(true, addedCount == 2 ? true : false);
}

#endif
