// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CGraphicsPolygonItem (issue V-5697)
 *
 * Target methods (6):
 *   setPointCount/nPointsCount, calcPoints_helper,
 *   getSelfOrgShape, loadGraphicsUnit, getGraphicsUnit
 *
 * Source: src/drawshape/drawItems/bzItems/cgraphicspolygonitem.cpp
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
#include "drawshape/drawItems/bzItems/cgraphicspolygonitem.h"
#include "drawshape/drawItems/bzItems/cgraphicsrectitem.h"
#include "drawshape/drawItems/bzItems/cgraphicsitem.h"
#include "drawshape/sitemdata.h"
#include "drawshape/globaldefine.h"
#undef protected
#undef private

// =========================================================================
// setPointCount / nPointsCount
// =========================================================================

TEST(CGraphicsPolygonItemFuncTest, SetPointCount_DefaultNotPreview_ReturnsCount)
{
    CGraphicsPolygonItem item(5, 0, 0, 100, 80);
    EXPECT_EQ(item.nPointsCount(), 5);
}

TEST(CGraphicsPolygonItemFuncTest, SetPointCount_UpdateValue_ReflectsImmediately)
{
    CGraphicsPolygonItem item(3, 0, 0, 100, 80);
    item.setPointCount(8, false);
    EXPECT_EQ(item.nPointsCount(), 8);
}

TEST(CGraphicsPolygonItemFuncTest, SetPointCount_PreviewMode_DoesNotChangeNonPreview)
{
    CGraphicsPolygonItem item(5, 0, 0, 100, 80);
    item.setPointCount(10, true);

    EXPECT_EQ(item.m_nPointsCount[true], 10);
    EXPECT_EQ(item.m_nPointsCount[false], 5);
    EXPECT_TRUE(item.m_isPreviewPointCount);
}

TEST(CGraphicsPolygonItemFuncTest, SetPointCount_PreviewThenNonPreview_RestoresActual)
{
    CGraphicsPolygonItem item(5, 0, 0, 100, 80);
    item.setPointCount(10, true);
    item.setPointCount(7, false);

    EXPECT_EQ(item.nPointsCount(), 7);
    EXPECT_FALSE(item.m_isPreviewPointCount);
}

TEST(CGraphicsPolygonItemFuncTest, NPointsCount_AfterPreviewToggle_ReturnsCorrectSlot)
{
    CGraphicsPolygonItem item(4, 0, 0, 100, 80);
    item.setPointCount(6, true);
    EXPECT_EQ(item.nPointsCount(), 6);

    item.setPointCount(4, false);
    EXPECT_EQ(item.nPointsCount(), 4);
}

// =========================================================================
// calcPoints_helper
// =========================================================================

TEST(CGraphicsPolygonItemFuncTest, CalcPointsHelper_FivePoints_ReturnsFiveVertices)
{
    CGraphicsPolygonItem item(5, 0, 0, 100, 80);
    QVector<QPointF> points;
    item.calcPoints_helper(points, 5, item.rect());

    EXPECT_EQ(points.size(), 5);
}

TEST(CGraphicsPolygonItemFuncTest, CalcPointsHelper_ZeroN_ReturnsEmpty)
{
    CGraphicsPolygonItem item(5, 0, 0, 100, 80);
    QVector<QPointF> points;
    item.calcPoints_helper(points, 0, item.rect());

    EXPECT_TRUE(points.isEmpty());
}

TEST(CGraphicsPolygonItemFuncTest, CalcPointsHelper_FirstPointAtTopCenter)
{
    CGraphicsPolygonItem item(5, 0, 0, 100, 80);
    QVector<QPointF> points;
    item.calcPoints_helper(points, 5, item.rect());

    // First point is generated at 90 degrees (top center)
    EXPECT_NEAR(points[0].x(), 50.0, 1.0);
}

TEST(CGraphicsPolygonItemFuncTest, CalcPointsHelper_WithOffset_ReturnsSameCount)
{
    CGraphicsPolygonItem item(6, 0, 0, 100, 80);
    QVector<QPointF> points;
    item.calcPoints_helper(points, 6, item.rect(), 5.0);

    EXPECT_EQ(points.size(), 6);
}

TEST(CGraphicsPolygonItemFuncTest, CalcPointsHelper_OffsetZero_NoModification)
{
    CGraphicsPolygonItem item(4, 0, 0, 100, 80);
    QVector<QPointF> pointsNoOffset;
    QVector<QPointF> pointsZeroOffset;

    item.calcPoints_helper(pointsNoOffset, 4, item.rect());
    item.calcPoints_helper(pointsZeroOffset, 4, item.rect(), 0.0);

    for (int i = 0; i < 4; ++i) {
        EXPECT_NEAR(pointsNoOffset[i].x(), pointsZeroOffset[i].x(), 0.01);
        EXPECT_NEAR(pointsNoOffset[i].y(), pointsZeroOffset[i].y(), 0.01);
    }
}

// =========================================================================
// getSelfOrgShape
// =========================================================================

TEST(CGraphicsPolygonItemFuncTest, GetSelfOrgShape_ReturnsNonEmptyPath)
{
    CGraphicsPolygonItem item(5, 0, 0, 100, 80);
    QPainterPath path = item.getSelfOrgShape();
    EXPECT_FALSE(path.isEmpty());
}

TEST(CGraphicsPolygonItemFuncTest, GetSelfOrgShape_BoundingRectWithinItemRect)
{
    CGraphicsPolygonItem item(5, 0, 0, 100, 80);
    QPainterPath path = item.getSelfOrgShape();
    QRectF br = path.boundingRect();
    EXPECT_NEAR(br.width(), 100.0, 10.0);
    EXPECT_NEAR(br.height(), 80.0, 10.0);
}

TEST(CGraphicsPolygonItemFuncTest, GetSelfOrgShape_FiveSidedPolygon)
{
    CGraphicsPolygonItem item(5, 0, 0, 100, 100);
    QPainterPath path = item.getSelfOrgShape();
    // A 5-sided polygon has 5 vertices in the path (plus close)
    EXPECT_GE(path.elementCount(), 5);
}

// =========================================================================
// getGraphicsUnit
// =========================================================================

TEST(CGraphicsPolygonItemFuncTest, GetGraphicsUnit_ReturnsCorrectDataType)
{
    CGraphicsPolygonItem item(5, 10, 20, 80, 60);
    CGraphicsUnit unit = item.getGraphicsUnit(EDuplicate);

    EXPECT_EQ(unit.head.dataType, PolygonType);
    unit.release();
}

TEST(CGraphicsPolygonItemFuncTest, GetGraphicsUnit_StoresPolygonData)
{
    CGraphicsPolygonItem item(5, 10, 20, 80, 60);
    CGraphicsUnit unit = item.getGraphicsUnit(EDuplicate);

    ASSERT_NE(unit.data.pPolygon, nullptr);
    EXPECT_EQ(unit.data.pPolygon->rect.topLeft, QPointF(10, 20));
    EXPECT_EQ(unit.data.pPolygon->rect.bottomRight, QPointF(90, 80));
    EXPECT_EQ(unit.data.pPolygon->pointNum, 5);
    unit.release();
}

TEST(CGraphicsPolygonItemFuncTest, GetGraphicsUnit_StoresCorrectPointNum)
{
    CGraphicsPolygonItem item(8, 0, 0, 100, 100);
    CGraphicsUnit unit = item.getGraphicsUnit(EDuplicate);

    ASSERT_NE(unit.data.pPolygon, nullptr);
    EXPECT_EQ(unit.data.pPolygon->pointNum, 8);
    unit.release();
}

// =========================================================================
// loadGraphicsUnit
// =========================================================================

TEST(CGraphicsPolygonItemFuncTest, LoadGraphicsUnit_RestoresPolygonData)
{
    CGraphicsPolygonItem source(7, 10, 20, 80, 60);
    CGraphicsUnit unit = source.getGraphicsUnit(EDuplicate);

    CGraphicsPolygonItem dest(3, 0, 0, 1, 1);
    dest.loadGraphicsUnit(unit);

    EXPECT_EQ(dest.m_nPointsCount[0], 7);
    EXPECT_FALSE(dest.m_isPreviewPointCount);
    EXPECT_EQ(dest.nPointsCount(), 7);

    unit.release();
}

TEST(CGraphicsPolygonItemFuncTest, LoadGraphicsUnit_RestoresRectFromSource)
{
    CGraphicsPolygonItem source(5, 10, 20, 80, 60);
    CGraphicsUnit unit = source.getGraphicsUnit(EDuplicate);

    CGraphicsPolygonItem dest(3, 0, 0, 1, 1);
    dest.loadGraphicsUnit(unit);

    QRectF r = dest.rect();
    EXPECT_NEAR(r.x(), 10.0, 1.0);
    EXPECT_NEAR(r.y(), 20.0, 1.0);
    EXPECT_NEAR(r.width(), 80.0, 1.0);
    EXPECT_NEAR(r.height(), 60.0, 1.0);

    unit.release();
}

TEST(CGraphicsPolygonItemFuncTest, LoadGraphicsUnit_NullData_OnlyLoadsHead)
{
    CGraphicsUnit unit;
    unit.head.dataType = PolygonType;

    CGraphicsPolygonItem dest(5, 0, 0, 50, 50);
    int origCount = dest.nPointsCount();

    dest.loadGraphicsUnit(unit);

    EXPECT_EQ(dest.nPointsCount(), origCount);
}
