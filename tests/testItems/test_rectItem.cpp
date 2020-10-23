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

#if TEST_RECT_ITEM

TEST(RectItem, TestRectItemCreateView)
{
    int i = 0;
    while (i++ < 50) {
        QTest::qWait(200);
        if (getCurView() != nullptr) {
            break;
        }
    }
    if (getCurView() == nullptr) {
        qDebug() << __FILE__ << __LINE__ << "get CGraphicsView is nullptr.";
    }
    ASSERT_NE(getCurView(), nullptr);

    QTestEventList e;
    e.addKeyClick(Qt::Key_N, Qt::ControlModifier);
    e.simulate(getCurView());
    QTest::qWait(200);

    i = 0;
    while (i++ < 50) {
        QTest::qWait(200);
        if (getCurView() != nullptr) {
            break;
        }
    }
    if (getCurView() == nullptr) {
        qDebug() << __FILE__ << __LINE__ << "get CGraphicsView is nullptr.";
    }
    ASSERT_NE(getCurView(), nullptr);
}

TEST(RectItem, TestDrawRectItem)
{
    CGraphicsView *view = getCurView();
    ASSERT_NE(view, nullptr);
    CCentralwidget *c = getMainWindow()->getCCentralwidget();
    ASSERT_NE(c, nullptr);

    QToolButton *tool = nullptr;
    tool = c->getLeftToolBar()->findChild<QToolButton *>("RectangleTool");
    ASSERT_NE(tool, nullptr);
    tool->clicked();

    int addedCount = view->drawScene()->getBzItems().count();
    createItemByMouse(view);
    ASSERT_EQ(view->drawScene()->getBzItems().count(), addedCount + 1);

    ASSERT_EQ(view->drawScene()->getBzItems().first()->type(), RectType);
}

TEST(RectItem, TestCopyRectItem)
{
    CGraphicsView *view = getCurView();
    ASSERT_NE(view, nullptr);

    keyShortCutCopyItem();
}

TEST(RectItem, TestRectItemProperty)
{
    CGraphicsView *view = getCurView();
    ASSERT_NE(view, nullptr);

    CGraphicsRectItem *rect = dynamic_cast<CGraphicsRectItem *>(view->drawScene()->getBzItems().first());
    ASSERT_NE(rect, nullptr);

    // pen width
    setPenWidth(rect, 4);
    ASSERT_EQ(rect->pen().width(), 4);

    // stroke color
    QColor strokeColor(Qt::red);
    setStrokeColor(rect, strokeColor);
    ASSERT_EQ(rect->pen().color(), strokeColor);

    // brush color
    QColor brushColor(Qt::green);
    setBrushColor(rect, brushColor);
    ASSERT_EQ(rect->brush().color(), brushColor);

    // Rect Radius
    int defaultRadius = rect->getXRedius();
    QSpinBox *sp = dApp->topToolbar()->findChild<QSpinBox *>("RectRadius");
    ASSERT_NE(sp, nullptr);
    int value = sp->value() * 10;
    sp->setValue(value);
    QTest::qWait(100);
    ASSERT_EQ(rect->getXRedius(), sp->value());

    QTestEventList e;
    e.addKeyPress(Qt::Key_Z, Qt::ControlModifier, 100);
    e.simulate(view->viewport());
    ASSERT_EQ(rect->getXRedius(), defaultRadius);
    e.clear();
    e.addKeyPress(Qt::Key_Y, Qt::ControlModifier, 100);
    e.simulate(view->viewport());
    ASSERT_EQ(rect->getXRedius(), value);
}

TEST(RectItem, TestResizeRectItem)
{
    resizeItem();
}

TEST(RectItem, TestSelectAllRectItem)
{
    selectAllItem();
}

TEST(RectItem, TestSaveRectItemToFile)
{
    CGraphicsView *view = getCurView();
    ASSERT_NE(view, nullptr);
    CCentralwidget *c = getMainWindow()->getCCentralwidget();
    ASSERT_NE(c, nullptr);

    // save ddf file
    QString RectItemPath = QApplication::applicationDirPath() + "/test_rect.ddf";
    QFile file(RectItemPath);
    file.open(QIODevice::ReadWrite);
    file.close();
    view->getDrawParam()->setDdfSavePath(RectItemPath);
    c->slotSaveToDDF(true);
    QTest::qWait(100);

    QFileInfo info(RectItemPath);
    ASSERT_TRUE(info.exists());
}

TEST(RectItem, TestOpenRectItemFromFile)
{
    CGraphicsView *view = getCurView();
    ASSERT_NE(view, nullptr);

    // 打开保存绘制的 ddf
    QString RectItemPath = QApplication::applicationDirPath() + "/test_rect.ddf";
    QMimeData mimedata;
    QList<QUrl> li;
    li.append(QUrl(RectItemPath));
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
