// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CGraphicsPolygonalStarItem (issue V-5697)
 *
 * Target methods (9):
 *   setAnchorNum/anchorNum, setInnerRadius/innerRadius,
 *   updatePolygonalStar, calcPolygon_helper,
 *   getSelfOrgShape, getPenStrokerShape, getShape
 *
 * Source:
 *   src/drawshape/drawItems/bzItems/cgraphicspolygonalstaritem.cpp
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QtCore/QtGlobal>
#include <QPen>
#include <QPointF>
#include <QPainterPath>
#include <QPolygonF>
#include <QVector>

#define protected public
#define private public
#include "drawshape/drawItems/bzItems/cgraphicspolygonalstaritem.h"
#include "drawshape/drawItems/bzItems/cgraphicsrectitem.h"
#include "drawshape/drawItems/bzItems/cgraphicsitem.h"
#include "drawshape/sitemdata.h"
#include "drawshape/globaldefine.h"
#undef protected
#undef private

// =========================================================================
// setAnchorNum / anchorNum
// =========================================================================

TEST(CGraphicsPolygonalStarItemFuncTest, SetAnchorNum_NotPreview_ReturnsValue)
{
    CGraphicsPolygonalStarItem item(5, 50, 0, 0, 100, 80);
    EXPECT_EQ(item.anchorNum(), 5);
}

TEST(CGraphicsPolygonalStarItemFuncTest, SetAnchorNum_UpdateValue_ReflectsImmediately)
{
    CGraphicsPolygonalStarItem item(5, 50, 0, 0, 100, 80);
    item.setAnchorNum(8, false);
    EXPECT_EQ(item.anchorNum(), 8);
}

TEST(CGraphicsPolygonalStarItemFuncTest, SetAnchorNum_Preview_DoesNotChangeNonPreview)
{
    CGraphicsPolygonalStarItem item(5, 50, 0, 0, 100, 80);
    item.setAnchorNum(10, true);

    EXPECT_EQ(item.m_anchorNum[true], 10);
    EXPECT_EQ(item.m_anchorNum[false], 5);
}

TEST(CGraphicsPolygonalStarItemFuncTest, AnchorNum_AfterPreviewToggle_ReturnsCorrectSlot)
{
    CGraphicsPolygonalStarItem item(5, 50, 0, 0, 100, 80);
    item.setAnchorNum(7, true);
    EXPECT_EQ(item.anchorNum(), 7);

    item.setAnchorNum(6, false);
    EXPECT_EQ(item.anchorNum(), 6);
}

// =========================================================================
// setInnerRadius / innerRadius
// =========================================================================

TEST(CGraphicsPolygonalStarItemFuncTest, SetInnerRadius_NotPreview_ReturnsValue)
{
    CGraphicsPolygonalStarItem item(5, 50, 0, 0, 100, 80);
    EXPECT_EQ(item.innerRadius(), 50);
}

TEST(CGraphicsPolygonalStarItemFuncTest, SetInnerRadius_UpdateValue_ReflectsImmediately)
{
    CGraphicsPolygonalStarItem item(5, 50, 0, 0, 100, 80);
    item.setInnerRadius(70, false);
    EXPECT_EQ(item.innerRadius(), 70);
}

TEST(CGraphicsPolygonalStarItemFuncTest, SetInnerRadius_Preview_DoesNotChangeNonPreview)
{
    CGraphicsPolygonalStarItem item(5, 50, 0, 0, 100, 80);
    item.setInnerRadius(80, true);

    EXPECT_EQ(item.m_innerRadius[true], 80);
    EXPECT_EQ(item.m_innerRadius[false], 50);
}

TEST(CGraphicsPolygonalStarItemFuncTest, InnerRadius_AfterPreviewToggle_ReturnsCorrectSlot)
{
    CGraphicsPolygonalStarItem item(5, 50, 0, 0, 100, 80);
    item.setInnerRadius(60, true);
    EXPECT_EQ(item.innerRadius(), 60);

    item.setInnerRadius(40, false);
    EXPECT_EQ(item.innerRadius(), 40);
}

// =========================================================================
// updatePolygonalStar
// =========================================================================

TEST(CGraphicsPolygonalStarItemFuncTest, UpdatePolygonalStar_SetsNonPreviewValues)
{
    CGraphicsPolygonalStarItem item(5, 50, 0, 0, 100, 80);
    item.updatePolygonalStar(10, 30);

    EXPECT_EQ(item.anchorNum(), 10);
    EXPECT_EQ(item.innerRadius(), 30);
}

TEST(CGraphicsPolygonalStarItemFuncTest, UpdatePolygonalStar_ResetsPreviewFlags)
{
    CGraphicsPolygonalStarItem item(5, 50, 0, 0, 100, 80);
    item.setAnchorNum(8, true);
    item.setInnerRadius(70, true);
    item.updatePolygonalStar(6, 40);

    EXPECT_FALSE(item.m_preview[0]);
    EXPECT_FALSE(item.m_preview[1]);
    EXPECT_EQ(item.anchorNum(), 6);
    EXPECT_EQ(item.innerRadius(), 40);
}

TEST(CGraphicsPolygonalStarItemFuncTest, UpdatePolygonalStar_OverwritesPreviewSlots)
{
    CGraphicsPolygonalStarItem item(5, 50, 0, 0, 100, 80);
    item.setAnchorNum(12, true);
    item.setInnerRadius(90, true);
    item.updatePolygonalStar(7, 35);

    // After updatePolygonalStar, index [0] values are authoritative
    EXPECT_EQ(item.m_anchorNum[0], 7);
    EXPECT_EQ(item.m_innerRadius[0], 35);
}

// =========================================================================
// calcPolygon_helper
// =========================================================================

TEST(CGraphicsPolygonalStarItemFuncTest, CalcPolygonHelper_FiveAnchors_ReturnsTenPoints)
{
    CGraphicsPolygonalStarItem item(5, 50, 0, 0, 100, 100);
    QPolygonF polygon;
    item.calcPolygon_helper(polygon, 5, 0.0);

    // 5 anchors → 5 outer + 5 inner = 10 points
    EXPECT_EQ(polygon.size(), 10);
}

TEST(CGraphicsPolygonalStarItemFuncTest, CalcPolygonHelper_ZeroN_ReturnsEmpty)
{
    CGraphicsPolygonalStarItem item(5, 50, 0, 0, 100, 100);
    QPolygonF polygon;
    item.calcPolygon_helper(polygon, 0, 0.0);

    EXPECT_TRUE(polygon.isEmpty());
}

TEST(CGraphicsPolygonalStarItemFuncTest, CalcPolygonHelper_WithOffset_ReturnsSameCount)
{
    CGraphicsPolygonalStarItem item(5, 50, 0, 0, 100, 100);
    QPolygonF polygon;
    item.calcPolygon_helper(polygon, 5, 5.0);

    EXPECT_EQ(polygon.size(), 10);
}

TEST(CGraphicsPolygonalStarItemFuncTest, CalcPolygonHelper_ZeroOffset_PointsAtExpectedPosition)
{
    CGraphicsPolygonalStarItem item(5, 50, 0, 0, 100, 100);
    QPolygonF polygon;
    item.calcPolygon_helper(polygon, 5, 0.0);

    // First outer point is at 90 degrees → top center
    EXPECT_NEAR(polygon[0].x(), 50.0, 1.0);
}

TEST(CGraphicsPolygonalStarItemFuncTest, CalcPolygonHelper_InnerPointsCloserToCenter)
{
    CGraphicsPolygonalStarItem item(5, 50, 0, 0, 100, 100);
    QPolygonF polygon;
    item.calcPolygon_helper(polygon, 5, 0.0);

    QPointF center(50, 50);
    qreal outerDist = QLineF(polygon[0], center).length();
    qreal innerDist = QLineF(polygon[1], center).length();

    // Inner radius is 50% of outer, so inner points are closer
    EXPECT_LT(innerDist, outerDist);
}

// =========================================================================
// getSelfOrgShape
// =========================================================================

TEST(CGraphicsPolygonalStarItemFuncTest, GetSelfOrgShape_ReturnsNonEmptyPath)
{
    CGraphicsPolygonalStarItem item(5, 50, 0, 0, 100, 80);
    QPainterPath path = item.getSelfOrgShape();
    EXPECT_FALSE(path.isEmpty());
}

TEST(CGraphicsPolygonalStarItemFuncTest, GetSelfOrgShape_UsesZeroOffset)
{
    CGraphicsPolygonalStarItem item(5, 50, 0, 0, 100, 80);
    QPainterPath path = item.getSelfOrgShape();
    // getSelfOrgShape calls calcPolygon_helper with offset=0
    // so the shape should match the raw polygon
    EXPECT_GE(path.elementCount(), 10);
}

TEST(CGraphicsPolygonalStarItemFuncTest, GetSelfOrgShape_BoundingRectWithinItem)
{
    CGraphicsPolygonalStarItem item(5, 50, 0, 0, 100, 80);
    QPainterPath path = item.getSelfOrgShape();
    QRectF br = path.boundingRect();
    EXPECT_NEAR(br.width(), 100.0, 10.0);
    EXPECT_NEAR(br.height(), 80.0, 10.0);
}

// =========================================================================
// getPenStrokerShape
// =========================================================================

TEST(CGraphicsPolygonalStarItemFuncTest, GetPenStrokerShape_ReturnsNonEmptyPath)
{
    CGraphicsPolygonalStarItem item(5, 50, 0, 0, 100, 80);
    QPainterPath path = item.getPenStrokerShape();
    EXPECT_FALSE(path.isEmpty());
}

TEST(CGraphicsPolygonalStarItemFuncTest, GetPenStrokerShape_ContainsTwoPolygons)
{
    CGraphicsPolygonalStarItem item(5, 50, 0, 0, 100, 80);
    QPainterPath path = item.getPenStrokerShape();
    // getPenStrokerShape adds two polygons (outer and inner offset)
    // so the path should have at least 20 elements (10 + 10)
    EXPECT_GE(path.elementCount(), 20);
}

// =========================================================================
// getShape
// =========================================================================

TEST(CGraphicsPolygonalStarItemFuncTest, GetShape_ReturnsNonEmptyPath)
{
    CGraphicsPolygonalStarItem item(5, 50, 0, 0, 100, 80);
    QPainterPath path = item.getShape();
    EXPECT_FALSE(path.isEmpty());
}

TEST(CGraphicsPolygonalStarItemFuncTest, GetShape_ContainsPolygonPoints)
{
    CGraphicsPolygonalStarItem item(5, 50, 0, 0, 100, 80);
    QPainterPath path = item.getShape();
    // getShape uses anchorNum() points with offset
    EXPECT_GE(path.elementCount(), 10);
}

TEST(CGraphicsPolygonalStarItemFuncTest, GetShape_BoundingRectCoversItem)
{
    CGraphicsPolygonalStarItem item(5, 50, 0, 0, 100, 80);
    QPainterPath path = item.getShape();
    QRectF br = path.boundingRect();
    // With pen width offset, the shape may extend slightly beyond
    EXPECT_GE(br.width(), 90.0);
    EXPECT_GE(br.height(), 70.0);
}
