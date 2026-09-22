// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CDrawToolFactory::Create
 *
 * Target method (from issue V-5046):
 *   - CDrawToolFactory::Create  (level=high, complexity:16, lines:58)
 *
 * Source: src/drawshape/drawTools/cdrawtoolfactory.cpp
 *
 * Minimum case counts:
 *   Create: high + complexity≥8 (but <20) → 3 cases
 *
 * | Method | level | min | actual |
 * |--------|-------|-----|--------|
 * | Create | high  | 3   | 5      |
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

// =========================================================================
// CDrawToolFactory::Create tests
// =========================================================================

TEST(CDrawToolFactoryCreate, Selection_ReturnsNonNullTool)
{
    // Arrange
    EDrawToolMode mode = selection;

    // Act
    std::unique_ptr<IDrawTool> tool(CDrawToolFactory::Create(mode));

    // Assert
    ASSERT_NE(tool, nullptr);
    EXPECT_EQ(tool->getDrawToolMode(), selection);
}

TEST(CDrawToolFactoryCreate, Rectangle_ReturnsNonNullTool)
{
    // Arrange
    EDrawToolMode mode = rectangle;

    // Act
    std::unique_ptr<IDrawTool> tool(CDrawToolFactory::Create(mode));

    // Assert
    ASSERT_NE(tool, nullptr);
    EXPECT_EQ(tool->getDrawToolMode(), rectangle);
}

TEST(CDrawToolFactoryCreate, Text_ReturnsNonNullTool)
{
    // Arrange
    EDrawToolMode mode = text;

    // Act
    std::unique_ptr<IDrawTool> tool(CDrawToolFactory::Create(mode));

    // Assert
    ASSERT_NE(tool, nullptr);
    EXPECT_EQ(tool->getDrawToolMode(), text);
}

TEST(CDrawToolFactoryCreate, Pen_ReturnsNonNullTool)
{
    // Arrange
    EDrawToolMode mode = pen;

    // Act
    std::unique_ptr<IDrawTool> tool(CDrawToolFactory::Create(mode));

    // Assert
    ASSERT_NE(tool, nullptr);
    EXPECT_EQ(tool->getDrawToolMode(), pen);
}

TEST(CDrawToolFactoryCreate, UnknownMode_ReturnsNullptr)
{
    // Arrange - use a value outside the valid EDrawToolMode range
    EDrawToolMode mode = static_cast<EDrawToolMode>(999);

    // Act
    IDrawTool *tool = CDrawToolFactory::Create(mode);

    // Assert
    EXPECT_EQ(tool, nullptr);
}
