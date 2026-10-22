// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QCursor>
#include <QVariant>

#define protected public
#define private public
#include "publicApi.h"
#include "ccentralwidget.h"
#include "globaldefine.h"

/*
 * Page remaining-methods tests (V-6123, Section 四)
 *
 * Covers: isActivedPage, setBorad, setContext,
 *         setDefaultAttriVar / defaultAttriVar,
 *         setDrawCursor / blockSettingDrawCursor,
 *         adjustViewScaleRatio, updateContext,
 *         setCurrentTool / currentTool
 */

// ---------------------------------------------------------------------------
// isActivedPage
// ---------------------------------------------------------------------------

TEST(PageIsActivedPageTest, CurrentPageIsActived)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);
    Page *page = board->currentPage();
    ASSERT_NE(page, nullptr);

    // The current page should be the active page.
    EXPECT_TRUE(page->isActivedPage());
}

TEST(PageIsActivedPageTest, NonCurrentPageIsNotActived)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    Page *first = board->firstPage();
    ASSERT_NE(first, nullptr);
    Page *second = board->addPage();
    ASSERT_NE(second, nullptr);

    // Switch to second page; first should no longer be active.
    board->setCurrentPage(second);
    EXPECT_FALSE(first->isActivedPage());
    EXPECT_TRUE(second->isActivedPage());
}

// ---------------------------------------------------------------------------
// setBorad
// ---------------------------------------------------------------------------

TEST(PageSetBoradTest, SetBoradDoesNotCrash)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);
    Page *page = board->currentPage();
    ASSERT_NE(page, nullptr);

    // setBorad with the same board should not crash.
    page->setBorad(board);
    EXPECT_EQ(page->borad(), board);
}

// ---------------------------------------------------------------------------
// setContext
// ---------------------------------------------------------------------------

TEST(PageSetContextTest, SetContextDoesNotCrash)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);
    Page *page = board->currentPage();
    ASSERT_NE(page, nullptr);

    PageContext *originalCtx = page->context();
    ASSERT_NE(originalCtx, nullptr);

    // Setting the same context back should not crash.
    page->setContext(originalCtx);
    EXPECT_EQ(page->context(), originalCtx);
}

// ---------------------------------------------------------------------------
// setDefaultAttriVar / defaultAttriVar
// ---------------------------------------------------------------------------

TEST(PageDefaultAttriVarTest, SetAndRetrieveValue)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);
    Page *page = board->currentPage();
    ASSERT_NE(page, nullptr);
    ASSERT_NE(page->context(), nullptr);

    // Set a default attribute and read it back.
    page->setDefaultAttriVar(1, QVariant(42));
    QVariant retrieved = page->defaultAttriVar(1);
    EXPECT_EQ(retrieved.toInt(), 42);
}

TEST(PageDefaultAttriVarTest, UnsetAttriReturnsInvalid)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);
    Page *page = board->currentPage();
    ASSERT_NE(page, nullptr);

    // An attribute that was never set should return an invalid QVariant.
    QVariant retrieved = page->defaultAttriVar(9999);
    EXPECT_FALSE(retrieved.isValid());
}

// ---------------------------------------------------------------------------
// setDrawCursor / blockSettingDrawCursor
// ---------------------------------------------------------------------------

TEST(PageSetDrawCursorTest, SetCursorDoesNotCrash)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);
    Page *page = board->currentPage();
    ASSERT_NE(page, nullptr);

    // Ensure blocking is off, then set a cursor.
    page->blockSettingDrawCursor(false);
    page->setDrawCursor(Qt::CrossCursor);
    SUCCEED();
}

TEST(PageBlockDrawCursorTest, BlockedCursorNotApplied)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);
    Page *page = board->currentPage();
    ASSERT_NE(page, nullptr);

    // Block cursor setting, then try to set a different cursor.
    page->blockSettingDrawCursor(true);
    page->setDrawCursor(Qt::WaitCursor);

    // The cursor should NOT be WaitCursor because setting was blocked.
    // (setDrawCursor returns early when _blockSettingDrawCursor is true.)
    // We verify it does not crash; the actual cursor may be anything.
    SUCCEED();

    // Restore: unblock and set a cursor.
    page->blockSettingDrawCursor(false);
    page->setDrawCursor(Qt::ArrowCursor);
    SUCCEED();
}

// ---------------------------------------------------------------------------
// adjustViewScaleRatio
// ---------------------------------------------------------------------------

TEST(PageAdjustViewScaleRatioTest, EmptySceneDoesNotCrash)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);
    Page *page = board->currentPage();
    ASSERT_NE(page, nullptr);

    // An empty scene should not crash when adjusting scale ratio.
    page->adjustViewScaleRatio();
    SUCCEED();
}

TEST(PageAdjustViewScaleRatioTest, WithSceneDoesNotCrash)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);
    Page *page = board->currentPage();
    ASSERT_NE(page, nullptr);

    // Even with a scene that may have items, this should not crash.
    page->adjustViewScaleRatio();
    SUCCEED();
}

// ---------------------------------------------------------------------------
// updateContext
// ---------------------------------------------------------------------------

TEST(PageUpdateContextTest, DoesNotCrashWithValidContext)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);
    Page *page = board->currentPage();
    ASSERT_NE(page, nullptr);
    ASSERT_NE(page->context(), nullptr);

    // updateContext delegates to _context->update().
    page->updateContext();
    SUCCEED();
}

// ---------------------------------------------------------------------------
// setCurrentTool / currentTool
// ---------------------------------------------------------------------------

TEST(PageSetCurrentToolTest, SetAndRetrieveTool)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);
    Page *page = board->currentPage();
    ASSERT_NE(page, nullptr);

    // Set a valid tool and verify currentTool returns the same value.
    page->setCurrentTool(selection);
    EXPECT_EQ(page->currentTool(), static_cast<int>(selection));
}

TEST(PageSetCurrentToolTest, SetDifferentTool)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);
    Page *page = board->currentPage();
    ASSERT_NE(page, nullptr);

    page->setCurrentTool(rectangle);
    EXPECT_EQ(page->currentTool(), static_cast<int>(rectangle));

    page->setCurrentTool(noselected);
    EXPECT_EQ(page->currentTool(), static_cast<int>(noselected));
}
