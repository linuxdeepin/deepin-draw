// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for MainWindow methods (issue V-7429)
 *
 * Source: src/frame/mainwindow.cpp
 *
 * Target methods:
 *   MainWindow::setWindowTitleInfo()
 *   MainWindow::slotOnEscButtonClick()
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#define protected public
#define private public
#include "publicApi.h"
#include "mainwindow.h"
#include "drawshape/globaldefine.h"
#include "drawshape/drawTools/cdrawtoolfactory.h"
#include "drawshape/drawTools/idrawtool.h"
#undef protected
#undef private

// =========================================================================
// MainWindow::setWindowTitleInfo
// =========================================================================

TEST(MainWindowSetWindowTitleInfo, DoesNotCrashWithCurrentPage)
{
    createNewViewByShortcutKey();
    MainWindow *mw = drawApp->topMainWindow();
    ASSERT_NE(mw, nullptr);

    mw->setWindowTitleInfo();
    SUCCEED();
}

TEST(MainWindowSetWindowTitleInfo, SetsTitleFromPageName)
{
    createNewViewByShortcutKey();
    MainWindow *mw = drawApp->topMainWindow();
    ASSERT_NE(mw, nullptr);

    auto *page = mw->drawBoard()->currentPage();
    ASSERT_NE(page, nullptr);

    // New page has no file path set, so title should be set from page name
    mw->setWindowTitleInfo();

    QString title = mw->windowTitle();
    // Title should contain the page name (non-empty)
    EXPECT_FALSE(title.isEmpty());
}

TEST(MainWindowSetWindowTitleInfo, SetsTitleFromFilePath)
{
    createNewViewByShortcutKey();
    MainWindow *mw = drawApp->topMainWindow();
    ASSERT_NE(mw, nullptr);

    auto *page = mw->drawBoard()->currentPage();
    ASSERT_NE(page, nullptr);

    // Set a file path on the current page
    page->setFile("/tmp/test_draw.png");

    mw->setWindowTitleInfo();
    EXPECT_EQ(mw->windowTitle(), "/tmp/test_draw.png");

    // Reset
    page->setFile("");
}

// =========================================================================
// MainWindow::slotOnEscButtonClick
// =========================================================================

TEST(MainWindowSlotOnEscButtonClick, DoesNotCrash)
{
    createNewViewByShortcutKey();
    MainWindow *mw = drawApp->topMainWindow();
    ASSERT_NE(mw, nullptr);

    mw->slotOnEscButtonClick();
    SUCCEED();
}

TEST(MainWindowSlotOnEscButtonClick, SwitchesToSelectionTool)
{
    createNewViewByShortcutKey();
    MainWindow *mw = drawApp->topMainWindow();
    ASSERT_NE(mw, nullptr);

    // First switch to rectangle tool
    drawApp->setCurrentTool(rectangle);

    // Then press ESC which should switch to selection
    mw->slotOnEscButtonClick();

    // Verify current tool is selection
    IDrawTool *currentTool = mw->drawBoard()->currentTool_p();
    ASSERT_NE(currentTool, nullptr);
    EXPECT_EQ(currentTool->getDrawToolMode(), selection);
}
