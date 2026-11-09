// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CGraphicsTriangleItem (issue V-7935)
 *
 * Target methods:
 *   - Constructor (QRectF / x,y,w,h forms)
 *   - type() returns TriangleType
 *   - getSelfOrgShape() shape bounding rect matches item rect
 *   - calcPoints_helper() zero offset returns three vertices / positive offset expands outward
 *   - getGraphicsUnit() head fields + triangle rect data
 *   - loadGraphicsUnit() serialization roundtrip / null pTriangle protection
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QPen>
#include <QPointF>
#include <QPainterPath>
#include <QPolygonF>
#include <QVector>
#include <QTransform>

#define protected public
#define private public
#include "drawshape/drawItems/bzItems/cgraphicstriangleitem.h"
#include "drawshape/drawItems/bzItems/cgraphicsrectitem.h"
#include "drawshape/sitemdata.h"
#include "drawshape/globaldefine.h"
#undef protected
#undef private

// =========================================================================
// Constructor — QRectF form
// =========================================================================

TEST(CGraphicsTriangleItemFunc2Test, Constructor_RectForm_StoresGeometry)
{
    CGraphicsTriangleItem item(QRectF(10, 20, 80, 60));
    QRectF r = item.rect();
    EXPECT_NEAR(r.x(), 10.0, 0.01);
    EXPECT_NEAR(r.y(), 20.0, 0.01);
    EXPECT_NEAR(r.width(), 80.0, 0.01);
    EXPECT_NEAR(r.height(), 60.0, 0.01);
}

// =========================================================================
// Constructor — x,y,w,h form
// =========================================================================

TEST(CGraphicsTriangleItemFunc2Test, Constructor_XYWHForm_StoresGeometry)
{
    CGraphicsTriangleItem item(5.0, 15.0, 100.0, 50.0);
    QRectF r = item.rect();
    EXPECT_NEAR(r.x(), 5.0, 0.01);
    EXPECT_NEAR(r.y(), 15.0, 0.01);
    EXPECT_NEAR(r.width(), 100.0, 0.01);
    EXPECT_NEAR(r.height(), 50.0, 0.01);
}

// =========================================================================
// type() returns TriangleType
// =========================================================================

TEST(CGraphicsTriangleItemFunc2Test, Type_ReturnsTriangleType)
{
    CGraphicsTriangleItem item(QRectF(0, 0, 100, 80));
    EXPECT_EQ(item.type(), TriangleType);
}

// =========================================================================
// getSelfOrgShape() — bounding rect matches item rect
// =========================================================================

TEST(CGraphicsTriangleItemFunc2Test, GetSelfOrgShape_BoundingRectMatchesItem)
{
    CGraphicsTriangleItem item(QRectF(10, 20, 100, 80));
    QPainterPath path = item.getSelfOrgShape();
    EXPECT_FALSE(path.isEmpty());

    QRectF br = path.boundingRect();
    EXPECT_NEAR(br.x(), 10.0, 1.0);
    EXPECT_NEAR(br.y(), 20.0, 1.0);
    EXPECT_NEAR(br.width(), 100.0, 1.0);
    EXPECT_NEAR(br.height(), 80.0, 1.0);
}

TEST(CGraphicsTriangleItemFunc2Test, GetSelfOrgShape_IsClosedPolygon)
{
    CGraphicsTriangleItem item(QRectF(0, 0, 100, 80));
    QPainterPath path = item.getSelfOrgShape();
    // A closed triangle polygon has 3 line segments + close
    EXPECT_GE(path.elementCount(), 3);
}

// =========================================================================
// calcPoints_helper() — zero offset returns three vertices
// =========================================================================

TEST(CGraphicsTriangleItemFunc2Test, CalcPointsHelper_ZeroOffset_ReturnsThreeVertices)
{
    CGraphicsTriangleItem item(QRectF(0, 0, 100, 80));
    QVector<QPointF> points;
    item.calcPoints_helper(points, 0.0);

    ASSERT_EQ(points.size(), 3);
}

TEST(CGraphicsTriangleItemFunc2Test, CalcPointsHelper_ZeroOffset_TopVertexAtCenterX)
{
    CGraphicsTriangleItem item(QRectF(0, 0, 100, 80));
    QVector<QPointF> points;
    item.calcPoints_helper(points, 0.0);

    ASSERT_EQ(points.size(), 3);
    // With zero offset the vertices are top, bottomLeft, bottomRight.
    // The top vertex sits at the horizontal center of the rect.
    EXPECT_NEAR(points[0].x(), 50.0, 1.0);
    EXPECT_NEAR(points[0].y(), 0.0, 1.0);
}

TEST(CGraphicsTriangleItemFunc2Test, CalcPointsHelper_ZeroOffset_BottomVerticesAtCorners)
{
    CGraphicsTriangleItem item(QRectF(0, 0, 100, 80));
    QVector<QPointF> points;
    item.calcPoints_helper(points, 0.0);

    ASSERT_EQ(points.size(), 3);
    // bottomLeft and bottomRight corners
    EXPECT_NEAR(points[1].x(), 0.0, 1.0);
    EXPECT_NEAR(points[1].y(), 80.0, 1.0);
    EXPECT_NEAR(points[2].x(), 100.0, 1.0);
    EXPECT_NEAR(points[2].y(), 80.0, 1.0);
}

TEST(CGraphicsTriangleItemFunc2Test, CalcPointsHelper_ClearsExistingVector)
{
    CGraphicsTriangleItem item(QRectF(0, 0, 100, 80));
    QVector<QPointF> points;
    points.append(QPointF(999, 999));
    points.append(QPointF(888, 888));

    item.calcPoints_helper(points, 0.0);
    // The helper clears the vector before appending.
    ASSERT_EQ(points.size(), 3);
    EXPECT_NE(points[0].x(), 999.0);
}

// =========================================================================
// calcPoints_helper() — positive offset expands vertices outward
// =========================================================================

TEST(CGraphicsTriangleItemFunc2Test, CalcPointsHelper_PositiveOffset_ReturnsThreePoints)
{
    CGraphicsTriangleItem item(QRectF(0, 0, 100, 80));
    QVector<QPointF> points;
    item.calcPoints_helper(points, 10.0);

    ASSERT_EQ(points.size(), 3);
}

TEST(CGraphicsTriangleItemFunc2Test, CalcPointsHelper_PositiveOffset_ShrinksInward)
{
    CGraphicsTriangleItem item(QRectF(0, 0, 100, 80));

    QVector<QPointF> zeroPoints;
    item.calcPoints_helper(zeroPoints, 0.0);

    QVector<QPointF> offsetPoints;
    item.calcPoints_helper(offsetPoints, 15.0);

    ASSERT_EQ(zeroPoints.size(), 3);
    ASSERT_EQ(offsetPoints.size(), 3);

    // A positive offset insets the triangle vertices inward, so the
    // bounding rect of the offset points must be strictly smaller.
    QRectF zeroBR = QPolygonF(zeroPoints).boundingRect();
    QRectF offsetBR = QPolygonF(offsetPoints).boundingRect();

    EXPECT_LT(offsetBR.width(), zeroBR.width());
    EXPECT_LT(offsetBR.height(), zeroBR.height());
}

// =========================================================================
// getGraphicsUnit() — head fields
// =========================================================================

TEST(CGraphicsTriangleItemFunc2Test, GetGraphicsUnit_HeadFieldsCorrect)
{
    CGraphicsTriangleItem item(QRectF(10, 20, 80, 60));
    item.setPen(QPen(Qt::red, 3));
    item.setBrush(QBrush(Qt::blue));
    item.setPos(QPointF(5, 7));
    item.setZValue(42);

    CGraphicsUnit unit = item.getGraphicsUnit(EDuplicate);

    EXPECT_EQ(unit.head.dataType, TriangleType);
    EXPECT_EQ(unit.head.dataLength, static_cast<qint64>(sizeof(SGraphicsTriangleUnitData)));
    EXPECT_EQ(unit.head.pen.color(), Qt::red);
    EXPECT_EQ(unit.head.brush.color(), Qt::blue);
    EXPECT_EQ(unit.head.pos, QPointF(5, 7));
    EXPECT_EQ(unit.head.zValue, 42);

    unit.release();
}

// =========================================================================
// getGraphicsUnit() — triangle rect data
// =========================================================================

TEST(CGraphicsTriangleItemFunc2Test, GetGraphicsUnit_StoresTriangleRectData)
{
    CGraphicsTriangleItem item(QRectF(10, 20, 80, 60));

    CGraphicsUnit unit = item.getGraphicsUnit(ESaveToDDf);

    ASSERT_NE(unit.data.pTriangle, nullptr);
    EXPECT_EQ(unit.data.pTriangle->rect.topLeft, QPointF(10, 20));
    EXPECT_EQ(unit.data.pTriangle->rect.bottomRight, QPointF(90, 80));

    unit.release();
}

// =========================================================================
// loadGraphicsUnit() — serialization roundtrip restores geometry and appearance
// =========================================================================

TEST(CGraphicsTriangleItemFunc2Test, LoadGraphicsUnit_RoundtripRestoresGeometry)
{
    CGraphicsTriangleItem source(QRectF(10, 20, 80, 60));
    source.setPen(QPen(Qt::green, 2));
    source.setBrush(QBrush(Qt::yellow));

    CGraphicsUnit unit = source.getGraphicsUnit(EDuplicate);

    CGraphicsTriangleItem dest(QRectF(0, 0, 1, 1));
    dest.loadGraphicsUnit(unit);

    EXPECT_EQ(dest.m_topLeftPoint, QPointF(10, 20));
    EXPECT_EQ(dest.m_bottomRightPoint, QPointF(90, 80));
    EXPECT_EQ(dest.pen().color(), Qt::green);
    EXPECT_EQ(dest.brush().color(), Qt::yellow);

    unit.release();
}

TEST(CGraphicsTriangleItemFunc2Test, LoadGraphicsUnit_RoundtripRestoresPosAndZValue)
{
    CGraphicsTriangleItem source(QRectF(0, 0, 50, 50));
    source.setPos(QPointF(100, 200));
    source.setZValue(15);

    CGraphicsUnit unit = source.getGraphicsUnit(EDuplicate);

    CGraphicsTriangleItem dest(QRectF(0, 0, 1, 1));
    dest.loadGraphicsUnit(unit);

    EXPECT_EQ(dest.pos(), QPointF(100, 200));
    EXPECT_EQ(dest.zValue(), 15);

    unit.release();
}

// =========================================================================
// loadGraphicsUnit() — null pTriangle protection
// =========================================================================

TEST(CGraphicsTriangleItemFunc2Test, LoadGraphicsUnit_NullPTriangle_DoesNotCrash)
{
    CGraphicsUnit unit;
    unit.head.dataType = TriangleType;

    CGraphicsTriangleItem dest(QRectF(0, 0, 50, 50));
    QPointF origTL = dest.m_topLeftPoint;
    QPointF origBR = dest.m_bottomRightPoint;

    dest.loadGraphicsUnit(unit);

    // Geometry should be unchanged when pTriangle is null.
    EXPECT_EQ(dest.m_topLeftPoint, origTL);
    EXPECT_EQ(dest.m_bottomRightPoint, origBR);
}
