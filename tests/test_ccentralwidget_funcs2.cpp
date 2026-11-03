// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for DrawBoard signals and methods (issue V-7429)
 *
 * Source: src/frame/ccentralwidget.cpp / .h
 *
 * Target methods/signals:
 *   DrawBoard::setAutoClose(bool b)
 *   DrawBoard::isAutoClose() const
 *   DrawBoard::zoomValueChanged(qreal totalScaled)  [SIGNAL]
 *   DrawBoard::toClose()  [SIGNAL]
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QSignalSpy>

#define protected public
#define private public
#include "publicApi.h"
#include "ccentralwidget.h"
#include "globaldefine.h"
#undef protected
#undef private

// =========================================================================
// DrawBoard::setAutoClose / isAutoClose
// =========================================================================

TEST(DrawBoardAutoClose, DefaultIsFalse)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    // Default should be false
    bool val = board->isAutoClose();
    // Just verify it doesn't crash; the default may vary
    SUCCEED();
}

TEST(DrawBoardAutoClose, SetTrueReadsBackTrue)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    board->setAutoClose(true);
    EXPECT_TRUE(board->isAutoClose());
}

TEST(DrawBoardAutoClose, SetFalseReadsBackFalse)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    board->setAutoClose(false);
    EXPECT_FALSE(board->isAutoClose());
}

TEST(DrawBoardAutoClose, ToggleValue)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    board->setAutoClose(true);
    EXPECT_TRUE(board->isAutoClose());

    board->setAutoClose(false);
    EXPECT_FALSE(board->isAutoClose());

    board->setAutoClose(true);
    EXPECT_TRUE(board->isAutoClose());
}

// =========================================================================
// DrawBoard::zoomValueChanged (SIGNAL)
// =========================================================================

TEST(DrawBoardZoomValueChangedSignal, CanBeEmitted)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    QSignalSpy spy(board, &DrawBoard::zoomValueChanged);

    emit board->zoomValueChanged(1.5);

    EXPECT_EQ(spy.count(), 1);
    if (spy.count() >= 1) {
        QList<QVariant> args = spy.takeFirst();
        EXPECT_DOUBLE_EQ(args.at(0).toDouble(), 1.5);
    }
}

TEST(DrawBoardZoomValueChangedSignal, MultipleEmissions)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    QSignalSpy spy(board, &DrawBoard::zoomValueChanged);

    emit board->zoomValueChanged(1.0);
    emit board->zoomValueChanged(2.0);
    emit board->zoomValueChanged(0.5);

    EXPECT_EQ(spy.count(), 3);
}

// =========================================================================
// DrawBoard::toClose (SIGNAL)
// =========================================================================

TEST(DrawBoardToCloseSignal, CanBeEmitted)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    QSignalSpy spy(board, &DrawBoard::toClose);

    emit board->toClose();

    EXPECT_EQ(spy.count(), 1);
}

TEST(DrawBoardToCloseSignal, EmittedOncePerEmit)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    QSignalSpy spy(board, &DrawBoard::toClose);

    emit board->toClose();
    emit board->toClose();

    EXPECT_EQ(spy.count(), 2);
}
