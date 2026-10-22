// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QSignalSpy>
#include <QTimer>
#include <QApplication>

#define protected public
#define private public
#include "publicApi.h"
#include "ccentralwidget.h"
#include "globaldefine.h"

/*
 * DrawBoard zoom / signal / loadDDf tests (V-6123)
 *
 * Covers: zoomTo, signal verification (pageAdded, pageRemoved,
 *         currentPageChanged, currentTitleNameChange), loadDDf
 */

// ---------------------------------------------------------------------------
// zoomTo
// ---------------------------------------------------------------------------

TEST(DrawBoardZoomToTest, ValidZoomDoesNotCrash)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);
    ASSERT_NE(board->currentPage(), nullptr);
    ASSERT_NE(board->currentPage()->view(), nullptr);

    // zoomTo delegates to currentPage()->view()->scale(total).
    board->zoomTo(2.0);
    SUCCEED();
}

TEST(DrawBoardZoomToTest, ZeroZoomDoesNotCrash)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    board->zoomTo(0.0);
    SUCCEED();
}

// ---------------------------------------------------------------------------
// Signal verification: pageAdded
// ---------------------------------------------------------------------------

TEST(DrawBoardSignalTest, AddPageEmitsPageAdded)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    QSignalSpy spy(board, &DrawBoard::pageAdded);
    ASSERT_TRUE(spy.isValid());

    board->addPage();

    EXPECT_GE(spy.count(), 1);
}

// ---------------------------------------------------------------------------
// Signal verification: currentPageChanged
// ---------------------------------------------------------------------------

TEST(DrawBoardSignalTest, SetCurrentPageEmitsCurrentPageChanged)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    Page *first = board->firstPage();
    ASSERT_NE(first, nullptr);
    Page *second = board->addPage();
    ASSERT_NE(second, nullptr);

    board->setCurrentPage(first);

    QSignalSpy spy(board, QOverload<Page*>::of(&DrawBoard::currentPageChanged));
    ASSERT_TRUE(spy.isValid());

    board->setCurrentPage(second);

    EXPECT_GE(spy.count(), 1);
}

// ---------------------------------------------------------------------------
// Signal verification: currentTitleNameChange (via setPageTitle)
// ---------------------------------------------------------------------------

TEST(DrawBoardSignalTest, SetPageTitleEmitsCurrentTitleNameChange)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);
    Page *page = board->currentPage();
    ASSERT_NE(page, nullptr);

    QSignalSpy spy(board, &DrawBoard::currentTitleNameChange);
    ASSERT_TRUE(spy.isValid());

    board->setPageTitle(page, "signal-test-title");

    EXPECT_GE(spy.count(), 1);
}

// ---------------------------------------------------------------------------
// Signal verification: pageRemoved (via closePage)
// ---------------------------------------------------------------------------

TEST(DrawBoardSignalTest, ClosePageEmitsPageRemoved)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    // Ensure at least 2 pages so one can be closed safely.
    Page *extra = board->addPage();
    ASSERT_NE(extra, nullptr);

    QSignalSpy spy(board, &DrawBoard::pageRemoved);
    ASSERT_TRUE(spy.isValid());

    board->closePage(extra);

    EXPECT_GE(spy.count(), 1);
}

// ---------------------------------------------------------------------------
// loadDDf
// ---------------------------------------------------------------------------

TEST(DrawBoardLoadDDfTest, NonExistentFileReturnsFalse)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    // A non-existent .ddf file should not crash and returns false
    // (loadDdf returns nullptr context).
    // loadDDf pops a modal MessageDlg on error; start a timer (before the
    // call) that finds and closes the dialog automatically so the test
    // does not block.
    auto *closer = new QTimer(board);
    closer->setInterval(50);
    QObject::connect(closer, &QTimer::timeout, board, [closer]() {
        if (QWidget *w = QApplication::activeModalWidget()) {
            w->close();
        }
    });
    closer->start();

    bool result = board->loadDDf("/tmp/nonexistent_ddf_file_12345.ddf");

    // Unconditional cleanup after loadDDf returns (the lambda must not
    // delete the timer itself while it may still fire).
    closer->stop();
    closer->deleteLater();
    EXPECT_FALSE(result);
}
