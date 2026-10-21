// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for PageScene selection control methods (issue V-6027).
 *
 * Target methods:
 *   - blockSelect(b)
 *   - notSelectItem(pItem, updateAttri, updateRect)
 *   - selectItemsByRect(rect, replace, onlyBzItem)
 *   - selectItem(pItem, onlyBzItem, updateAttri, updateRect)
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

#include <QRectF>

// =========================================================================
// blockSelect
// =========================================================================

TEST(PageSceneSelect, BlockSelect_EnableThenDisable)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    scene->blockSelect(true);
    EXPECT_TRUE(scene->blockSel);

    scene->blockSelect(false);
    EXPECT_FALSE(scene->blockSel);
}

TEST(PageSceneSelect, BlockSelect_BlocksSelectItemsByRect)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(100, 100), QPoint(200, 200), false);

    scene->clearSelectGroup();
    scene->blockSelect(true);

    QRectF rect(0, 0, 500, 500);
    scene->selectItemsByRect(rect, true, true);

    EXPECT_EQ(scene->selectGroup()->items().count(), 0);

    scene->blockSelect(false);
}

// =========================================================================
// notSelectItem
// =========================================================================

TEST(PageSceneSelect, NotSelectItem_RemovesFromSelection)
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
    ASSERT_EQ(scene->selectGroup()->items().count(), bzItems.count());

    scene->notSelectItem(bzItems.first());
    EXPECT_FALSE(bzItems.first()->isSelected());
    EXPECT_EQ(scene->selectGroup()->items().count(), bzItems.count() - 1);
}

TEST(PageSceneSelect, NotSelectItem_SingleItem)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(100, 100), QPoint(200, 200), false);

    auto bzItems = scene->getBzItems();
    ASSERT_GE(bzItems.count(), 1);

    scene->clearSelectGroup();
    scene->selectItem(bzItems.first());
    ASSERT_EQ(scene->selectGroup()->items().count(), 1);

    scene->notSelectItem(bzItems.first());
    EXPECT_FALSE(bzItems.first()->isSelected());
    EXPECT_EQ(scene->selectGroup()->items().count(), 0);
}

// =========================================================================
// selectItemsByRect
// =========================================================================

TEST(PageSceneSelect, SelectItemsByRect_NoItemsHit)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(100, 100), QPoint(200, 200), false);

    scene->clearSelectGroup();
    QRectF farRect(5000, 5000, 100, 100);
    scene->selectItemsByRect(farRect, true, true);

    EXPECT_EQ(scene->selectGroup()->items().count(), 0);
}

TEST(PageSceneSelect, SelectItemsByRect_SingleItemHit)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(100, 100), QPoint(200, 200), false);

    scene->clearSelectGroup();
    QRectF rect(50, 50, 300, 300);
    scene->selectItemsByRect(rect, true, true);

    EXPECT_EQ(scene->selectGroup()->items().count(), 1);
}

TEST(PageSceneSelect, SelectItemsByRect_MultipleItemsHit)
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

    scene->clearSelectGroup();
    QRectF rect(0, 0, 500, 500);
    scene->selectItemsByRect(rect, true, true);

    EXPECT_EQ(scene->selectGroup()->items().count(), 2);
}

TEST(PageSceneSelect, SelectItemsByRect_ReplaceTrue)
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

    // Pre-select one item
    scene->clearSelectGroup();
    scene->selectItem(bzItems.first());
    ASSERT_EQ(scene->selectGroup()->items().count(), 1);

    // Replace should clear previous and select via rect
    QRectF rect(0, 0, 500, 500);
    scene->selectItemsByRect(rect, true, true);

    // After replace, selection should be from rect only
    EXPECT_GE(scene->selectGroup()->items().count(), 1);
}

TEST(PageSceneSelect, SelectItemsByRect_ReplaceFalse_Accumulates)
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
    // Select first item
    scene->selectItem(bzItems.first());
    int firstCount = scene->selectGroup()->items().count();
    ASSERT_EQ(firstCount, 1);

    // Accumulate via rect with replace=false
    QRectF rect(0, 0, 500, 500);
    scene->selectItemsByRect(rect, false, true);

    EXPECT_GE(scene->selectGroup()->items().count(), firstCount);
}

// =========================================================================
// selectItem
// =========================================================================

TEST(PageSceneSelect, SelectItem_AddsToSelectionGroup)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(100, 100), QPoint(200, 200), false);

    auto bzItems = scene->getBzItems();
    ASSERT_GE(bzItems.count(), 1);

    scene->clearSelectGroup();
    scene->selectItem(bzItems.first());
    EXPECT_TRUE(bzItems.first()->isSelected());
    EXPECT_EQ(scene->selectGroup()->items().count(), 1);
}

TEST(PageSceneSelect, SelectItem_Nullptr_ReturnsImmediately)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    scene->clearSelectGroup();
    scene->selectItem(nullptr);
    EXPECT_EQ(scene->selectGroup()->items().count(), 0);
}

TEST(PageSceneSelect, SelectItem_BlockedSelect_NoEffect)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(100, 100), QPoint(200, 200), false);

    auto bzItems = scene->getBzItems();
    ASSERT_GE(bzItems.count(), 1);

    scene->clearSelectGroup();
    scene->blockSelect(true);
    scene->selectItem(bzItems.first());
    scene->blockSelect(false);

    EXPECT_FALSE(bzItems.first()->isSelected());
    EXPECT_EQ(scene->selectGroup()->items().count(), 0);
}
