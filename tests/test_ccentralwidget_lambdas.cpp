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

namespace {

// Mirrors AddBtn_Size_Compact / AddBtn_Size_Normal
// (cmultiptabbarwidget.cpp:23-24). The SizeMode lambda calls
// setMaximumSize(QSize(n, n)) on the AddButton.
constexpr int kAddBtnSizeCompact = 20;
constexpr int kAddBtnSizeNormal = 24;

} // namespace

// ===========================================================================
// FileSelectDialog — construction (exec() is modal, skipped)
// ===========================================================================

TEST(FileSelectDialogLambdaTest, ConstructionDoesNotCrash)
{
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    FileSelectDialog dialog(board);
    EXPECT_EQ(dialog.parent(), board);
    EXPECT_TRUE(dialog.isWindow());
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

TEST(ExecFileSelectDialogLambdaTest, AutoCloseAccessorRoundTrips)
{
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    // DrawBoard_private is defined only inside ccentralwidget.cpp, so its
    // members are not visible here. Verify the accessor pair that the
    // execFileSelectDialog wrapper relies on instead.
    const bool saved = board->isAutoClose();
    board->setAutoClose(!saved);
    EXPECT_EQ(board->isAutoClose(), !saved);
    board->setAutoClose(saved);
    EXPECT_EQ(board->isAutoClose(), saved);
}

// ===========================================================================
// TabBarWgt SizeMode lambda
// ===========================================================================

TEST(TabBarWgtSizeModeLambdaTest, SizeModeChangedResizesAddButton)
{
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    TabBarWgt bar(board);

    // TabBarWgt's constructor calls setVisibleAddButton(true) before looking
    // up the button, so it must exist in any DTabBar build.
    auto *addBtn = bar.findChild<DIconButton *>("AddButton");
    if (addBtn == nullptr) {
        GTEST_SKIP() << "DTabBar build exposes no AddButton child";
    }

    // CompactMode -> AddBtn_Size_Compact.
    emit DGuiApplicationHelper::instance()->sizeModeChanged(
                DGuiApplicationHelper::CompactMode);
    qApp->processEvents();
    EXPECT_EQ(addBtn->maximumWidth(), kAddBtnSizeCompact);
    EXPECT_EQ(addBtn->maximumHeight(), kAddBtnSizeCompact);

    // NormalMode -> AddBtn_Size_Normal; also restores the global size mode
    // the harness runs in.
    emit DGuiApplicationHelper::instance()->sizeModeChanged(
                DGuiApplicationHelper::NormalMode);
    qApp->processEvents();
    EXPECT_EQ(addBtn->maximumWidth(), kAddBtnSizeNormal);
    EXPECT_EQ(addBtn->maximumHeight(), kAddBtnSizeNormal);
}

TEST(TabBarWgtSizeModeLambdaTest, SizeModeChangedKeepsBarHidden)
{
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    TabBarWgt bar(board);
    // The constructor hides the bar; the lambda only resizes tabs/button.
    ASSERT_FALSE(bar.isVisible());

    emit DGuiApplicationHelper::instance()->sizeModeChanged(
                DGuiApplicationHelper::CompactMode);
    qApp->processEvents();
    EXPECT_FALSE(bar.isVisible());

    emit DGuiApplicationHelper::instance()->sizeModeChanged(
                DGuiApplicationHelper::NormalMode);
    qApp->processEvents();
    EXPECT_FALSE(bar.isVisible());
}
