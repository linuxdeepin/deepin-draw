// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for PageScene touch events, group tree, z-order,
 * and null-param guards (issue V-9310 §4).
 *
 * Source: src/drawshape/cdrawscene.cpp
 *
 * Target paths:
 *   doLeave (283-284): interrupt queued lambda
 *   Touch events with real touch points (299-303, 315-341)
 *   getGroupTreeInfo(pGroup, ESaveToDDf, true) (467-473)
 *   loadGroupTreeInfo(info) (530-532)
 *   moveItemsZDown/Up within group (593-595, 611-613, 666-668)
 *   addCItem(nullptr) (866-867), removeCItem(nullptr)/foreign (889-895)
 *   firstItem with penalgor/seeNodeAsBzItem (1349-1350, 1410-1411)
 *   recordSecenInfoToCmd(EItemAdded) → early return (1491-1492)
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QImage>
#include <QPointF>
#include <QTouchEvent>
#include <QEventPoint>
#include <QGraphicsScene>

#define protected public
#define private public
#include "cdrawscene.h"
#include "drawshape/drawItems/bzItems/cgraphicsitem.h"
#include "drawshape/drawItems/bzItems/cgraphicsitemevent.h"
#include "drawshape/drawItems/bzItems/cgraphicsrectitem.h"
#include "drawshape/drawItems/cgraphicsitemselectedmgr.h"
#include "drawshape/drawItems/cgraphicsitemselectedmgr.h"
#include "drawshape/sitemdata.h"
#include "drawshape/globaldefine.h"
#include "publicApi.h"
#include "frame/cundoredocommand.h"
#undef protected
#undef private

#include "test_helpers.h"

// ===========================================================================
// doLeave (283-284): interrupt via queued lambda
// ===========================================================================

TEST_F(PageStateGuard, DoLeave_InterruptsWorkingTool)
{
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);
    Page *page = view->page();
    ASSERT_NE(page, nullptr);

    // Start a rectangle tool to enter isWorking() state.
    page->setCurrentTool(rectangle);
    auto *tool = page->currentTool_p();
    ASSERT_NE(tool, nullptr);

    // Simulate tool start by drawing (mouse press).
    createItemByMouse(view, false, QPoint(100, 100), QPoint(150, 150), false);
    qApp->processEvents();

    // Call doLeave which queues an interrupt.
    scene->doLeave();
    qApp->processEvents();

    // After processing, the tool should no longer be working.
    EXPECT_FALSE(tool->isWorking());
}

// ===========================================================================
// Touch events with real touch points (299-303, 315-341)
// ===========================================================================

TEST_F(PageStateGuard, TouchEvent_TwoPoints_SelectionTool_Interrupts)
{
    // Target: touch event with 2 touch points + selection tool → interrupt() (299-303).
    //
    // PageScene::event() calls page()->currentTool_p() at line 298 without a
    // null check on page().  In the offscreen test environment, page() returns
    // a valid Page outside event() but returns nullptr inside event() — likely
    // because the deprecated QTouchEvent::touchPoints() triggers internal Qt
    // event processing that invalidates the view/scene/page chain.  This path
    // cannot be reliably covered without modifying business code.
    GTEST_SKIP() << "PageScene::event() with real touch points crashes in "
                    "offscreen mode: page() returns null inside event() despite "
                    "being valid outside. Requires source fix (null check) or "
                    "a different test approach.";
}

TEST_F(PageStateGuard, TouchEvent_SinglePoint_BeginUpdateEnd_NoCrash)
{
    // Target: single touch point Begin/Update/End cycle → toolDoStart/Update/
    // Finish + clearITE (315-341).
    //
    // Same crash issue as TouchEvent_TwoPoints: PageScene::event() calls
    // page()->currentTool_p() without null check, and page() returns null
    // inside event() in the offscreen environment.
    GTEST_SKIP() << "PageScene::event() with real touch points crashes in "
                    "offscreen mode: page() returns null inside event() despite "
                    "being valid outside. Requires source fix (null check) or "
                    "a different test approach.";
}

// ===========================================================================
// getGroupTreeInfo(pGroup, ESaveToDDf, true) (467-473)
// ===========================================================================

TEST_F(PageStateGuard, GetGroupTreeInfo_WithGroup_ProcessEventTrue)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);
    PageContext *ctx = scene->pageContext();
    ASSERT_NE(ctx, nullptr);

    QImage img(16, 16, QImage::Format_ARGB32);
    img.fill(Qt::red);
    ctx->addImage(img, QPointF(0, 0), QRectF(0, 0, 16, 16), false, false);
    ctx->addImage(img, QPointF(20, 0), QRectF(0, 0, 16, 16), false, false);
    qApp->processEvents();

    QList<CGraphicsItem *> bzItems;
    for (auto *it : scene->items()) {
        auto *bz = dynamic_cast<CGraphicsItem *>(it);
        if (bz != nullptr && bz->isBzItem())
            bzItems.append(bz);
    }
    ASSERT_GE(bzItems.size(), 2);

    CGraphicsItemGroup *group = scene->creatGroup(bzItems);
    ASSERT_NE(group, nullptr);

    auto info = scene->getGroupTreeInfo(group, ESaveToDDf, true);
    EXPECT_GE(info.bzItems.size(), 2);

    PageScene::releaseBzItemsTreeInfo(info);
}

// ===========================================================================
// loadGroupTreeInfo(info) (530-532)
// Construct a CGroupBzItemsTreeInfo with a rect CGraphicsUnit and a child
// group info, then load it — the child group should be added to items.
// ===========================================================================

TEST_F(PageStateGuard, LoadGroupTreeInfo_WithChildGroup_ReturnsGroup)
{
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    // Build a tree info with two rectangle items (creatGroup requires >1
    // item for ENormalGroup, so use EVirRootGroup to avoid the restriction).
    CGroupBzItemsTreeInfo info;
    info.setGroupType(CGraphicsItemGroup::EVirRootGroup);

    CGraphicsUnit unit1;
    unit1.head.dataType = RectType;
    unit1.head.pos = QPointF(10, 10);
    unit1.head.zValue = 0;
    unit1.head.rotate = 0.0;
    unit1.head.trans = QTransform();
    info.bzItems.append(unit1);

    CGraphicsUnit unit2;
    unit2.head.dataType = RectType;
    unit2.head.pos = QPointF(50, 50);
    unit2.head.zValue = 1;
    unit2.head.rotate = 0.0;
    unit2.head.trans = QTransform();
    info.bzItems.append(unit2);

    // Add a child group info with one rectangle item.
    CGroupBzItemsTreeInfo childInfo;
    childInfo.setGroupType(CGraphicsItemGroup::EVirRootGroup);
    CGraphicsUnit childUnit;
    childUnit.head.dataType = RectType;
    childUnit.head.pos = QPointF(100, 100);
    childUnit.head.zValue = 0;
    childUnit.head.rotate = 0.0;
    childUnit.head.trans = QTransform();
    childInfo.bzItems.append(childUnit);
    info.childGroups.append(childInfo);

    CGraphicsItemGroup *result = scene->loadGroupTreeInfo(info, true);
    ASSERT_NE(result, nullptr);
    EXPECT_GE(result->getBzItems().count(), 1);

    PageScene::releaseBzItemsTreeInfo(info);
}

// ===========================================================================
// Group-internal z-order: moveItemsZDown/Up within a group
// (611-613, 666-668)
// ===========================================================================

TEST_F(PageStateGuard, MoveItemsZDown_WithinGroup_ChangesZOrder)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);
    PageContext *ctx = scene->pageContext();
    ASSERT_NE(ctx, nullptr);

    QImage img(16, 16, QImage::Format_ARGB32);
    img.fill(Qt::red);
    ctx->addImage(img, QPointF(0, 0), QRectF(0, 0, 16, 16), false, false);
    ctx->addImage(img, QPointF(20, 0), QRectF(0, 0, 16, 16), false, false);
    ctx->addImage(img, QPointF(40, 0), QRectF(0, 0, 16, 16), false, false);
    qApp->processEvents();

    QList<CGraphicsItem *> bzItems;
    for (auto *it : scene->items()) {
        auto *bz = dynamic_cast<CGraphicsItem *>(it);
        if (bz != nullptr && bz->isBzItem())
            bzItems.append(bz);
    }
    ASSERT_GE(bzItems.size(), 3);

    CGraphicsItemGroup *group = scene->creatGroup(bzItems);
    ASSERT_NE(group, nullptr);

    qreal zBefore = bzItems[0]->zValue();
    scene->moveItemsZDown({bzItems[0]}, 1);
    qreal zAfter = bzItems[0]->zValue();
    EXPECT_NE(zBefore, zAfter);
}

TEST_F(PageStateGuard, MoveItemsZUp_WithinGroup_ChangesZOrder)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);
    PageContext *ctx = scene->pageContext();
    ASSERT_NE(ctx, nullptr);

    QImage img(16, 16, QImage::Format_ARGB32);
    img.fill(Qt::blue);
    ctx->addImage(img, QPointF(0, 0), QRectF(0, 0, 16, 16), false, false);
    ctx->addImage(img, QPointF(20, 0), QRectF(0, 0, 16, 16), false, false);
    ctx->addImage(img, QPointF(40, 0), QRectF(0, 0, 16, 16), false, false);
    qApp->processEvents();

    QList<CGraphicsItem *> bzItems;
    for (auto *it : scene->items()) {
        auto *bz = dynamic_cast<CGraphicsItem *>(it);
        if (bz != nullptr && bz->isBzItem())
            bzItems.append(bz);
    }
    ASSERT_GE(bzItems.size(), 3);

    CGraphicsItemGroup *group = scene->creatGroup(bzItems);
    ASSERT_NE(group, nullptr);

    qreal zBefore = bzItems[2]->zValue();
    scene->moveItemsZUp({bzItems[2]}, 1);
    qreal zAfter = bzItems[2]->zValue();
    EXPECT_NE(zBefore, zAfter);
}

// ===========================================================================
// addCItem(nullptr) (866-867), removeCItem(nullptr)/foreign (889-895)
// ===========================================================================

TEST_F(PageStateGuard, AddCItem_NullPtr_NoCrash_NoChange)
{
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    int countBefore = scene->items().size();
    scene->addCItem(nullptr);
    EXPECT_EQ(scene->items().size(), countBefore);
}

TEST_F(PageStateGuard, RemoveCItem_NullPtr_NoCrash_NoChange)
{
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    int countBefore = scene->items().size();
    scene->removeCItem(nullptr);
    EXPECT_EQ(scene->items().size(), countBefore);
}

TEST_F(PageStateGuard, RemoveCItem_ForeignItem_NoCrash_NoChange)
{
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    // Create a rect item that is NOT in this scene.
    CGraphicsRectItem foreignItem(QRectF(0, 0, 50, 50));

    int countBefore = scene->items().size();
    scene->removeCItem(&foreignItem);
    EXPECT_EQ(scene->items().size(), countBefore);
}

// ===========================================================================
// firstItem with penalgor=true, incW=5 (1349-1350)
// ===========================================================================

TEST_F(PageStateGuard, FirstItem_Penalgor_IncW5_HitsOpaqueItem)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);
    PageContext *ctx = scene->pageContext();
    ASSERT_NE(ctx, nullptr);

    QImage img(50, 50, QImage::Format_ARGB32);
    img.fill(Qt::red);
    ctx->addImage(img, QPointF(0, 0), QRectF(0, 0, 50, 50), false, false);
    qApp->processEvents();

    auto sceneItems = scene->items();
    QList<CGraphicsItem *> bzItems;
    for (auto *it : sceneItems) {
        auto *bz = dynamic_cast<CGraphicsItem *>(it);
        if (bz != nullptr && bz->isBzItem())
            bzItems.append(bz);
    }
    ASSERT_FALSE(bzItems.isEmpty());

    QPointF pos = bzItems.first()->mapToScene(bzItems.first()->boundingRect().center());
    QGraphicsItem *result = scene->firstItem(pos, sceneItems, true, true, true, 5);
    EXPECT_NE(result, nullptr);
}

// ===========================================================================
// recordSecenInfoToCmd(EItemAdded) → early return (1491-1492)
// ===========================================================================

TEST_F(PageStateGuard, RecordSecenInfoToCmd_EItemAdded_ReturnsEarly)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    QImage img(16, 16, QImage::Format_ARGB32);
    img.fill(Qt::green);
    scene->pageContext()->addImage(img, QPointF(0, 0), QRectF(0, 0, 16, 16), false, false);
    qApp->processEvents();

    QList<CGraphicsItem *> items;
    for (auto *it : scene->items()) {
        auto *bz = dynamic_cast<CGraphicsItem *>(it);
        if (bz != nullptr && bz->isBzItem())
            items.append(bz);
    }
    ASSERT_FALSE(items.isEmpty());

    // EItemAdded is not EGroupChanged → should return early without recording.
    int undoBefore = 0;
    MainWindow *mw = drawApp->topMainWindow();
    if (mw != nullptr && mw->drawBoard() != nullptr) {
        Page *page = view->page();
        if (page != nullptr && view->m_pUndoStack != nullptr)
            undoBefore = view->m_pUndoStack->count();
    }

    scene->recordSecenInfoToCmd(
        CSceneUndoRedoCommand::EItemAdded, UndoVar, items);

    int undoAfter = 0;
    if (mw != nullptr && mw->drawBoard() != nullptr) {
        Page *page = view->page();
        if (page != nullptr && view->m_pUndoStack != nullptr)
            undoAfter = view->m_pUndoStack->count();
    }

    EXPECT_EQ(undoBefore, undoAfter);
}
