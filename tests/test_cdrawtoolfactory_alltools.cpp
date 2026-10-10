// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CDrawToolFactory::allTools() (cdrawtoolfactory.cpp).
 *
 * Target function (from issue V-5568):
 *   - CDrawToolFactory::allTools()  (static, returns CDrawToolsMap&)
 *
 * allTools() returns a reference to the internal static map s_tools.
 * After tools are registered via Create(), the map should be non-empty
 * and contain the expected tool entries.
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <memory>

#define protected public
#define private public
#include "drawshape/drawTools/cdrawtoolfactory.h"
#include "drawshape/globaldefine.h"
#include "drawshape/drawTools/idrawtool.h"
#undef protected
#undef private

TEST(CDrawToolFactoryAllTools, ReturnsNonEmptyMap_AfterCreate)
{
    // Register some tools by calling Create
    CDrawToolFactory::Create(selection);
    CDrawToolFactory::Create(rectangle);
    CDrawToolFactory::Create(text);

    auto &tools = CDrawToolFactory::allTools();
    EXPECT_FALSE(tools.isEmpty());
}

TEST(CDrawToolFactoryAllTools, ContainsRegisteredTools)
{
    // Ensure tools are registered
    CDrawToolFactory::Create(selection);
    CDrawToolFactory::Create(rectangle);

    auto &tools = CDrawToolFactory::allTools();

    // The map should contain entries for the created tools
    bool hasSelection = false;
    bool hasRectangle = false;
    for (auto it = tools.begin(); it != tools.end(); ++it) {
        if (it.value() != nullptr) {
            if (it.value()->getDrawToolMode() == selection)
                hasSelection = true;
            if (it.value()->getDrawToolMode() == rectangle)
                hasRectangle = true;
        }
    }
    EXPECT_TRUE(hasSelection);
    EXPECT_TRUE(hasRectangle);
}

TEST(CDrawToolFactoryAllTools, AllEntriesAreNonNull)
{
    CDrawToolFactory::Create(pen);
    CDrawToolFactory::Create(text);

    auto &tools = CDrawToolFactory::allTools();
    for (auto it = tools.begin(); it != tools.end(); ++it) {
        EXPECT_NE(it.value(), nullptr);
    }
}

TEST(CDrawToolFactoryAllTools, ReturnsSameReference)
{
    auto &tools1 = CDrawToolFactory::allTools();
    auto &tools2 = CDrawToolFactory::allTools();

    // Should return the same underlying map (same address)
    EXPECT_EQ(&tools1, &tools2);
}
