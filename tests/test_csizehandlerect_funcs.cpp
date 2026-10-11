// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CSizeHandleRect methods (issue V-5575)
 *
 * Source: src/drawshape/csizehandlerect.cpp
 *
 * Target methods (not already covered by test_csizehandlerect.cpp):
 *   CSizeHandleRect::isFatherDragging, hitTest, getCursor,
 *   getTransBlockFlag, transCenter, setJustExitLogicAbility
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#define protected public
#define private public
#include "drawshape/drawItems/csizehandlerect.h"
#include "drawshape/drawItems/bzItems/cgraphicsitem.h"
#include "drawshape/drawItems/bzItems/cgraphicsrectitem.h"
#include "publicApi.h"
#undef protected
#undef private

#include <QPointF>

// =========================================================================
// CSizeHandleRect::getTransBlockFlag
// =========================================================================

TEST(CSizeHandleRectGetTransBlockFlagTest, LeftSetsBlockY)
{
    CSizeHandleRect handle(nullptr, CSizeHandleRect::Left);
    bool blockX = false, blockY = false;
    handle.getTransBlockFlag(CSizeHandleRect::Left, blockX, blockY);
    EXPECT_FALSE(blockX);
    EXPECT_TRUE(blockY);
}

TEST(CSizeHandleRectGetTransBlockFlagTest, RightSetsBlockY)
{
    CSizeHandleRect handle(nullptr, CSizeHandleRect::Right);
    bool blockX = false, blockY = false;
    handle.getTransBlockFlag(CSizeHandleRect::Right, blockX, blockY);
    EXPECT_FALSE(blockX);
    EXPECT_TRUE(blockY);
}

TEST(CSizeHandleRectGetTransBlockFlagTest, TopSetsBlockX)
{
    CSizeHandleRect handle(nullptr, CSizeHandleRect::Top);
    bool blockX = false, blockY = false;
    handle.getTransBlockFlag(CSizeHandleRect::Top, blockX, blockY);
    EXPECT_TRUE(blockX);
    EXPECT_FALSE(blockY);
}

TEST(CSizeHandleRectGetTransBlockFlagTest, BottomSetsBlockX)
{
    CSizeHandleRect handle(nullptr, CSizeHandleRect::Bottom);
    bool blockX = false, blockY = false;
    handle.getTransBlockFlag(CSizeHandleRect::Bottom, blockX, blockY);
    EXPECT_TRUE(blockX);
    EXPECT_FALSE(blockY);
}

TEST(CSizeHandleRectGetTransBlockFlagTest, LeftTopSetsNeither)
{
    CSizeHandleRect handle(nullptr, CSizeHandleRect::LeftTop);
    bool blockX = false, blockY = false;
    handle.getTransBlockFlag(CSizeHandleRect::LeftTop, blockX, blockY);
    EXPECT_FALSE(blockX);
    EXPECT_FALSE(blockY);
}

TEST(CSizeHandleRectGetTransBlockFlagTest, RotationSetsNeither)
{
    CSizeHandleRect handle(nullptr, CSizeHandleRect::Rotation);
    bool blockX = false, blockY = false;
    handle.getTransBlockFlag(CSizeHandleRect::Rotation, blockX, blockY);
    EXPECT_FALSE(blockX);
    EXPECT_FALSE(blockY);
}

// =========================================================================
// CSizeHandleRect::hitTest
// =========================================================================

TEST(CSizeHandleRectHitTestTest, NotVisibleReturnsFalse)
{
    CSizeHandleRect handle(nullptr, CSizeHandleRect::LeftTop);
    handle.m_bVisible = false;
    EXPECT_FALSE(handle.hitTest(QPointF(0, 0)));
}

// =========================================================================
// CSizeHandleRect::isFatherDragging
// =========================================================================

TEST(CSizeHandleRectIsFatherDraggingTest, NoParentReturnsFalse)
{
    CSizeHandleRect handle(nullptr, CSizeHandleRect::LeftTop);
    EXPECT_FALSE(handle.isFatherDragging());
}

// =========================================================================
// CSizeHandleRect::setJustExitLogicAbility
// =========================================================================

TEST(CSizeHandleRectSetJustExitLogicAbilityTest, SetsFlag)
{
    CSizeHandleRect handle(nullptr, CSizeHandleRect::LeftTop);
    handle.m_bVisible = false;
    handle.setJustExitLogicAbility(true);
    EXPECT_TRUE(handle.m_onlyLogicAblity);
}

TEST(CSizeHandleRectSetJustExitLogicAbilityTest, SetFalse)
{
    CSizeHandleRect handle(nullptr, CSizeHandleRect::LeftTop);
    handle.m_bVisible = false;
    handle.setJustExitLogicAbility(true);
    handle.setJustExitLogicAbility(false);
    EXPECT_FALSE(handle.m_onlyLogicAblity);
}

// =========================================================================
// CSizeHandleRect::getCursor
// =========================================================================

TEST(CSizeHandleRectGetCursorFuncTest, DoesNotCrashWithNullParent)
{
    CSizeHandleRect handle(nullptr, CSizeHandleRect::LeftTop);
    handle.m_bVisible = false;
    // getCursor loads SVG cursor resources; with a null parent it should
    // still return a valid cursor without crashing.
    QCursor cursor = handle.getCursor();
    Q_UNUSED(cursor)
    SUCCEED();
}

// =========================================================================
// CSizeHandleRect::transCenter
// =========================================================================

TEST(CSizeHandleRectTransCenterTest, ReturnsItemCenterForLeftTop)
{
    CGraphicsRectItem item(QRectF(0, 0, 100, 100));
    QPointF center = CSizeHandleRect::transCenter(
        CSizeHandleRect::LeftTop, &item);
    // LeftTop center is the bottom-right corner of the item.
    EXPECT_FLOAT_EQ(center.x(), 100.0f);
    EXPECT_FLOAT_EQ(center.y(), 100.0f);
}

TEST(CSizeHandleRectTransCenterTest, ReturnsItemCenterForRightBottom)
{
    CGraphicsRectItem item(QRectF(0, 0, 100, 100));
    QPointF center = CSizeHandleRect::transCenter(
        CSizeHandleRect::RightBottom, &item);
    // RightBottom center is the top-left corner of the item.
    EXPECT_FLOAT_EQ(center.x(), 0.0f);
    EXPECT_FLOAT_EQ(center.y(), 0.0f);
}

TEST(CSizeHandleRectTransCenterTest, ReturnsItemCenterForRotation)
{
    CGraphicsRectItem item(QRectF(0, 0, 100, 100));
    QPointF center = CSizeHandleRect::transCenter(
        CSizeHandleRect::Rotation, &item);
    // Rotation center is the center of the item.
    EXPECT_FLOAT_EQ(center.x(), 50.0f);
    EXPECT_FLOAT_EQ(center.y(), 50.0f);
}
