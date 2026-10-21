// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for PageScene topBzItem and bounding rect methods (issue V-6027).
 *
 * Target methods:
 *   - topBzItem(pos, penalgor, IncW)
 *   - updateMrItemBoundingRect()
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#define protected public
#define private public
#include "drawshape/cdrawscene.h"
#include "drawshape/drawItems/bzItems/cgraphicsitem.h"
#include "drawshape/drawItems/bzItems/cgraphicsrectitem.h"
#include "drawshape/drawItems/cgraphicsitemselectedmgr.h"
#include "drawshape/globaldefine.h"
#include "publicApi.h"
#undef protected
#undef private

#include <QPointF>
#include <QRectF>

// =========================================================================
// topBzItem
// =========================================================================

TEST(PageSceneTopItem, NoItemAtPos_ReturnsNullptr)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    scene->resetScene();
    CGraphicsItem *result = scene->topBzItem(QPointF(100, 100), false, 0);
    EXPECT_EQ(result, nullptr);
}

TEST(PageSceneTopItem, SingleItem_ReturnsItem)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(100, 100), QPoint(300, 300), false);

    CGraphicsItem *result = scene->topBzItem(QPointF(200, 200), false, 0);
    EXPECT_NE(result, nullptr);
}

TEST(PageSceneTopItem, OverlappingItems_ReturnsTopItem)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    // Create two overlapping items
    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(100, 100), QPoint(300, 300), false);
    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(100, 100), QPoint(300, 300), false);

    auto bzItems = scene->getBzItems();
    ASSERT_GE(bzItems.count(), 2);

    // The first item in bzItems (des-sorted) is the top one
    CGraphicsItem *expectedTop = bzItems.first();

    CGraphicsItem *result = scene->topBzItem(QPointF(200, 200), false, 0);
    ASSERT_NE(result, nullptr);
    EXPECT_EQ(result, expectedTop);
}

TEST(PageSceneTopItem, PenalgorTrue)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(100, 100), QPoint(300, 300), false);

    CGraphicsItem *result = scene->topBzItem(QPointF(200, 200), true, 0);
    EXPECT_NE(result, nullptr);
}

TEST(PageSceneTopItem, PenalgorFalse)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(100, 100), QPoint(300, 300), false);

    CGraphicsItem *result = scene->topBzItem(QPointF(200, 200), false, 0);
    EXPECT_NE(result, nullptr);
}

// =========================================================================
// updateMrItemBoundingRect
// =========================================================================

TEST(PageSceneTopItem, UpdateMrItemBoundingRect_NoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(100, 100), QPoint(200, 200), false);
    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(300, 300), QPoint(400, 400), false);

    auto bzItems = scene->getBzItems();
    ASSERT_GE(bzItems.count(), 2);

    scene->clearSelectGroup();
    for (auto item : bzItems) {
        scene->selectItem(item);
    }

    // Should not crash
    scene->updateMrItemBoundingRect();
    SUCCEED();
}

TEST(PageSceneTopItem, UpdateMrItemBoundingRect_EmptySelectionNoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    scene->clearSelectGroup();
    scene->updateMrItemBoundingRect();
    SUCCEED();
}
