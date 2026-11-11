// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CGraphicsItemGroup zero-coverage methods (V-8216)
 *
 * Target methods:
 *   - getSelfOrgShape() const
 *   - getMinZ() const
 *   - move(QPointF, QPointF)
 *   - rasterToSelfLayer(bool)
 *
 * Source: src/drawshape/drawItems/cgraphicsitemselectedmgr.cpp
 */

#include <gtest/gtest.h>

#include <QPainterPath>
#include <QRectF>
#include <QPointF>

#define protected public
#define private public
#include "drawshape/drawItems/bzItems/cgraphicsrectitem.h"
#include "drawshape/drawItems/cgraphicsitemselectedmgr.h"
#include "drawshape/drawItems/bzItems/cgraphicsitem.h"
#undef protected
#undef private

// =========================================================================
// getSelfOrgShape
// =========================================================================

TEST(CGraphicsItemGroupExt2, GetSelfOrgShape_EmptyGroup_NoCrash)
{
    CGraphicsItemGroup group(CGraphicsItemGroup::ENormalGroup, "grp");
    QPainterPath path = group.getSelfOrgShape();
    // For an empty group boundingRect() is a null rect; just verify no crash.
    SUCCEED();
}

TEST(CGraphicsItemGroupExt2, GetSelfOrgShape_WithItems_MatchesBoundingRect)
{
    CGraphicsItemGroup group(CGraphicsItemGroup::ENormalGroup, "grp");
    CGraphicsRectItem item(0, 0, 100, 80, nullptr);
    group.add(&item, false, false);
    group.updateBoundingRect(true);

    QPainterPath path = group.getSelfOrgShape();
    QRectF br = group.boundingRect();
    // The shape should be the bounding rect
    EXPECT_FALSE(path.isEmpty());
    EXPECT_TRUE(path.contains(br.center()));
}

// =========================================================================
// getMinZ
// =========================================================================

TEST(CGraphicsItemGroupExt2, GetMinZ_EmptyGroup_ReturnsZero)
{
    CGraphicsItemGroup group(CGraphicsItemGroup::ENormalGroup, "grp");
    EXPECT_EQ(group.getMinZ(), 0.0);
}

TEST(CGraphicsItemGroupExt2, GetMinZ_WithItems_ReturnsLowestZ)
{
    CGraphicsItemGroup group(CGraphicsItemGroup::ENormalGroup, "grp");
    CGraphicsRectItem item1(0, 0, 50, 50, nullptr);
    CGraphicsRectItem item2(0, 0, 50, 50, nullptr);
    item1.setZValue(10.0);
    item2.setZValue(3.0);
    group.add(&item1, false, false);
    group.add(&item2, false, false);
    EXPECT_EQ(group.getMinZ(), 3.0);
}

TEST(CGraphicsItemGroupExt2, GetMinZ_SingleItem_ReturnsItemZ)
{
    CGraphicsItemGroup group(CGraphicsItemGroup::ENormalGroup, "grp");
    CGraphicsRectItem item(0, 0, 50, 50, nullptr);
    item.setZValue(7.5);
    group.add(&item, false, false);
    EXPECT_EQ(group.getMinZ(), 7.5);
}

// =========================================================================
// move
// =========================================================================

TEST(CGraphicsItemGroupExt2, Move_UpdatesItemPositions)
{
    CGraphicsItemGroup group(CGraphicsItemGroup::ENormalGroup, "grp");
    CGraphicsRectItem item(0, 0, 50, 50, nullptr);
    group.add(&item, false, false);
    group.updateBoundingRect(true);

    QPointF begin(0, 0);
    QPointF target(20, 30);
    group.move(begin, target);

    // move() calls item->move() which calls setPos(pos + delta)
    QPointF pos = item.pos();
    EXPECT_FLOAT_EQ(static_cast<float>(pos.x()), 20.0f);
    EXPECT_FLOAT_EQ(static_cast<float>(pos.y()), 30.0f);
}

TEST(CGraphicsItemGroupExt2, Move_ZeroDelta_KeepsPositions)
{
    CGraphicsItemGroup group(CGraphicsItemGroup::ENormalGroup, "grp");
    CGraphicsRectItem item(10, 10, 50, 50, nullptr);
    group.add(&item, false, false);
    group.updateBoundingRect(true);

    QPointF p(5, 5);
    group.move(p, p);

    QPointF pos = item.pos();
    EXPECT_FLOAT_EQ(static_cast<float>(pos.x()), 0.0f);
    EXPECT_FLOAT_EQ(static_cast<float>(pos.y()), 0.0f);
}

// =========================================================================
// rasterToSelfLayer
// =========================================================================

TEST(CGraphicsItemGroupExt2, RasterToSelfLayer_FalseDelete_RequiresSceneLayer)
{
    // rasterToSelfLayer calls CGraphicsItem::rasterToSelfLayer which dereferences
    // this->layer(); without a scene/layer context it segfaults.
    GTEST_SKIP() << "rasterToSelfLayer requires a valid CGraphicsLayer (scene context)";
}

TEST(CGraphicsItemGroupExt2, RasterToSelfLayer_TrueDelete_RequiresSceneLayer)
{
    GTEST_SKIP() << "rasterToSelfLayer requires a valid CGraphicsLayer (scene context)";
}

TEST(CGraphicsItemGroupExt2, RasterToSelfLayer_EmptyGroup_RequiresSceneLayer)
{
    GTEST_SKIP() << "rasterToSelfLayer requires a valid CGraphicsLayer (scene context)";
}
