// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QEvent>
#include <QFocusEvent>
#include <QHideEvent>
#include <QStringList>

#define protected public
#define private public
#include "publicApi.h"
#include "ccentralwidget.h"

/*
 * DrawBoard::eventFilter
 *
 * Dispatches on event type: Shortcut (tool refresh), Hide (window refocus),
 * FocusOut (prox widget management via doFocusChanged), FocusIn, mouse events.
 * We exercise several type branches with synthetic events and assert no crash
 * and sensible return values.
 */

TEST(DrawBoardEventFilterTest, UnknownEventReturnsBaseBehavior)
{
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    QEvent event(QEvent::None);
    // No branch matches → falls through to DWidget::eventFilter (false).
    EXPECT_FALSE(board->eventFilter(board, &event));
}

TEST(DrawBoardEventFilterTest, ShortcutEventNoWorkingToolNoCrash)
{
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    // No tool is working → the Shortcut branch queues a refresh and falls
    // through to the base eventFilter. Must not crash.
    QEvent event(QEvent::Shortcut);
    board->eventFilter(board, &event);
    SUCCEED();
}

TEST(DrawBoardEventFilterTest, HideEventWithWindowTypeNoCrash)
{
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    // A Hide event from a window-type object triggers the refocus path
    // (only when no popup widget is active). Must not crash.
    QHideEvent event;
    board->eventFilter(board->window(), &event);
    SUCCEED();
}

/*
 * DrawBoard::loadFiles
 *
 * Complex 132-line method that loads DDF / image files via FileHander, with
 * threaded invokeMethod calls and progress dialog. We test the simplest path:
 * an empty file list is a no-op that must not crash.
 */

TEST(DrawBoardLoadFilesTest, EmptyFileListIsNoOp)
{
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    // Empty list → foreach body never executes → no files loaded.
    // quitIfAllFialed=false → app is not quit. Must not crash.
    board->loadFiles(QStringList(), false, 0, false);
    SUCCEED();
}
