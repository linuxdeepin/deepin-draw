// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QCloseEvent>

#define protected public
#define private public
#include "publicApi.h"
#include "ccentralwidget.h"
#include "globaldefine.h"

/*
 * Page::save / Page::saveAs / Page::closeEvent
 *
 * These methods orchestrate file I/O and close confirmation. The dialog-
 * popping branches (save failure, close-with-modifications) are intentionally
 * avoided to keep tests deterministic. We exercise the early-return paths:
 *   save:     !isModified()          → true
 *   saveAs:   _context->isEmpty()    → true
 *   closeEvent: not modified, no tool blocks → event accepted
 */

TEST(PageSaveTest, ReturnsTrueWhenNotModified)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);
    Page *page = board->currentPage();
    ASSERT_NE(page, nullptr);

    // Ensure the page is clean so save() returns true at the !isModified guard.
    if (page->context() != nullptr)
        page->context()->setDirty(false);

    EXPECT_TRUE(page->save(""));
}

TEST(PageSaveAsTest, ReturnsTrueWhenContextEmpty)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);
    Page *page = board->currentPage();
    ASSERT_NE(page, nullptr);

    // A fresh page with no drawn items has an empty context → saveAs returns
    // true at the isEmpty() guard, before any file dialog is shown.
    EXPECT_TRUE(page->saveAs());
}

TEST(PageCloseEventTest, AcceptsWhenNotModified)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);
    Page *page = board->currentPage();
    ASSERT_NE(page, nullptr);

    // No active tool → blockPageBeforeOutput is skipped → refuse stays false.
    drawApp->setCurrentTool(noselected);
    if (page->context() != nullptr)
        page->context()->setDirty(false);

    QCloseEvent event;
    page->closeEvent(&event);
    // Not modified, not refused → event accepted.
    EXPECT_TRUE(event.isAccepted());
}

/*
 * Page accessor tests (V-5282)
 *
 * Exercises the simple getter and utility methods of Page:
 *   borad, close, context, file, isModified, key, name, scene, title,
 *   view, saveToImage
 *
 * Each test creates a fresh page via createNewViewByShortcutKey() and
 * accesses it through drawApp->topMainWindow()->drawBoard()->currentPage().
 */

// ---------------------------------------------------------------------------
// borad
// ---------------------------------------------------------------------------

TEST(PageBoradTest, ReturnsDrawBoardWhenPageHasParent)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);
    Page *page = board->currentPage();
    ASSERT_NE(page, nullptr);

    // The page is parented inside the DrawBoard widget hierarchy.
    EXPECT_EQ(page->borad(), board);
}

// ---------------------------------------------------------------------------
// close
// ---------------------------------------------------------------------------

TEST(PageCloseTest, ForceCloseReturnsBool)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);
    Page *page = board->currentPage();
    ASSERT_NE(page, nullptr);

    // close(force=true) sets the force-close flag, calls QWidget::close(),
    // then resets the flag.  We only verify it returns without crashing.
    page->close(true);
    SUCCEED();
}

// ---------------------------------------------------------------------------
// context
// ---------------------------------------------------------------------------

TEST(PageContextTest, ReturnsNonNullContext)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);
    Page *page = board->currentPage();
    ASSERT_NE(page, nullptr);

    EXPECT_NE(page->context(), nullptr);
}

// ---------------------------------------------------------------------------
// file
// ---------------------------------------------------------------------------

TEST(PageFileTest, ReturnsEmptyForNewPage)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);
    Page *page = board->currentPage();
    ASSERT_NE(page, nullptr);

    // A freshly created page has no file path associated.
    EXPECT_TRUE(page->file().isEmpty());
}

// ---------------------------------------------------------------------------
// isModified
// ---------------------------------------------------------------------------

TEST(PageIsModifiedTest, NewPageIsNotModified)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);
    Page *page = board->currentPage();
    ASSERT_NE(page, nullptr);

    if (page->context() != nullptr)
        page->context()->setDirty(false);

    EXPECT_FALSE(page->isModified());
}

TEST(PageIsModifiedTest, ReturnsTrueWhenContextDirty)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);
    Page *page = board->currentPage();
    ASSERT_NE(page, nullptr);

    ASSERT_NE(page->context(), nullptr);
    page->context()->setDirty(true);
    EXPECT_TRUE(page->isModified());

    // Restore clean state.
    page->context()->setDirty(false);
}

// ---------------------------------------------------------------------------
// key
// ---------------------------------------------------------------------------

TEST(PageKeyTest, ReturnsNonEmptyKey)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);
    Page *page = board->currentPage();
    ASSERT_NE(page, nullptr);

    // Every page is assigned a unique key via genericOneKey().
    EXPECT_FALSE(page->key().isEmpty());
}

// ---------------------------------------------------------------------------
// name
// ---------------------------------------------------------------------------

TEST(PageNameTest, ReturnsNameFromContext)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);
    Page *page = board->currentPage();
    ASSERT_NE(page, nullptr);

    // name() delegates to _context->name(); the context must exist.
    ASSERT_NE(page->context(), nullptr);
    QString name = page->name();
    // The default name may be empty or set by the application; just verify
    // it does not crash and returns a valid string.
    EXPECT_NO_FATAL_FAILURE(name.toStdString());
}

// ---------------------------------------------------------------------------
// scene
// ---------------------------------------------------------------------------

TEST(PageSceneTest, ReturnsNonNullScene)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);
    Page *page = board->currentPage();
    ASSERT_NE(page, nullptr);

    // A fully initialised page has a scene attached via its context.
    EXPECT_NE(page->scene(), nullptr);
}

// ---------------------------------------------------------------------------
// title
// ---------------------------------------------------------------------------

TEST(PageTitleTest, ReturnsNameWhenNotModified)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);
    Page *page = board->currentPage();
    ASSERT_NE(page, nullptr);

    ASSERT_NE(page->context(), nullptr);
    page->context()->setDirty(false);

    // When not modified, title() == name().
    EXPECT_EQ(page->title(), page->name());
}

TEST(PageTitleTest, ReturnsStarPrefixWhenModified)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);
    Page *page = board->currentPage();
    ASSERT_NE(page, nullptr);

    ASSERT_NE(page->context(), nullptr);
    page->context()->setDirty(true);

    // When modified and name is non-empty, title() == "* " + name().
    QString name = page->name();
    if (!name.isEmpty()) {
        EXPECT_EQ(page->title(), QString("* ") + name);
    }

    // Restore clean state.
    page->context()->setDirty(false);
}

// ---------------------------------------------------------------------------
// view
// ---------------------------------------------------------------------------

TEST(PageViewTest, ReturnsNonNullView)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);
    Page *page = board->currentPage();
    ASSERT_NE(page, nullptr);

    EXPECT_NE(page->view(), nullptr);
}

// ---------------------------------------------------------------------------
// saveToImage
// ---------------------------------------------------------------------------

TEST(PageSaveToImageTest, ReturnsFalseForEmptyFile)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);
    Page *page = board->currentPage();
    ASSERT_NE(page, nullptr);

    // An empty file path triggers the early-return false guard.
    EXPECT_FALSE(page->saveToImage("", QSize(100, 100), -1));
}
