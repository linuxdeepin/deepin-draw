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
#undef protected
#undef private
#include "ccentralwidget.h"
#include "clefttoolbar.h"
#include "toptoolbar.h"
#include "cdrawscene.h"
#include "drawshape/cdrawparamsigleton.h"
#include "drawshape/drawItems/cgraphicsitemselectedmgr.h"
#include "application.h"

#include "crecttool.h"
#include "ccuttool.h"
#include "cellipsetool.h"
#include "cmasicotool.h"
#define protected public
#define private public
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
#include "cgraphicsitemevent.h"

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
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    Page *c = getMainWindow()->drawBoard()->currentPage();
    ASSERT_NE(c, nullptr);

    int oldCount = view->drawScene()->getBzItems().count();

    drawApp->setCurrentTool(pen);

    createItemByMouse(view);


    auto items   = view->drawScene()->getBzItems();

    int nowCount = items.count();

    ASSERT_EQ(nowCount - oldCount, 1);

    foreach (auto item, items) {
        ASSERT_EQ(item->type(), DyLayer);
    }
}

TEST(PenItem, TestPenItemProperty)
{
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    JDynamicLayer *pen = dynamic_cast<JDynamicLayer *>(view->drawScene()->getBzItems().first());
    ASSERT_NE(pen, nullptr);

    view->drawScene()->selectItem(pen);
    setPenWidth(pen, 1);
    ASSERT_EQ(pen->pen().width(), 1);

    // Start Type
//    DComboBox *typeCombox = drawApp->topToolbar()->findChild<DComboBox *>("Line start style combox");
//    ASSERT_NE(typeCombox, nullptr);
//    for (int i = 0; i < typeCombox->count(); i++) {
//        ELineType defaultType = pen->getPenStartType();
//        typeCombox->setCurrentIndex(i);
//        QTest::qWait(300);
//        ASSERT_EQ(pen->getPenStartType(), i);

//        DTestEventList e;
//        e.addKeyPress(Qt::Key_Z, Qt::ControlModifier, 200);
//        e.simulate(view->viewport());
//        ASSERT_EQ(pen->getPenStartType(), defaultType);
//        e.clear();
//        e.addKeyPress(Qt::Key_Y, Qt::ControlModifier, 200);
//        e.simulate(view->viewport());
//        ASSERT_EQ(pen->getPenStartType(), i);
//    }
}

//蜡笔测试
TEST(PenItem, TestCrayon)
{
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    Page *c = getMainWindow()->drawBoard()->currentPage();
    ASSERT_NE(c, nullptr);

    drawApp->setCurrentTool(pen);

    createItemByMouse(view);

    dynamic_cast<CPenTool *>(drawApp->drawBoard()->tool(pen))->m_pPenStyleComboBox->setCurrentIndex(2);

    QTest::qWait(200);

    //坐标测算
    DTestEventList e;
    e.clear();
    e.addMousePress(Qt::MouseButton::LeftButton, Qt::KeyboardModifier::NoModifier, QPoint(1030, 600), 200);
    e.addMouseMove(QPoint(1230, 700), 200);
    e.addMouseRelease(Qt::MouseButton::LeftButton);
    e.simulate(view->viewport());

    QTest::qWait(800);
}

//书法笔测试
TEST(PenItem, TestCalligraphyPen)
{
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    Page *c = getMainWindow()->drawBoard()->currentPage();
    ASSERT_NE(c, nullptr);

    drawApp->setCurrentTool(pen);

    createItemByMouse(view);

    dynamic_cast<CPenTool *>(drawApp->drawBoard()->tool(pen))->m_pPenStyleComboBox->setCurrentIndex(1);

    QTest::qWait(200);

    //坐标测算
    DTestEventList e;
    e.clear();
    e.addMousePress(Qt::MouseButton::LeftButton, Qt::KeyboardModifier::NoModifier, QPoint(1080, 680), 200);
    e.addMouseMove(QPoint(1280, 780), 200);
    e.addMouseRelease(Qt::MouseButton::LeftButton);
    e.simulate(view->viewport());

    QTest::qWait(800);
}

//CPenTool
TEST(PenItem, TestCPenOtherFunction)
{
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    Page *c = getMainWindow()->drawBoard()->currentPage();
    ASSERT_NE(c, nullptr);

    drawApp->setCurrentTool(pen);

    createItemByMouse(view);

    auto tool = dynamic_cast<CPenTool *>(drawApp->drawBoard()->tool(pen));

    QTest::qWait(200);

    //eventfilter
    tool->eventFilter(nullptr, nullptr);

    auto eventFilterObject = const_cast<QAbstractItemView *>(tool->m_pPenStyleComboBox->view());
    QEvent e1(QEvent::Type::Show);
    tool->eventFilter(reinterpret_cast<QObject *>(eventFilterObject), &e1);

    QEvent e2(QEvent::Type::Hide);
    tool->eventFilter(reinterpret_cast<QObject *>(eventFilterObject), &e2);

    QEvent e3(QEvent::Type::None);
    tool->eventFilter(reinterpret_cast<QObject *>(eventFilterObject), &e3);
}

TEST(PenItem, TestSavePenItemToFile)
{
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    Page *c = getMainWindow()->drawBoard()->currentPage();
    ASSERT_NE(c, nullptr);

    // save ddf file
    QString PenItemPath = QApplication::applicationDirPath() + "/test_pen.ddf";
    c->setFile(PenItemPath);
    c->save(true);

    QFileInfo info(PenItemPath);
    ASSERT_TRUE(info.exists());
}

TEST(PenItem, TestOpenPenItemFromFile)
{
    PageView *view = getCurView();
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
    (void)QTest::qWaitFor([ = ]() {
        return (view != getCurView() && getCurView()->drawScene()->getBzItems().count());
    });

    view = getCurView();
    ASSERT_NE(view, nullptr);

    int addedCount = view->drawScene()->getBzItems().count();
    ASSERT_EQ(addedCount, 1);
    view->page()->close(true);
}

TEST(PenItem, TestOtherFunction)
{
    //带起始点的构造函数
    CGraphicsPenItem penItem(QPointF(50.0, 50.0));

    //画笔属性读取
    auto attrList = penItem.attributions();

    //画笔属性设置
    QVariant var;

    var = QColor(1, 1, 1);
    penItem.setAttributionVar(DrawAttribution::EPenColor, var, EChangedBegin);

    var = 1;
    penItem.setAttributionVar(DrawAttribution::EPenWidth, var, EChangedUpdate);

    var = 1;
    penItem.setAttributionVar(DrawAttribution::EStreakEndStyle, var, EChangedUpdate);

    var = 1;
    penItem.setAttributionVar(DrawAttribution::EStreakBeginStyle, var, EChangedUpdate);

    var = 1;
    penItem.setAttributionVar(999, var, 3);

    //缩放
    CGraphItemScalEvent scalEvent;
    penItem.doScaling(&scalEvent);
    penItem.testScaling(&scalEvent);

    //rect获取
    penItem.rect();

    //graphics units获取
    EDataReason reson = EDuplicate;
    auto unit = penItem.getGraphicsUnit(reson);
    unit.release();

    //draw complete
    penItem.updatePenPath(QPointF(60.0, 60.0), true);
    QTest::qWait(200);
    penItem.drawComplete(true);
    QTest::qWait(200);
    penItem.drawComplete(false);
    QTest::qWait(200);

    //get path
    penItem.getPath();
    penItem.getPenStartpath();
    penItem.getPenEndpath();
    penItem.setDrawFlag(true);
}

#endif
