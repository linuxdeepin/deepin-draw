// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CSizeHandleRect (csizehandlerect.cpp).
 *
 * Target functions (from issue V-5568):
 *   - CSizeHandleRect::setState(ESelectionHandleState)
 *   - CSizeHandleRect::getTransNegtiveFlag(EDirection, bool &, bool &)
 *
 * getTransNegtiveFlag is a static method and can be tested without
 * a QGraphicsScene instance.
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#define protected public
#define private public
#include "drawshape/drawItems/csizehandlerect.h"
#undef protected
#undef private

// =========================================================================
// CSizeHandleRect::getTransNegtiveFlag (static)
// =========================================================================

TEST(CSizeHandleRectGetTransNegtiveFlagTest, LeftSetsNegXOnly)
{
    bool negX = false, negY = false;
    CSizeHandleRect::getTransNegtiveFlag(CSizeHandleRect::Left, negX, negY);
    EXPECT_TRUE(negX);
    EXPECT_FALSE(negY);
}

TEST(CSizeHandleRectGetTransNegtiveFlagTest, LeftBottomSetsNegXOnly)
{
    bool negX = false, negY = false;
    CSizeHandleRect::getTransNegtiveFlag(CSizeHandleRect::LeftBottom, negX, negY);
    EXPECT_TRUE(negX);
    EXPECT_FALSE(negY);
}

TEST(CSizeHandleRectGetTransNegtiveFlagTest, TopSetsNegYOnly)
{
    bool negX = false, negY = false;
    CSizeHandleRect::getTransNegtiveFlag(CSizeHandleRect::Top, negX, negY);
    EXPECT_FALSE(negX);
    EXPECT_TRUE(negY);
}

TEST(CSizeHandleRectGetTransNegtiveFlagTest, RightTopSetsNegYOnly)
{
    bool negX = false, negY = false;
    CSizeHandleRect::getTransNegtiveFlag(CSizeHandleRect::RightTop, negX, negY);
    EXPECT_FALSE(negX);
    EXPECT_TRUE(negY);
}

TEST(CSizeHandleRectGetTransNegtiveFlagTest, LeftTopSetsBothNeg)
{
    bool negX = false, negY = false;
    CSizeHandleRect::getTransNegtiveFlag(CSizeHandleRect::LeftTop, negX, negY);
    EXPECT_TRUE(negX);
    EXPECT_TRUE(negY);
}

TEST(CSizeHandleRectGetTransNegtiveFlagTest, RightSetsNeither)
{
    bool negX = false, negY = false;
    CSizeHandleRect::getTransNegtiveFlag(CSizeHandleRect::Right, negX, negY);
    EXPECT_FALSE(negX);
    EXPECT_FALSE(negY);
}

TEST(CSizeHandleRectGetTransNegtiveFlagTest, BottomSetsNeither)
{
    bool negX = false, negY = false;
    CSizeHandleRect::getTransNegtiveFlag(CSizeHandleRect::Bottom, negX, negY);
    EXPECT_FALSE(negX);
    EXPECT_FALSE(negY);
}

TEST(CSizeHandleRectGetTransNegtiveFlagTest, RightBottomSetsNeither)
{
    bool negX = false, negY = false;
    CSizeHandleRect::getTransNegtiveFlag(CSizeHandleRect::RightBottom, negX, negY);
    EXPECT_FALSE(negX);
    EXPECT_FALSE(negY);
}

TEST(CSizeHandleRectGetTransNegtiveFlagTest, RotationSetsNeither)
{
    bool negX = false, negY = false;
    CSizeHandleRect::getTransNegtiveFlag(CSizeHandleRect::Rotation, negX, negY);
    EXPECT_FALSE(negX);
    EXPECT_FALSE(negY);
}

TEST(CSizeHandleRectGetTransNegtiveFlagTest, InRectSetsNeither)
{
    bool negX = false, negY = false;
    CSizeHandleRect::getTransNegtiveFlag(CSizeHandleRect::InRect, negX, negY);
    EXPECT_FALSE(negX);
    EXPECT_FALSE(negY);
}

// =========================================================================
// CSizeHandleRect::setState (needs a constructed item)
// =========================================================================

TEST(CSizeHandleRectSetStateTest, SetStateOffDoesNotCrash)
{
    // CSizeHandleRect requires a QGraphicsItem parent; construct with
    // a null parent to avoid scene dependencies.
    CSizeHandleRect handle(nullptr, CSizeHandleRect::LeftTop);
    handle.setState(SelectionHandleOff);
    EXPECT_EQ(handle.m_state, SelectionHandleOff);
}

TEST(CSizeHandleRectSetStateTest, SetStateActiveDoesNotCrash)
{
    CSizeHandleRect handle(nullptr, CSizeHandleRect::Right);
    handle.m_bVisible = false;
    handle.setState(SelectionHandleActive);
    EXPECT_EQ(handle.m_state, SelectionHandleActive);
}

TEST(CSizeHandleRectSetStateTest, SetStateInactiveDoesNotCrash)
{
    CSizeHandleRect handle(nullptr, CSizeHandleRect::Bottom);
    handle.m_bVisible = false;
    handle.setState(SelectionHandleInactive);
    EXPECT_EQ(handle.m_state, SelectionHandleInactive);
}

TEST(CSizeHandleRectSetStateTest, SetSameStateNoChange)
{
    CSizeHandleRect handle(nullptr, CSizeHandleRect::Left);
    handle.m_state = SelectionHandleOff;
    handle.setState(SelectionHandleOff);
    EXPECT_EQ(handle.m_state, SelectionHandleOff);
}
