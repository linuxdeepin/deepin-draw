// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for DrawToolManager methods (issue V-7429)
 *
 * Source: src/frame/clefttoolbar.cpp
 *
 * Target methods not covered by test_clefttoolbar.cpp:
 *   initDrawTools(), registerAllTools(), setScrollArea()
 *
 * Note: currentToolChanged is a SIGNAL in clefttoolbar.h, tested via
 *       QSignalSpy when setCurrentTool triggers it.
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QButtonGroup>
#include <QSignalSpy>

#define protected public
#define private public
#include "clefttoolbar.h"
#include "drawshape/globaldefine.h"
#include "drawshape/drawTools/cdrawtoolfactory.h"
#include "drawshape/drawTools/idrawtool.h"
#include "publicApi.h"
#undef protected
#undef private

extern bool blocked;

// =========================================================================
// DrawToolManager::initDrawTools
// =========================================================================

TEST(DrawToolManagerInitDrawTools, CreatesButtonGroup)
{
    createNewViewByShortcutKey();
    MainWindow *mainWnd = getMainWindow();
    ASSERT_NE(mainWnd, nullptr);
    DrawToolManager *toolbar = mainWnd->drawBoard()->toolManager();
    ASSERT_NE(toolbar, nullptr);

    // initDrawTools is already called during app startup; calling again
    // recreates the button group. Smoke test: no crash.
    toolbar->initDrawTools();
    EXPECT_NE(toolbar->toolButtonGroup, nullptr);
}

TEST(DrawToolManagerInitDrawTools, ButtonGroupIsExclusive)
{
    createNewViewByShortcutKey();
    MainWindow *mainWnd = getMainWindow();
    ASSERT_NE(mainWnd, nullptr);
    DrawToolManager *toolbar = mainWnd->drawBoard()->toolManager();
    ASSERT_NE(toolbar, nullptr);

    toolbar->initDrawTools();
    ASSERT_NE(toolbar->toolButtonGroup, nullptr);
    EXPECT_TRUE(toolbar->toolButtonGroup->exclusive());
}

TEST(DrawToolManagerInitDrawTools, ButtonGroupHasCorrectObjectName)
{
    createNewViewByShortcutKey();
    MainWindow *mainWnd = getMainWindow();
    ASSERT_NE(mainWnd, nullptr);
    DrawToolManager *toolbar = mainWnd->drawBoard()->toolManager();
    ASSERT_NE(toolbar, nullptr);

    toolbar->initDrawTools();
    ASSERT_NE(toolbar->toolButtonGroup, nullptr);
    EXPECT_EQ(toolbar->toolButtonGroup->objectName(), "ToolButtonGroup");
}

// =========================================================================
// DrawToolManager::registerAllTools
// =========================================================================

TEST(DrawToolManagerRegisterAllTools, DoesNotCrashWithToolsRegistered)
{
    createNewViewByShortcutKey();
    MainWindow *mainWnd = getMainWindow();
    ASSERT_NE(mainWnd, nullptr);
    DrawToolManager *toolbar = mainWnd->drawBoard()->toolManager();
    ASSERT_NE(toolbar, nullptr);

    // _tools is populated during app startup
    EXPECT_FALSE(toolbar->_tools.isEmpty());

    toolbar->registerAllTools();
    SUCCEED();
}

TEST(DrawToolManagerRegisterAllTools, DoesNotCrashWithEmptyTools)
{
    createNewViewByShortcutKey();
    MainWindow *mainWnd = getMainWindow();
    ASSERT_NE(mainWnd, nullptr);
    DrawToolManager *toolbar = mainWnd->drawBoard()->toolManager();
    ASSERT_NE(toolbar, nullptr);

    // Save and clear _tools
    auto savedTools = toolbar->_tools;
    toolbar->_tools.clear();

    toolbar->registerAllTools();
    SUCCEED();

    // Restore
    toolbar->_tools = savedTools;
}

// =========================================================================
// DrawToolManager::setScrollArea
// =========================================================================

TEST(DrawToolManagerSetScrollArea, SetsScrollArea)
{
    createNewViewByShortcutKey();
    MainWindow *mainWnd = getMainWindow();
    ASSERT_NE(mainWnd, nullptr);
    DrawToolManager *toolbar = mainWnd->drawBoard()->toolManager();
    ASSERT_NE(toolbar, nullptr);

    DScrollArea *area = new DScrollArea();
    toolbar->setScrollArea(area);
    EXPECT_EQ(toolbar->m_scrollArea, area);

    delete area;
}

TEST(DrawToolManagerSetScrollArea, SetNullptr)
{
    createNewViewByShortcutKey();
    MainWindow *mainWnd = getMainWindow();
    ASSERT_NE(mainWnd, nullptr);
    DrawToolManager *toolbar = mainWnd->drawBoard()->toolManager();
    ASSERT_NE(toolbar, nullptr);

    toolbar->setScrollArea(nullptr);
    EXPECT_EQ(toolbar->m_scrollArea, nullptr);
}

// =========================================================================
// currentToolChanged signal (emitted by setCurrentTool)
// =========================================================================

TEST(DrawToolManagerCurrentToolChangedSignal, EmittedOnToolChange)
{
    createNewViewByShortcutKey();
    MainWindow *mainWnd = getMainWindow();
    ASSERT_NE(mainWnd, nullptr);
    DrawToolManager *toolbar = mainWnd->drawBoard()->toolManager();
    ASSERT_NE(toolbar, nullptr);

    blocked = false;
    IDrawTool *rectTool = CDrawToolFactory::tool(rectangle);
    ASSERT_NE(rectTool, nullptr);

    QSignalSpy spy(toolbar, &DrawToolManager::currentToolChanged);
    toolbar->setCurrentTool(rectTool, false);
    // The signal should have fired at least once
    EXPECT_GE(spy.count(), 1);
}
