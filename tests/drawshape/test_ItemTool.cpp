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
#include "drawshape/cdrawscene.h"
#include "drawshape/cdrawparamsigleton.h"
#include "drawshape/drawItems/cgraphicsitemselectedmgr.h"

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

    CCentralwidget *c = w->getCCentralwidget();
    CGraphicsView *view = c->getGraphicsView();
    TopToolbar *toptoolbar = w->getTopToolbar();

    ASSERT_NE(w, nullptr);
    ASSERT_NE(c, nullptr);
    ASSERT_NE(toptoolbar, nullptr);


    int i = 0;
    while (!view && i++ < 50) {
        QTest::qWait(200);
        view = c->getGraphicsView();
    }

    if (view == nullptr) {
        qDebug() << __FILE__ << __LINE__ << "get CGraphicsView is nullptr.";
        return;
    }

    ASSERT_NE(view, nullptr);

    QMouseEvent *pressEvent = new QMouseEvent(QMouseEvent::MouseButtonPress
                                              , QPointF(10, 10), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QMouseEvent *moveEvent1 = new QMouseEvent(QMouseEvent::MouseButtonPress
                                              , QPointF(100, 100), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QMouseEvent *moveEvent2 = new QMouseEvent(QMouseEvent::MouseButtonPress
                                              , QPointF(200, 200), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QMouseEvent *releaseEvent = new QMouseEvent(QMouseEvent::MouseButtonRelease
                                                , QPointF(200, 200), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);

    QMouseEvent *resizeMoveEvent = new QMouseEvent(QMouseEvent::MouseButtonRelease
                                                   , QPointF(300, 300), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);

    QMouseEvent *resizeReleaseEvent = new QMouseEvent(QMouseEvent::MouseButtonRelease
                                                      , QPointF(350, 350), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);

    view->getDrawParam()->setCurrentDrawToolMode(EDrawToolMode::noselected);

    /**
     * @brief slotShortCutRound　矩形按钮快捷键
     */
    view->getDrawParam()->setCurrentDrawToolMode(EDrawToolMode::rectangle);
    c->getLeftToolBar()->slotShortCutRect();
    view->mousePressEvent(pressEvent);
    view->mouseMoveEvent(moveEvent1);
    view->mouseMoveEvent(moveEvent2);
    view->mouseReleaseEvent(releaseEvent);
    view->drawScene()->getItemsMgr()->updateAttributes();
    /**
     * @brief slotShortCutRound　圆形按钮快捷键
     */
    view->getDrawParam()->setCurrentDrawToolMode(EDrawToolMode::ellipse);
    c->getLeftToolBar()->slotShortCutRound();
    view->mousePressEvent(pressEvent);
    view->mouseMoveEvent(moveEvent1);
    view->mouseReleaseEvent(releaseEvent);
    view->drawScene()->getItemsMgr()->updateAttributes();

    /**
     * @brief slotShortCutTriangle　三角形按钮快捷键
     */
    view->getDrawParam()->setCurrentDrawToolMode(EDrawToolMode::triangle);
    c->getLeftToolBar()->slotShortCutTriangle();
    view->mousePressEvent(pressEvent);
    view->mouseMoveEvent(moveEvent1);
    view->mouseReleaseEvent(releaseEvent);
    view->drawScene()->getItemsMgr()->updateAttributes();

    /**
     * @brief slotShortCutPolygonalStar　星型按钮快捷键
     */
    view->getDrawParam()->setCurrentDrawToolMode(EDrawToolMode::polygonalStar);
    c->getLeftToolBar()->slotShortCutPolygonalStar();
    view->mousePressEvent(pressEvent);
    view->mouseMoveEvent(moveEvent1);
    view->mouseReleaseEvent(releaseEvent);
    view->drawScene()->getItemsMgr()->updateAttributes();
    // 模拟resize 暂时没有成功
    QTest::qWait(1000);
    view->mousePressEvent(releaseEvent);
    view->mouseMoveEvent(resizeMoveEvent);
    view->mouseReleaseEvent(resizeReleaseEvent);
    QTest::qWait(2000);

    /**
     * @brief slotShortCutPolygon　多边形按钮快捷键
     */
    view->getDrawParam()->setCurrentDrawToolMode(EDrawToolMode::polygon);
    c->getLeftToolBar()->slotShortCutPolygon();
    view->mousePressEvent(pressEvent);
    view->mouseMoveEvent(moveEvent1);
    view->mouseReleaseEvent(releaseEvent);
    view->drawScene()->getItemsMgr()->updateAttributes();

    /**
     * @brief slotShortCutLine　线段按钮快捷键
     */
    view->getDrawParam()->setCurrentDrawToolMode(EDrawToolMode::line);
    view->getDrawParam()->setLineColor(QColor(Qt::red));
    view->getDrawParam()->setLineStartType(ELineType::soildArrow);
    view->getDrawParam()->setLineEndType(ELineType::normalRing);
    c->getLeftToolBar()->slotShortCutLine();
    view->mousePressEvent(pressEvent);
    view->mouseMoveEvent(moveEvent1);
    view->mouseReleaseEvent(releaseEvent);
    view->drawScene()->getItemsMgr()->updateAttributes();

    /**
     * @brief slotShortCutPen　画笔按钮快捷键
     */
    view->getDrawParam()->setCurrentDrawToolMode(EDrawToolMode::pen);
    view->getDrawParam()->setPenStartType(ELineType::soildRing);
    view->getDrawParam()->setPenEndType(ELineType::normalRing);
    c->getLeftToolBar()->slotShortCutPen();
    view->mousePressEvent(pressEvent);
    view->mouseMoveEvent(moveEvent1);
    view->mouseReleaseEvent(releaseEvent);
    view->drawScene()->getItemsMgr()->updateAttributes();

    /**
     * @brief slotShortCutText　文字按钮快捷键
     */
    view->getDrawParam()->setCurrentDrawToolMode(EDrawToolMode::text);
    c->getLeftToolBar()->slotShortCutText();
    view->mousePressEvent(pressEvent);
    view->mouseMoveEvent(moveEvent1);
    view->mouseReleaseEvent(releaseEvent);
    view->drawScene()->getItemsMgr()->updateAttributes();

    /**
     * @brief slotShortCutBlur　模糊按钮快捷键
     */
    view->getDrawParam()->setCurrentDrawToolMode(EDrawToolMode::blur);
    c->getLeftToolBar()->slotShortCutBlur();
    view->mousePressEvent(pressEvent);
    view->mouseMoveEvent(moveEvent1);
    view->mouseReleaseEvent(releaseEvent);
    view->drawScene()->getItemsMgr()->updateAttributes();

    /**
     * @brief slotShortCutCut　裁剪按钮快捷键
     */
    view->getDrawParam()->setCurrentDrawToolMode(EDrawToolMode::cut);
    c->getLeftToolBar()->slotShortCutCut();
    view->mousePressEvent(pressEvent);
    view->mouseMoveEvent(moveEvent1);
    view->mouseReleaseEvent(releaseEvent);
    view->drawScene()->getItemsMgr()->updateAttributes();
    view->drawScene()->doCutScene();

    /**
     * @brief slotShortCutRound　粘贴图片
     */
    QString path = QApplication::applicationDirPath() + "/test.png";
    QPixmap pix(":/test.png");
    ASSERT_EQ(true, pix.save(path, "PNG"));
    QTest::qWait(1000);
    c->slotLoadDragOrPasteFile(path);
    QTest::qWait(2000);

    // save ddf file
    QString ddfpath = QApplication::applicationDirPath() + "/test_save.ddf";
    QFile file(ddfpath);
    file.open(QIODevice::ReadWrite);
    file.close();
    view->getDrawParam()->setDdfSavePath(ddfpath);
    c->slotSaveToDDF(true);
    QTest::qWait(500);

    // reopen ddf
    view->signalLoadDragOrPasteFile(ddfpath);
    QTest::qWait(500);
}
