// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

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

#include <QtTest>
#include <QTestEventList>

#include "../testItems/publicApi.h"

#if TEST_ITEMALIGNMENT_ITEM

TEST(ItemAlignment, TestItemAlignmentCreateView)
{
    createNewViewByShortcutKey();
}

TEST(ItemAlignment, TestSingleItemAlignment)
{
    // [0] create item
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    Page *c = getMainWindow()->drawBoard()->currentPage();
    ASSERT_NE(c, nullptr);

    drawApp->setCurrentTool(rectangle);

    int addedCount = view->drawScene()->getBzItems().count();
    createItemByMouse(view);
    ASSERT_EQ(view->drawScene()->getBzItems().count(), addedCount + 1);

    ASSERT_EQ(view->drawScene()->getBzItems().first()->type(), RectType);

    itemAlignment();
}

TEST(ItemAlignment, TestMutilItemAlignment)
{
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    int delay = 50;

    // [0] 添加图元
    int addedCount = view->drawScene()->getBzItems().count();
    DTestEventList e;
    e.addKeyPress(Qt::Key_A, Qt::ControlModifier, delay);
    e.addKeyPress(Qt::Key_C, Qt::ControlModifier, delay);
    e.addKeyPress(Qt::Key_V, Qt::ControlModifier, delay);
    e.addKeyPress(Qt::Key_V, Qt::ControlModifier, delay);
    e.addKeyRelease(Qt::Key_A, Qt::ControlModifier, delay);
    e.addKeyRelease(Qt::Key_C, Qt::ControlModifier, delay);
    e.addKeyRelease(Qt::Key_V, Qt::ControlModifier, delay);
    e.addKeyPress(Qt::Key_A, Qt::ControlModifier, delay);
    e.addKeyRelease(Qt::Key_A, Qt::ControlModifier, delay);
    e.simulate(view->viewport());
    ASSERT_EQ(view->drawScene()->getBzItems().count(), addedCount + 2);

    // [1] 普通对齐
    itemAlignment();

    // [2] 水平居中对齐
    QAction *hAction = view->findChild<QAction *>("DistributeHorizontalSpace");
    ASSERT_NE(hAction, nullptr);
    hAction->triggered();

    // [3] 垂直居中对齐
    QAction *vAction  = view->findChild<QAction *>("DistributeVerticalSpace");
    ASSERT_NE(vAction, nullptr);
    vAction->triggered();

    //  [4] 不同的对齐方式
    CGraphicsItem *pItem = dynamic_cast<CGraphicsItem *>(view->drawScene()->getBzItems().first());
    ASSERT_NE(pItem, nullptr);
    // view->drawScene()->clearMrSelection();
    view->drawScene()->selectItem(pItem);
    QTest::qWait(delay);
    hAction->triggered();
    QTest::qWait(delay);
    vAction->triggered();
    QTest::qWait(delay);
    pItem->moveBy(-200, -100);
    view->slotOnSelectAll();
    QTest::qWait(delay);
    hAction->triggered();
    QTest::qWait(delay);
    vAction->triggered();

    //  [5] 右键菜单
    // view->drawScene()->clearMrSelection();
    QContextMenuEvent event(QContextMenuEvent::Mouse, QPoint(100, 100));
    dApp->sendEvent(view->viewport(), &event);
    QTest::qWait(200);
    e.clear();
    e.addKeyPress(Qt::Key_Escape, Qt::NoModifier, delay);
    e.addKeyRelease(Qt::Key_Escape, Qt::NoModifier, delay);
    e.simulate(dApp->activePopupWidget());
    QTest::qWait(100);
    view->drawScene()->selectItemsByRect(view->drawScene()->sceneRect());
    dApp->sendEvent(view->viewport(), &event);
    QTest::qWait(200);
    e.simulate(dApp->activePopupWidget());
    QTest::qWait(100);
}

TEST(ItemAlignment, TestSaveItemAlignmentToFile)
{
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    Page *c = getMainWindow()->drawBoard()->currentPage();
    ASSERT_NE(c, nullptr);

    // save ddf file
    QString path = QApplication::applicationDirPath() + "/test_itemAlignment.ddf";
    c->setFile(path);
    c->save();
    c->close(true);
    QTest::qWait(100);

    QFileInfo info(path);
    ASSERT_TRUE(info.exists());
}
TEST(ItemAlignment, TestOpenItemAlignmentFromFile)
{
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    // 打开保存绘制的 ddf
    QString path = QApplication::applicationDirPath() + "/test_itemAlignment.ddf";

    QMimeData mimedata;
    QList<QUrl> li;
    li.append(QUrl(path));
    mimedata.setUrls(li);

    const QPoint pos = view->viewport()->rect().center();
    QDragEnterEvent eEnter(pos, Qt::IgnoreAction, &mimedata, Qt::LeftButton, Qt::NoModifier);
    dApp->sendEvent(view->viewport(), &eEnter);

    QDropEvent e(pos, Qt::IgnoreAction, &mimedata, Qt::LeftButton, Qt::NoModifier);
    dApp->sendEvent(view->viewport(), &e);
    QTest::qWait(100);

    qMyWaitFor([ = ]() {return (view != getCurView());});

    view = getCurView();

    ASSERT_NE(view, nullptr);

    view->page()->close(true);
}

#endif
