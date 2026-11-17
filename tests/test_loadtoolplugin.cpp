// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CDrawToolFactory::loadToolPlugin (issue V-8787)
 *
 * Source: src/drawshape/drawTools/cdrawtoolfactory.cpp
 *
 * Target method:
 *   static IDrawTool *CDrawToolFactory::loadToolPlugin(const QString &pluginPath)
 *
 * loadToolPlugin uses dlopen to load a .so plugin and looks up the
 * "creatTool" symbol. With an invalid path, dlopen fails and the
 * function returns nullptr. We test the invalid-path return behavior
 * since real plugin files are not available in the test environment.
 *
 * Known limitation: testing the success path requires a real plugin .so
 * with a "creatTool" exported symbol, which is not available in CI.
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#define protected public
#define private public
#include "drawshape/drawTools/cdrawtoolfactory.h"
#include "drawshape/drawTools/idrawtool.h"
#include "drawshape/globaldefine.h"
#undef protected
#undef private

// =========================================================================
// loadToolPlugin — invalid path returns nullptr
// =========================================================================

TEST(LoadToolPluginTest, NonExistentPath_ReturnsNull)
{
    IDrawTool *tool = CDrawToolFactory::loadToolPlugin("/nonexistent/path/to/plugin.so");
    EXPECT_EQ(tool, nullptr);
}

TEST(LoadToolPluginTest, EmptyPath_ReturnsNull)
{
    IDrawTool *tool = CDrawToolFactory::loadToolPlugin(QString());
    EXPECT_EQ(tool, nullptr);
}

TEST(LoadToolPluginTest, RegularFileNotSo_ReturnsNull)
{
    // A file that exists but is not a shared library should fail to load.
    // Use /etc/hostname or similar — a small text file that always exists.
    IDrawTool *tool = CDrawToolFactory::loadToolPlugin("/etc/hostname");
    EXPECT_EQ(tool, nullptr);
}

TEST(LoadToolPluginTest, DirectoryPath_ReturnsNull)
{
    // A directory is not a valid shared library.
    IDrawTool *tool = CDrawToolFactory::loadToolPlugin("/tmp");
    EXPECT_EQ(tool, nullptr);
}
