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
