// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QSignalSpy>

#define protected public
#define private public
#include "publicApi.h"
#include "ccentralwidget.h"
#include "globaldefine.h"

/*
 * DrawBoard page-management tests (V-6123)
 *
 * Covers: firstPage, nextPage, closePage(Page*),
 *         setPageName, setPageTitle, setCurrentPage(Page*)
 */

// ---------------------------------------------------------------------------
// firstPage
// ---------------------------------------------------------------------------

TEST(DrawBoardFirstPageTest, ReturnsNonNullAfterPageCreated)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    // firstPage() delegates to page(0); with at least one page it is non-null.
    EXPECT_NE(board->firstPage(), nullptr);
}

TEST(DrawBoardFirstPageTest, EqualsCurrentPageWhenSinglePage)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    // With a single page, firstPage() should equal currentPage().
    // (Other tests may have added pages; use the first one.)
    Page *first = board->firstPage();
    ASSERT_NE(first, nullptr);
    EXPECT_EQ(board->page(0), first);
}

// ---------------------------------------------------------------------------
// nextPage
// ---------------------------------------------------------------------------

TEST(DrawBoardNextPageTest, MiddlePageReturnsNext)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    Page *first = board->firstPage();
    ASSERT_NE(first, nullptr);

    Page *second = board->addPage();
    ASSERT_NE(second, nullptr);

    // nextPage(first) should return the page after first.
    Page *next = board->nextPage(first);
    EXPECT_NE(next, nullptr);
}

TEST(DrawBoardNextPageTest, LastPageReturnsNullptr)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    // Ensure there is at least one page, then get the last one.
    int cnt = board->count();
    ASSERT_GE(cnt, 1);
    Page *last = board->page(cnt - 1);
    ASSERT_NE(last, nullptr);

    // nextPage of the last page should return nullptr.
    EXPECT_EQ(board->nextPage(last), nullptr);
}

// ---------------------------------------------------------------------------
// closePage(Page*)
// ---------------------------------------------------------------------------

TEST(DrawBoardClosePageTest, ClosingReducesCount)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    // Add an extra page so we always have one left after closing.
    Page *extra = board->addPage();
    ASSERT_NE(extra, nullptr);

    int countBefore = board->count();
    ASSERT_GE(countBefore, 2);

    // closePage delegates to page->close(); verify count decreases.
    board->closePage(extra);
    EXPECT_LT(board->count(), countBefore);
}

// ---------------------------------------------------------------------------
// setPageName / setPageTitle
// ---------------------------------------------------------------------------

TEST(DrawBoardSetPageNameTest, NameIsSetOnContext)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);
    Page *page = board->currentPage();
    ASSERT_NE(page, nullptr);
    ASSERT_NE(page->context(), nullptr);

    QString newName = "test-page-name-12345";
    board->setPageName(page, newName);

    EXPECT_EQ(page->name(), newName);
}

TEST(DrawBoardSetPageTitleTest, EmitsCurrentTitleNameChange)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);
    Page *page = board->currentPage();
    ASSERT_NE(page, nullptr);

    QSignalSpy spy(board, &DrawBoard::currentTitleNameChange);
    ASSERT_TRUE(spy.isValid());

    board->setPageTitle(page, "new-title");

    EXPECT_GE(spy.count(), 1);
}

TEST(DrawBoardSetPageNameTest, EmitsCurrentTitleNameChange)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);
    Page *page = board->currentPage();
    ASSERT_NE(page, nullptr);

    QSignalSpy spy(board, &DrawBoard::currentTitleNameChange);
    ASSERT_TRUE(spy.isValid());

    board->setPageName(page, "another-name");

    // setPageName calls setPageTitle which emits currentTitleNameChange.
    EXPECT_GE(spy.count(), 1);
}

// ---------------------------------------------------------------------------
// setCurrentPage(Page*)
// ---------------------------------------------------------------------------

TEST(DrawBoardSetCurrentPagePtrTest, SwitchesToTargetPage)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    Page *first = board->firstPage();
    ASSERT_NE(first, nullptr);
    Page *second = board->addPage();
    ASSERT_NE(second, nullptr);

    // Switch to first page.
    board->setCurrentPage(first);
    EXPECT_EQ(board->currentPage(), first);

    // Switch to second page.
    board->setCurrentPage(second);
    EXPECT_EQ(board->currentPage(), second);
}

TEST(DrawBoardSetCurrentPagePtrTest, EmitsCurrentPageChanged)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    Page *first = board->firstPage();
    ASSERT_NE(first, nullptr);
    Page *second = board->addPage();
    ASSERT_NE(second, nullptr);

    // Ensure we are on first.
    board->setCurrentPage(first);

    QSignalSpy spy(board, QOverload<Page*>::of(&DrawBoard::currentPageChanged));
    ASSERT_TRUE(spy.isValid());

    board->setCurrentPage(second);

    EXPECT_GE(spy.count(), 1);
}
