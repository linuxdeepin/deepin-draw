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
#include <dzoommenucombobox.h>

#include <QDebug>
#include <QtTest>
#include <QTestEventList>
#include <QDrag>
#include <QList>

TEST(ItemTool, TestCreateNewScence)
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
    e.simulate(getCurView());

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

TEST(ItemTool, TestDrawRectItem)
{
    CGraphicsView *view = getCurView();
    ASSERT_NE(view, nullptr);
    CCentralwidget *c = getMainWindow()->getCCentralwidget();
    ASSERT_NE(c, nullptr);

    QToolButton *tool = nullptr;
    tool = c->getLeftToolBar()->findChild<QToolButton *>("RectangleTool");
    ASSERT_NE(tool, nullptr);
    tool->clicked();

    QTestEventList e;
    int pointA = 50;
    int pointB = 100;
    int addedCount = view->drawScene()->getBzItems().count();
    e.addMouseMove(QPoint(pointA, pointA), 100);
    e.addMousePress(Qt::LeftButton, Qt::NoModifier, QPoint(pointA, pointA), 100);
    e.addMouseMove(QPoint(pointB, pointB), 100);
    e.addMouseRelease(Qt::LeftButton, Qt::NoModifier, QPoint(pointB, pointB), 100);

    // 左下角缩放
    e.addMouseMove(QPoint(pointB, pointB), 100);
    e.addMousePress(Qt::LeftButton, Qt::NoModifier, QPoint(pointB, pointB), 100);
    e.addMouseMove(QPoint(pointB + pointA, pointB + pointA), 100);
    e.addMouseRelease(Qt::LeftButton, Qt::NoModifier, QPoint(pointB + 50, pointB + 50), 100);
    e.simulate(view->viewport());
    ASSERT_EQ(view->drawScene()->getBzItems().count(), addedCount + 1);
}

TEST(ItemTool, TestDrawEllipseItem)
{
    CGraphicsView *view = getCurView();
    ASSERT_NE(view, nullptr);
    CCentralwidget *c = getMainWindow()->getCCentralwidget();
    ASSERT_NE(c, nullptr);

    QToolButton *tool = nullptr;
    tool = c->getLeftToolBar()->findChild<QToolButton *>("EllipseTool");
    ASSERT_NE(tool, nullptr);
    tool->clicked();

    QTestEventList e;
    int pointA = 160;
    int pointB = 210;
    int addedCount = view->drawScene()->getBzItems().count();
    e.addMouseMove(QPoint(pointA, pointA), 100);
    e.addMousePress(Qt::LeftButton, Qt::NoModifier, QPoint(pointA, pointA), 100);
    e.addMouseMove(QPoint(pointB, pointB), 100);
    e.addMouseRelease(Qt::LeftButton, Qt::NoModifier, QPoint(pointB, pointB), 100);
    // 左下角缩放
    e.addMouseMove(QPoint(pointB, pointB));
    e.addMousePress(Qt::LeftButton, Qt::NoModifier, QPoint(pointB, pointB), 100);
    e.addMouseMove(QPoint(pointB + 50, pointB + 50), 100);
    e.addMouseRelease(Qt::LeftButton, Qt::NoModifier, QPoint(pointB + 50, pointB + 50), 100);
    e.simulate(view->viewport());
    ASSERT_EQ(view->drawScene()->getBzItems().count(), addedCount + 1);
}

TEST(ItemTool, TestDrawTriangleItem)
{
    CGraphicsView *view = getCurView();
    ASSERT_NE(view, nullptr);
    CCentralwidget *c = getMainWindow()->getCCentralwidget();
    ASSERT_NE(c, nullptr);

    QToolButton *tool = nullptr;
    tool = c->getLeftToolBar()->findChild<QToolButton *>("TriangleTool");
    ASSERT_NE(tool, nullptr);
    tool->clicked();

    QTestEventList e;
    int pointA = 270;
    int pointB = 320;
    int addedCount = view->drawScene()->getBzItems().count();
    e.addMouseMove(QPoint(pointA, pointA), 100);
    e.addMousePress(Qt::LeftButton, Qt::NoModifier, QPoint(pointA, pointA), 100);
    e.addMouseMove(QPoint(pointB, pointB), 100);
    e.addMouseRelease(Qt::LeftButton, Qt::NoModifier, QPoint(pointB, pointB), 100);
    // 左下角缩放
    e.addMouseMove(QPoint(pointB, pointB));
    e.addMousePress(Qt::LeftButton, Qt::NoModifier, QPoint(pointB, pointB), 100);
    e.addMouseMove(QPoint(pointB + pointA, pointB + pointA), 100);
    e.addMouseRelease(Qt::LeftButton, Qt::NoModifier, QPoint(pointB + 50, pointB + 50), 100);
    e.simulate(view->viewport());
    ASSERT_EQ(view->drawScene()->getBzItems().count(), addedCount + 1);
}

TEST(ItemTool, TestDrawStartItem)
{
    CGraphicsView *view = getCurView();
    ASSERT_NE(view, nullptr);
    CCentralwidget *c = getMainWindow()->getCCentralwidget();
    ASSERT_NE(c, nullptr);


    QToolButton *tool = nullptr;
    tool = c->getLeftToolBar()->findChild<QToolButton *>("StarTool");
    ASSERT_NE(tool, nullptr);
    tool->clicked();

    QTestEventList e;
    int pointA = 130;
    int pointB = 330;
    int addedCount = view->drawScene()->getBzItems().count();
    e.addMouseMove(QPoint(pointA, pointA), 100);
    e.addMousePress(Qt::LeftButton, Qt::NoModifier, QPoint(pointA, pointA), 100);
    e.addMouseMove(QPoint(pointB, pointB), 100);
    e.addMouseRelease(Qt::LeftButton, Qt::NoModifier, QPoint(pointB, pointB), 100);
    // 左下角缩放
    e.addMouseMove(QPoint(pointB, pointB), 100);
    e.addMousePress(Qt::LeftButton, Qt::NoModifier, QPoint(pointB, pointB), 100);
    e.addMouseMove(QPoint(pointB + pointA, pointB + pointA), 100);
    e.addMouseRelease(Qt::LeftButton, Qt::NoModifier, QPoint(pointB + pointA, pointB + pointA), 100);
    e.simulate(view->viewport());
    ASSERT_EQ(view->drawScene()->getBzItems().count(), addedCount + 1);
}

TEST(ItemTool, TestDrawPolygonItem)
{
    CGraphicsView *view = getCurView();
    ASSERT_NE(view, nullptr);
    CCentralwidget *c = getMainWindow()->getCCentralwidget();
    ASSERT_NE(c, nullptr);


    QToolButton *tool = nullptr;
    tool = c->getLeftToolBar()->findChild<QToolButton *>("PolygonTool");
    ASSERT_NE(tool, nullptr);
    tool->clicked();

    QTestEventList e;
    int pointA = 140;
    int pointB = 340;
    int addedCount = view->drawScene()->getBzItems().count();
    e.addMouseMove(QPoint(pointA, pointA), 100);
    e.addMousePress(Qt::LeftButton, Qt::NoModifier, QPoint(pointA, pointA), 100);
    e.addMouseMove(QPoint(pointB, pointB), 100);
    e.addMouseRelease(Qt::LeftButton, Qt::NoModifier, QPoint(pointB, pointB), 100);
    // 左下角缩放
    e.addMouseMove(QPoint(pointB, pointB), 100);
    e.addMousePress(Qt::LeftButton, Qt::NoModifier, QPoint(pointB, pointB), 100);
    e.addMouseMove(QPoint(pointB + pointA, pointB + pointA), 100);
    e.addMouseRelease(Qt::LeftButton, Qt::NoModifier, QPoint(pointB + pointA, pointB + pointA), 100);
    e.simulate(view->viewport());
    ASSERT_EQ(view->drawScene()->getBzItems().count(), addedCount + 1);
}

TEST(ItemTool, TestDrawLineItem)
{
    CGraphicsView *view = getCurView();
    ASSERT_NE(view, nullptr);
    view->getDrawParam()->setPen(QPen(QColor(Qt::red)));
    CCentralwidget *c = getMainWindow()->getCCentralwidget();
    ASSERT_NE(c, nullptr);

    QToolButton *tool = nullptr;
    tool = c->getLeftToolBar()->findChild<QToolButton *>("LineTool");
    ASSERT_NE(tool, nullptr);
    tool->clicked();

    QTestEventList e;
    int pointA = 150;
    int pointB = 350;
    int addedCount = view->drawScene()->getBzItems().count();
    e.addMouseMove(QPoint(pointA, pointA), 100);
    e.addMousePress(Qt::LeftButton, Qt::NoModifier, QPoint(pointA, pointA), 100);
    e.addMouseMove(QPoint(pointB, pointB), 100);
    e.addMouseRelease(Qt::LeftButton, Qt::NoModifier, QPoint(pointB, pointB), 100);
    // 左下角缩放
    e.addMouseMove(QPoint(pointB, pointB), 100);
    e.addMousePress(Qt::LeftButton, Qt::NoModifier, QPoint(pointB, pointB), 100);
    e.addMouseMove(QPoint(pointB + pointA, pointB + pointA), 100);
    e.addMouseRelease(Qt::LeftButton, Qt::NoModifier, QPoint(pointB + pointA, pointB + pointA), 100);
    e.simulate(view->viewport());
    ASSERT_EQ(view->drawScene()->getBzItems().count(), addedCount + 1);
}

TEST(ItemTool, TestDrawPenItem)
{
    CGraphicsView *view = getCurView();
    ASSERT_NE(view, nullptr);
    view->getDrawParam()->setPen(QPen(QColor(Qt::green)));
    CCentralwidget *c = getMainWindow()->getCCentralwidget();
    ASSERT_NE(c, nullptr);

    QToolButton *tool = nullptr;
    tool = c->getLeftToolBar()->findChild<QToolButton *>("PencilTool");
    ASSERT_NE(tool, nullptr);
    tool->clicked();

    QTestEventList e;
    int pointA = 100;
    int pointB = 300;
    int addedCount = view->drawScene()->getBzItems().count();
    e.addMouseMove(QPoint(pointA * 0.5, pointA * 0.8), 100);
    e.addMousePress(Qt::LeftButton, Qt::NoModifier, QPoint(pointA * 2, pointA * 2), 100);
    e.addMouseMove(QPoint(pointB * 1.7, pointB * 1.9), 100);
    e.addMouseMove(QPoint(pointB * 1.3, pointB * 1.6), 100);
    e.addMouseRelease(Qt::LeftButton, Qt::NoModifier, QPoint(pointB, pointB), 100);

    // 继续连续画线
    e.addMouseMove(QPoint(pointA, pointA), 100);
    e.addMousePress(Qt::LeftButton, Qt::NoModifier, QPoint(pointA, pointA), 100);
    e.addMouseMove(QPoint(pointB, pointB), 100);
    e.addMouseRelease(Qt::LeftButton, Qt::NoModifier, QPoint(pointB, pointB), 100);
    e.addKeyClick(Qt::Key_Escape);
    e.addDelay(100);

    // 左下角缩放
    e.addMouseMove(QPoint(pointB, pointB), 100);
    e.addMousePress(Qt::LeftButton, Qt::NoModifier, QPoint(pointB, pointB), 100);
    e.addMouseMove(QPoint(pointB + pointA, pointB + pointA), 100);
    e.addMouseRelease(Qt::LeftButton, Qt::NoModifier, QPoint(pointB + pointA, pointB + pointA), 100);
    e.simulate(view->viewport());
    ASSERT_EQ(view->drawScene()->getBzItems().count(), addedCount + 2);
}

TEST(ItemTool, TestDrawTextItem)
{
    CGraphicsView *view = getCurView();
    ASSERT_NE(view, nullptr);
    CCentralwidget *c = getMainWindow()->getCCentralwidget();
    ASSERT_NE(c, nullptr);

    QToolButton *tool = nullptr;
    tool = c->getLeftToolBar()->findChild<QToolButton *>("TextTool");
    ASSERT_NE(tool, nullptr);
    tool->clicked();

    QTestEventList e;
    int pointA = 400;
    int pointB = 600;
    int addedCount = view->drawScene()->getBzItems().count();
    e.addMouseMove(QPoint(pointA, pointA), 100);
    e.addMousePress(Qt::LeftButton, Qt::NoModifier, QPoint(pointA, pointA), 100);
    e.addMouseMove(QPoint(pointB, pointB), 100);
    e.addMouseRelease(Qt::LeftButton, Qt::NoModifier, QPoint(pointB, pointB), 100);
    e.simulate(view->viewport());
    ASSERT_EQ(view->drawScene()->getBzItems().count(), addedCount + 1);
}

TEST(ItemTool, TestDrawBlurItem)
{
    CGraphicsView *view = getCurView();
    ASSERT_NE(view, nullptr);
    CCentralwidget *c = getMainWindow()->getCCentralwidget();
    ASSERT_NE(c, nullptr);

    QToolButton *tool = nullptr;
    tool = c->getLeftToolBar()->findChild<QToolButton *>("BlurTool");
    ASSERT_NE(tool, nullptr);
    tool->clicked();

    QTestEventList e;
    int pointA = 0;
    int pointB = 500;
    int addedCount = view->drawScene()->getBzItems().count();
    e.addMouseMove(QPoint(pointA, pointA), 100);
    e.addMousePress(Qt::LeftButton, Qt::NoModifier, QPoint(pointA, pointA), 100);
    e.addMouseMove(QPoint(pointB, pointB), 100);
    e.addMouseRelease(Qt::LeftButton, Qt::NoModifier, QPoint(pointB, pointB), 100);
    // 左下角缩放
    e.addMouseMove(QPoint(pointB, pointB), 100);
    e.addMousePress(Qt::LeftButton, Qt::NoModifier, QPoint(pointB, pointB), 100);
    e.addMouseMove(QPoint(pointB + pointA, pointB + pointA), 100);
    e.addMouseRelease(Qt::LeftButton, Qt::NoModifier, QPoint(pointB + pointA, pointB + pointA), 100);
    e.simulate(view->viewport());
    ASSERT_EQ(view->drawScene()->getBzItems().count(), addedCount + 1);
}

TEST(ItemTool, TestDrawCutItem)
{
    CGraphicsView *view = getCurView();
    ASSERT_NE(view, nullptr);
    CCentralwidget *c = getMainWindow()->getCCentralwidget();
    ASSERT_NE(c, nullptr);

    QToolButton *tool = nullptr;
    tool = c->getLeftToolBar()->findChild<QToolButton *>("CropTool");
    ASSERT_NE(tool, nullptr);
    int addedCount = view->drawScene()->getBzItems().count();
    tool->clicked();

    QTestEventList e;
    e.addKeyClick(Qt::Key_Escape);
    e.simulate(view->viewport());
    ASSERT_EQ(view->drawScene()->getBzItems().count(), addedCount);
}

TEST(ItemTool, TestPastePictureItem)
{
    CGraphicsView *view = getCurView();
    ASSERT_NE(view, nullptr);
    CCentralwidget *c = getMainWindow()->getCCentralwidget();
    ASSERT_NE(c, nullptr);

    int addedCount = view->drawScene()->getBzItems().count();
    QString path = QApplication::applicationDirPath() + "/test.png";
    QPixmap pix(":/test.png");
    ASSERT_EQ(true, pix.save(path, "PNG"));
    QTest::qWait(300);

    QMimeData mimedata;
    QList<QUrl> li;
    li.append(QUrl(path));
    mimedata.setUrls(li);

    const QPoint pos = view->viewport()->rect().center();
    QDragEnterEvent eEnter(pos, Qt::IgnoreAction, &mimedata, Qt::LeftButton, Qt::NoModifier);
    dApp->sendEvent(view->viewport(), &eEnter);

    QDropEvent e(pos, Qt::IgnoreAction, &mimedata, Qt::LeftButton, Qt::NoModifier);
    dApp->sendEvent(view->viewport(), &e);
    QTest::qWait(300);

    ASSERT_EQ(view->drawScene()->getBzItems().count(), addedCount + 1);
}

TEST(ItemTool, TestScanleScence)
{
    MainWindow *w = getMainWindow();
    ASSERT_NE(w, nullptr);
    TopToolbar *toptoolbar = w->getTopToolbar();
    ASSERT_NE(toptoolbar, nullptr);
    /**
     * @brief 缩放view以及缩放菜单测试
     */
    DFloatingButton *fbtn;
    fbtn = w->getTopToolbar()->findChild<DFloatingButton *>("ReduceScence");
    ASSERT_NE(fbtn, nullptr);
    fbtn->clicked();
    fbtn->clicked();

    fbtn = w->getTopToolbar()->findChild<DFloatingButton *>("IncreaseScence");
    ASSERT_NE(fbtn, nullptr);
    fbtn->clicked();
    fbtn->clicked();

    DZoomMenuComboBox *box = w->getTopToolbar()->findChild<DZoomMenuComboBox *>("zoomMenuComboBox");
    ASSERT_NE(box, nullptr);
    box->setCurrentText("75%");

    QTestEventList e;
    e.addKeyClick(Qt::Key_Minus, Qt::ControlModifier, 100);
    e.addKeyClick(Qt::Key_Minus, Qt::ControlModifier, 100);
    e.addKeyClick(Qt::Key_Plus, Qt::ControlModifier, 100);
    e.addKeyClick(Qt::Key_Equal, Qt::ControlModifier, 100);
    e.addKeyClick(Qt::Key_Equal, Qt::ControlModifier, 100);
    e.addKeyClick(Qt::Key_Plus, Qt::ControlModifier, 100);
    e.addKeyClick(Qt::Key_Plus, Qt::ControlModifier, 100);
    e.simulate(getCurView());
}

TEST(ItemTool, TestSaveDDF)
{
    CGraphicsView *view = getCurView();
    ASSERT_NE(view, nullptr);
    CCentralwidget *c = getMainWindow()->getCCentralwidget();
    ASSERT_NE(c, nullptr);

    // save ddf file
    QString ddfpath = QApplication::applicationDirPath() + "/test_save.ddf";
    QFile file(ddfpath);
    file.open(QIODevice::ReadWrite);
    file.close();
    view->getDrawParam()->setDdfSavePath(ddfpath);
    c->slotSaveToDDF(true);
    QTest::qWait(100);
}
