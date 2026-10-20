// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for PageView slots (src/frame/cgraphicsview.cpp).
 *
 * Covers (issue V-5981 §五):
 *   - slotBringToFront / slotSendTobackAct / slotOneLayerUp / slotOneLayerDown
 *   - slotOnCopy / slotOnCut / slotOnPaste / slotOnDelete
 *   - slotOnTextCopy / slotOnTextCut / slotOnTextPaste / slotOnTextDelete
 *   - slotOnTextUndo / slotOnTextRedo / slotOnTextSelectAll
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#define protected public
#define private public
#include "publicApi.h"
#undef protected
#undef private

// Helper: create two rectangle items on the current view.
static void createTwoItems(PageView *view, PageScene *scene)
{
    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(100, 100), QPoint(200, 200), false);
    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(300, 300), QPoint(400, 400), false);
}

// ===========================================================================
// Layer slots
// ===========================================================================

TEST(PageViewLayerSlotTest, SlotBringToFront_NoSelection_NoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    view->slotBringToFront();
    SUCCEED();
}

TEST(PageViewLayerSlotTest, SlotBringToFront_WithSelection_NoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    createTwoItems(view, scene);
    ASSERT_GE(scene->getBzItems().count(), 2);
    view->slotOnSelectAll();

    view->slotBringToFront();
    SUCCEED();
}

TEST(PageViewLayerSlotTest, SlotSendTobackAct_NoSelection_NoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    view->slotSendTobackAct();
    SUCCEED();
}

TEST(PageViewLayerSlotTest, SlotSendTobackAct_WithSelection_NoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    createTwoItems(view, scene);
    ASSERT_GE(scene->getBzItems().count(), 2);
    view->slotOnSelectAll();

    view->slotSendTobackAct();
    SUCCEED();
}

TEST(PageViewLayerSlotTest, SlotOneLayerUp_NoSelection_NoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    view->slotOneLayerUp();
    SUCCEED();
}

TEST(PageViewLayerSlotTest, SlotOneLayerUp_WithSelection_NoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    createTwoItems(view, scene);
    ASSERT_GE(scene->getBzItems().count(), 2);
    view->slotOnSelectAll();

    view->slotOneLayerUp();
    SUCCEED();
}

TEST(PageViewLayerSlotTest, SlotOneLayerDown_NoSelection_NoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    view->slotOneLayerDown();
    SUCCEED();
}

TEST(PageViewLayerSlotTest, SlotOneLayerDown_WithSelection_NoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    createTwoItems(view, scene);
    ASSERT_GE(scene->getBzItems().count(), 2);
    view->slotOnSelectAll();

    view->slotOneLayerDown();
    SUCCEED();
}

// ===========================================================================
// Edit slots: copy / cut / paste / delete
// ===========================================================================

TEST(PageViewEditSlotTest, SlotOnCopy_NoSelection_NoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    view->slotOnCopy();
    SUCCEED();
}

TEST(PageViewEditSlotTest, SlotOnCopy_WithSelection_NoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    createTwoItems(view, scene);
    ASSERT_GE(scene->getBzItems().count(), 2);
    view->slotOnSelectAll();

    view->slotOnCopy();
    SUCCEED();
}

TEST(PageViewEditSlotTest, SlotOnCut_NoSelection_NoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    view->slotOnCut();
    SUCCEED();
}

TEST(PageViewEditSlotTest, SlotOnCut_WithSelection_DecreasesItemCount)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    createTwoItems(view, scene);
    int countBefore = scene->getBzItems().count();
    ASSERT_GE(countBefore, 2);
    view->slotOnSelectAll();

    view->slotOnCut();
    qApp->processEvents();
    EXPECT_LT(scene->getBzItems().count(), countBefore);
}

TEST(PageViewEditSlotTest, SlotOnPaste_AfterCopy_IncreasesItemCount)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    createTwoItems(view, scene);
    int countBefore = scene->getBzItems().count();
    ASSERT_GE(countBefore, 2);
    view->slotOnSelectAll();

    view->slotOnCopy();
    qApp->processEvents();

    // Deselect then paste.
    scene->clearSelectGroup();
    view->slotOnPaste();
    qApp->processEvents();
    EXPECT_GT(scene->getBzItems().count(), countBefore);
}

TEST(PageViewEditSlotTest, SlotOnDelete_NoSelection_NoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    view->slotOnDelete();
    SUCCEED();
}

TEST(PageViewEditSlotTest, SlotOnDelete_WithSelection_DecreasesItemCount)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    createTwoItems(view, scene);
    int countBefore = scene->getBzItems().count();
    ASSERT_GE(countBefore, 2);
    view->slotOnSelectAll();

    view->slotOnDelete();
    qApp->processEvents();
    EXPECT_LT(scene->getBzItems().count(), countBefore);
}

// ===========================================================================
// Text slots — non-editing state should not crash
// ===========================================================================

TEST(PageViewTextSlotTest, SlotOnTextCopy_NotEditing_NoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    view->slotOnTextCopy();
    SUCCEED();
}

TEST(PageViewTextSlotTest, SlotOnTextCut_NotEditing_NoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    view->slotOnTextCut();
    SUCCEED();
}

TEST(PageViewTextSlotTest, SlotOnTextPaste_NotEditing_NoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    view->slotOnTextPaste();
    SUCCEED();
}

TEST(PageViewTextSlotTest, SlotOnTextDelete_NotEditing_NoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    view->slotOnTextDelete();
    SUCCEED();
}

TEST(PageViewTextSlotTest, SlotOnTextUndo_NotEditing_NoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    view->slotOnTextUndo();
    SUCCEED();
}

TEST(PageViewTextSlotTest, SlotOnTextRedo_NotEditing_NoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    view->slotOnTextRedo();
    SUCCEED();
}

TEST(PageViewTextSlotTest, SlotOnTextSelectAll_NotEditing_NoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    view->slotOnTextSelectAll();
    SUCCEED();
}
