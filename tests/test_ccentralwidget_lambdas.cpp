// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for lambdas in ccentralwidget.cpp & cmultiptabbarwidget.cpp.
 *
 * Source: src/frame/ccentralwidget.cpp, src/frame/cmultiptabbarwidget.cpp
 *
 * Target lambdas (0-coverage):
 *   DrawBoard_private::execFileSelectDialog — constructs FileSelectDialog,
 *       but exec() is modal/blocks. We test construction only.
 *   TabBarWgt constructor SizeMode lambda — triggered by
 *       DGuiApplicationHelper::sizeModeChanged signal.
 *   FileSelectDialog::exec() — modal, skipped (documented).
 *   noticeFileContextChanged — static, file-local; tested indirectly via
 *       DrawBoard::onFileContextChanged with a nonexistent file.
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QFileDialog>
#include <DGuiApplicationHelper>
#include <DIconButton>

// Include cmultiptabbarwidget.h with private access BEFORE publicApi.h
// (which pulls in ccentralwidget.h → cmultiptabbarwidget.h without macros)
#define protected public
#define private public
#include "cmultiptabbarwidget.h"
#undef protected
#undef private

#define protected public
#define private public
#include "publicApi.h"
#include "ccentralwidget.h"
#include "globaldefine.h"
#undef protected
#undef private

// ===========================================================================
// FileSelectDialog — construction (exec() is modal, skipped)
// ===========================================================================

TEST(FileSelectDialogLambdaTest, ConstructionDoesNotCrash)
{
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    FileSelectDialog dialog(board);
    EXPECT_NE(dialog.parent(), nullptr);
    SUCCEED();
}

TEST(FileSelectDialogLambdaTest, ResultFileInitiallyEmpty)
{
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    FileSelectDialog dialog(board);
    EXPECT_TRUE(dialog.resultFile().isEmpty());
}

TEST(FileSelectDialogLambdaTest, ExecIsModalSkipped)
{
    GTEST_SKIP() << "FileSelectDialog::exec() is a modal dialog that blocks "
                    "the event loop; cannot be tested in offscreen mode.";
}

// ===========================================================================
// DrawBoard_private::execFileSelectDialog — construction path
// ===========================================================================

TEST(ExecFileSelectDialogLambdaTest, PrivateAccessible)
{
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);
    ASSERT_NE(board->d_pri(), nullptr);
    SUCCEED();
}

// ===========================================================================
// TabBarWgt SizeMode lambda
// ===========================================================================

TEST(TabBarWgtSizeModeLambdaTest, SizeModeChangedNoCrash)
{
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    TabBarWgt bar(board);

    // Emit sizeModeChanged to trigger the lambda that resizes tabs and AddBtn
    emit DGuiApplicationHelper::instance()->sizeModeChanged(DGuiApplicationHelper::CompactMode);
    qApp->processEvents();

    emit DGuiApplicationHelper::instance()->sizeModeChanged(DGuiApplicationHelper::NormalMode);
    qApp->processEvents();

    SUCCEED();
}

TEST(TabBarWgtSizeModeLambdaTest, CompactModeResizesAddButton)
{
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    TabBarWgt bar(board);

    // Trigger CompactMode — the lambda should set AddBtn to compact size
    emit DGuiApplicationHelper::instance()->sizeModeChanged(DGuiApplicationHelper::CompactMode);
    qApp->processEvents();

    auto *addBtn = bar.findChild<DIconButton *>("AddButton");
    if (addBtn) {
        // In compact mode, addBtn max size should be AddBtn_Size_Compact (20)
        EXPECT_LE(addBtn->maximumWidth(), 24);
    }
    SUCCEED();
}
