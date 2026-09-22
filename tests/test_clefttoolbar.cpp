// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for DrawToolManager::setCurrentTool and DrawToolManager::initUI
 *
 * Target methods (from issue V-5046):
 *   - DrawToolManager::setCurrentTool  (level=high) — two overloads
 *   - DrawToolManager::initUI          (level=high)
 *
 * Source: src/frame/clefttoolbar.cpp
 * Note: The issue refers to "CLeftToolBar" but the actual class is
 *       DrawToolManager (defined in clefttoolbar.h/cpp).
 *
 * Minimum case counts:
 *   setCurrentTool: high → 3 cases (covers both overloads)
 *   initUI:         high → 3 cases
 *
 * | Method         | level | min | actual |
 * |----------------|-------|-----|--------|
 * | setCurrentTool | high  | 3   | 4      |
 * | initUI         | high  | 3   | 3      |
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

// Include Qt headers BEFORE #define private to avoid breaking Qt internals.
#include <QVBoxLayout>
#include <QButtonGroup>

#define protected public
#define private public
#include "clefttoolbar.h"
#include "drawshape/globaldefine.h"
#include "drawshape/drawTools/cdrawtoolfactory.h"
#include "drawshape/drawTools/idrawtool.h"
#include "publicApi.h"
#undef protected
#undef private

// The global 'blocked' flag in clefttoolbar.cpp
extern bool blocked;

// =========================================================================
// DrawToolManager::setCurrentTool tests
// =========================================================================

TEST(DrawToolManagerSetCurrentTool, NullTool_ReturnsFalse)
{
    // Arrange
    createNewViewByShortcutKey();
    MainWindow *mainWnd = getMainWindow();
    ASSERT_NE(mainWnd, nullptr);
    DrawToolManager *toolbar = mainWnd->drawBoard()->toolManager();
    ASSERT_NE(toolbar, nullptr);
    blocked = false;

    // Act
    bool result = toolbar->setCurrentTool(static_cast<IDrawTool *>(nullptr), false);

    // Assert
    EXPECT_FALSE(result);  // branch: tool == nullptr → return false
}

TEST(DrawToolManagerSetCurrentTool, Blocked_ReturnsFalse)
{
    // Arrange
    createNewViewByShortcutKey();
    MainWindow *mainWnd = getMainWindow();
    ASSERT_NE(mainWnd, nullptr);
    DrawToolManager *toolbar = mainWnd->drawBoard()->toolManager();
    ASSERT_NE(toolbar, nullptr);

    IDrawTool *tool = CDrawToolFactory::tool(rectangle);
    ASSERT_NE(tool, nullptr);

    // Set blocked flag
    blocked = true;

    // Act
    bool result = toolbar->setCurrentTool(tool, false);

    // Assert
    EXPECT_FALSE(result);  // branch: blocked → return false

    // Cleanup
    blocked = false;
}

TEST(DrawToolManagerSetCurrentTool, ValidToolById_ReturnsTrue)
{
    // Arrange
    createNewViewByShortcutKey();
    MainWindow *mainWnd = getMainWindow();
    ASSERT_NE(mainWnd, nullptr);
    DrawToolManager *toolbar = mainWnd->drawBoard()->toolManager();
    ASSERT_NE(toolbar, nullptr);
    blocked = false;

    // Act - use the int overload
    bool result = toolbar->setCurrentTool(static_cast<int>(rectangle), false);

    // Assert
    EXPECT_TRUE(result);
    EXPECT_NE(toolbar->_currentTool, nullptr);
    if (toolbar->_currentTool != nullptr) {
        EXPECT_EQ(toolbar->_currentTool->getDrawToolMode(), rectangle);
    }
}

TEST(DrawToolManagerSetCurrentTool, SameTool_ReturnsTrueNoChange)
{
    // Arrange
    createNewViewByShortcutKey();
    MainWindow *mainWnd = getMainWindow();
    ASSERT_NE(mainWnd, nullptr);
    DrawToolManager *toolbar = mainWnd->drawBoard()->toolManager();
    ASSERT_NE(toolbar, nullptr);
    blocked = false;

    // Set a tool first
    toolbar->setCurrentTool(static_cast<int>(selection), false);
    IDrawTool *firstTool = toolbar->_currentTool;

    // Act - set the same tool again
    bool result = toolbar->setCurrentTool(firstTool, false);

    // Assert
    EXPECT_TRUE(result);
    EXPECT_EQ(toolbar->_currentTool, firstTool);
}

// =========================================================================
// DrawToolManager::initUI tests
// =========================================================================

TEST(DrawToolManagerInitUI, SetsMinimumHeight)
{
    // Arrange
    createNewViewByShortcutKey();
    MainWindow *mainWnd = getMainWindow();
    ASSERT_NE(mainWnd, nullptr);
    DrawToolManager *toolbar = mainWnd->drawBoard()->toolManager();
    ASSERT_NE(toolbar, nullptr);

    // Act
    toolbar->initUI();

    // Assert
    EXPECT_GE(toolbar->minimumHeight(), 460);
}

TEST(DrawToolManagerInitUI, CreatesVBoxLayout)
{
    // Arrange
    createNewViewByShortcutKey();
    MainWindow *mainWnd = getMainWindow();
    ASSERT_NE(mainWnd, nullptr);
    DrawToolManager *toolbar = mainWnd->drawBoard()->toolManager();
    ASSERT_NE(toolbar, nullptr);

    // Act
    toolbar->initUI();

    // Assert - m_layout should be created
    ASSERT_NE(toolbar->m_layout, nullptr);
    EXPECT_EQ(toolbar->m_layout->spacing(), 12);  // BTN_SPACING = 12
}

TEST(DrawToolManagerInitUI, CreatesToolButtonGroup)
{
    // Arrange
    createNewViewByShortcutKey();
    MainWindow *mainWnd = getMainWindow();
    ASSERT_NE(mainWnd, nullptr);
    DrawToolManager *toolbar = mainWnd->drawBoard()->toolManager();
    ASSERT_NE(toolbar, nullptr);

    // Act
    toolbar->initUI();

    // Assert - toolButtonGroup should be created
    ASSERT_NE(toolbar->toolButtonGroup, nullptr);
}
