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
    int pointA = 100;
    int pointB = 300;
    int addedCount = view->drawScene()->getBzItems().count();
    e.addMouseMove(QPoint(pointA, pointA));
    e.addMousePress(Qt::LeftButton, Qt::NoModifier, QPoint(pointA, pointA));
    e.addMouseMove(QPoint(pointB, pointB));
    e.addMouseRelease(Qt::LeftButton, Qt::NoModifier, QPoint(pointB, pointB), 300);
    // 左下角缩放
    e.addMouseMove(QPoint(pointB, pointB));
    e.addMousePress(Qt::LeftButton, Qt::NoModifier, QPoint(pointB, pointB));
    e.addMouseMove(QPoint(pointB + pointA, pointB + pointA));
    e.addMouseRelease(Qt::LeftButton, Qt::NoModifier, QPoint(pointB + pointA, pointB + pointA), 300);
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
    int pointA = 100;
    int pointB = 300;
    int addedCount = view->drawScene()->getBzItems().count();
    e.addMouseMove(QPoint(pointA, pointA));
    e.addMousePress(Qt::LeftButton, Qt::NoModifier, QPoint(pointA, pointA));
    e.addMouseMove(QPoint(pointB, pointB));
    e.addMouseRelease(Qt::LeftButton, Qt::NoModifier, QPoint(pointB, pointB), 300);
    // 左下角缩放
    e.addMouseMove(QPoint(pointB, pointB));
    e.addMousePress(Qt::LeftButton, Qt::NoModifier, QPoint(pointB, pointB));
    e.addMouseMove(QPoint(pointB + pointA, pointB + pointA));
    e.addMouseRelease(Qt::LeftButton, Qt::NoModifier, QPoint(pointB + pointA, pointB + pointA), 300);
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
    int pointA = 100;
    int pointB = 300;
    int addedCount = view->drawScene()->getBzItems().count();
    e.addMouseMove(QPoint(pointA, pointA));
    e.addMousePress(Qt::LeftButton, Qt::NoModifier, QPoint(pointA, pointA));
    e.addMouseMove(QPoint(pointB, pointB));
    e.addMouseRelease(Qt::LeftButton, Qt::NoModifier, QPoint(pointB, pointB), 300);
    // 左下角缩放
    e.addMouseMove(QPoint(pointB, pointB));
    e.addMousePress(Qt::LeftButton, Qt::NoModifier, QPoint(pointB, pointB));
    e.addMouseMove(QPoint(pointB + pointA, pointB + pointA));
    e.addMouseRelease(Qt::LeftButton, Qt::NoModifier, QPoint(pointB + pointA, pointB + pointA), 300);
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
    int pointA = 100;
    int pointB = 300;
    int addedCount = view->drawScene()->getBzItems().count();
    e.addMouseMove(QPoint(pointA, pointA));
    e.addMousePress(Qt::LeftButton, Qt::NoModifier, QPoint(pointA, pointA));
    e.addMouseMove(QPoint(pointB, pointB));
    e.addMouseRelease(Qt::LeftButton, Qt::NoModifier, QPoint(pointB, pointB), 300);
    // 左下角缩放
    e.addMouseMove(QPoint(pointB, pointB));
    e.addMousePress(Qt::LeftButton, Qt::NoModifier, QPoint(pointB, pointB));
    e.addMouseMove(QPoint(pointB + pointA, pointB + pointA));
    e.addMouseRelease(Qt::LeftButton, Qt::NoModifier, QPoint(pointB + pointA, pointB + pointA), 300);
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
    int pointA = 100;
    int pointB = 300;
    int addedCount = view->drawScene()->getBzItems().count();
    e.addMouseMove(QPoint(pointA, pointA));
    e.addMousePress(Qt::LeftButton, Qt::NoModifier, QPoint(pointA, pointA));
    e.addMouseMove(QPoint(pointB, pointB));
    e.addMouseRelease(Qt::LeftButton, Qt::NoModifier, QPoint(pointB, pointB), 300);
    // 左下角缩放
    e.addMouseMove(QPoint(pointB, pointB));
    e.addMousePress(Qt::LeftButton, Qt::NoModifier, QPoint(pointB, pointB));
    e.addMouseMove(QPoint(pointB + pointA, pointB + pointA));
    e.addMouseRelease(Qt::LeftButton, Qt::NoModifier, QPoint(pointB + pointA, pointB + pointA), 300);
    e.simulate(view->viewport());
    ASSERT_EQ(view->drawScene()->getBzItems().count(), addedCount + 1);
}

TEST(ItemTool, TestDrawLineItem)
{
    CGraphicsView *view = getCurView();
    ASSERT_NE(view, nullptr);
    CCentralwidget *c = getMainWindow()->getCCentralwidget();
    ASSERT_NE(c, nullptr);


    QToolButton *tool = nullptr;
    tool = c->getLeftToolBar()->findChild<QToolButton *>("LineTool");
    ASSERT_NE(tool, nullptr);
    tool->clicked();

    QTestEventList e;
    int pointA = 100;
    int pointB = 300;
    int addedCount = view->drawScene()->getBzItems().count();
    e.addMouseMove(QPoint(pointA, pointA));
    e.addMousePress(Qt::LeftButton, Qt::NoModifier, QPoint(pointA, pointA));
    e.addMouseMove(QPoint(pointB, pointB));
    e.addMouseRelease(Qt::LeftButton, Qt::NoModifier, QPoint(pointB, pointB), 300);
    // 左下角缩放
    e.addMouseMove(QPoint(pointB, pointB));
    e.addMousePress(Qt::LeftButton, Qt::NoModifier, QPoint(pointB, pointB));
    e.addMouseMove(QPoint(pointB + pointA, pointB + pointA));
    e.addMouseRelease(Qt::LeftButton, Qt::NoModifier, QPoint(pointB + pointA, pointB + pointA), 300);
    e.simulate(view->viewport());
    ASSERT_EQ(view->drawScene()->getBzItems().count(), addedCount + 1);
}

TEST(ItemTool, TestDrawPenItem)
{
    CGraphicsView *view = getCurView();
    ASSERT_NE(view, nullptr);
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
    e.addMouseMove(QPoint(pointA, pointA));
    e.addMousePress(Qt::LeftButton, Qt::NoModifier, QPoint(pointA, pointA));
    e.addMouseMove(QPoint(pointB, pointB));
    e.addMouseRelease(Qt::LeftButton, Qt::NoModifier, QPoint(pointB, pointB), 300);
    // 左下角缩放
    e.addMouseMove(QPoint(pointB, pointB));
    e.addMousePress(Qt::LeftButton, Qt::NoModifier, QPoint(pointB, pointB));
    e.addMouseMove(QPoint(pointB + pointA, pointB + pointA));
    e.addMouseRelease(Qt::LeftButton, Qt::NoModifier, QPoint(pointB + pointA, pointB + pointA), 300);
    e.simulate(view->viewport());
    ASSERT_EQ(view->drawScene()->getBzItems().count(), addedCount + 1);
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
    int pointA = 100;
    int pointB = 300;
    int addedCount = view->drawScene()->getBzItems().count();
    e.addMouseMove(QPoint(pointA, pointA));
    e.addMousePress(Qt::LeftButton, Qt::NoModifier, QPoint(pointA, pointA));
    e.addMouseMove(QPoint(pointB, pointB));
    e.addMouseRelease(Qt::LeftButton, Qt::NoModifier, QPoint(pointB, pointB), 300);
    // 左下角缩放
    e.addMouseMove(QPoint(pointB, pointB));
    e.addMousePress(Qt::LeftButton, Qt::NoModifier, QPoint(pointB, pointB));
    e.addMouseMove(QPoint(pointB + pointA, pointB + pointA));
    e.addMouseRelease(Qt::LeftButton, Qt::NoModifier, QPoint(pointB + pointA, pointB + pointA), 300);
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
    int pointA = 100;
    int pointB = 300;
    int addedCount = view->drawScene()->getBzItems().count();
    e.addMouseMove(QPoint(pointA, pointA));
    e.addMousePress(Qt::LeftButton, Qt::NoModifier, QPoint(pointA, pointA));
    e.addMouseMove(QPoint(pointB, pointB));
    e.addMouseRelease(Qt::LeftButton, Qt::NoModifier, QPoint(pointB, pointB), 300);
    // 左下角缩放
    e.addMouseMove(QPoint(pointB, pointB));
    e.addMousePress(Qt::LeftButton, Qt::NoModifier, QPoint(pointB, pointB));
    e.addMouseMove(QPoint(pointB + pointA, pointB + pointA));
    e.addMouseRelease(Qt::LeftButton, Qt::NoModifier, QPoint(pointB + pointA, pointB + pointA), 300);
    e.simulate(view->viewport());
    ASSERT_EQ(view->drawScene()->getBzItems().count(), addedCount + 1);
}

TEST(ItemTool, TestDrawCutItem)
{
//    CGraphicsView *view = getCurView();
//    ASSERT_NE(view, nullptr);
//    CCentralwidget *c = getMainWindow()->getCCentralwidget();
//    ASSERT_NE(c, nullptr);


//    QToolButton *tool = nullptr;
//    tool = c->getLeftToolBar()->findChild<QToolButton *>("CropTool");
//    ASSERT_NE(tool, nullptr);
//    tool->clicked();

//    QTestEventList e;
//    int pointA = 100;
//    int pointB = 300;
//    int addedCount = view->drawScene()->getBzItems().count();
//    e.addMouseMove(QPoint(pointA, pointA));
//    e.addMousePress(Qt::LeftButton, Qt::NoModifier, QPoint(pointA, pointA));
//    e.addMouseMove(QPoint(pointB, pointB));
//    e.addMouseRelease(Qt::LeftButton, Qt::NoModifier, QPoint(pointB, pointB), 300);
//    ASSERT_EQ(view->drawScene()->getBzItems().count(), addedCount + 1);
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

TEST(ItemTool, TestOpenDDF)
{
    CGraphicsView *view = getCurView();
    ASSERT_NE(view, nullptr);

    // 打开保存绘制的 ddf
    QString ddfpath = QApplication::applicationDirPath() + "/test_save.ddf";
    QFileInfo info(ddfpath);
    ASSERT_TRUE(info.exists());

    QMimeData mimedata;
    QList<QUrl> li;
    li.append(QUrl(ddfpath));
    mimedata.setUrls(li);

    const QPoint pos = view->viewport()->rect().center();
    QDragEnterEvent eEnter(pos, Qt::IgnoreAction, &mimedata, Qt::LeftButton, Qt::NoModifier);
    dApp->sendEvent(view->viewport(), &eEnter);

    QDropEvent e(pos, Qt::IgnoreAction, &mimedata, Qt::LeftButton, Qt::NoModifier);
    dApp->sendEvent(view->viewport(), &e);
    QTest::qWait(300);

    int addedCount = view->drawScene()->getBzItems().count();
    ASSERT_EQ(true, addedCount == 10 ? true : false); // if cut item added, must be equal to 11
}
