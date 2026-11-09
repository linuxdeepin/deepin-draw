// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CGraphicsEllipseItem (issue V-7935)
 *
 * Target methods:
 *   - Constructor (x,y,w,h form)
 *   - type() returns EllipseType
 *   - getSelfOrgShape() contains center, excludes bounding-rect corners
 *   - getGraphicsUnit() head fields + reson
 *   - loadGraphicsUnit() serialization roundtrip / null pCircle protection
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QPen>
#include <QPointF>
#include <QPainterPath>
#include <QTransform>

#define protected public
#define private public
#include "drawshape/drawItems/bzItems/cgraphicsellipseitem.h"
#include "drawshape/drawItems/bzItems/cgraphicsrectitem.h"
#include "drawshape/sitemdata.h"
#include "drawshape/globaldefine.h"
#undef protected
#undef private

// =========================================================================
// Constructor — x,y,w,h form
// =========================================================================

TEST(CGraphicsEllipseItemFunc2Test, Constructor_XYWHForm_StoresGeometry)
{
    CGraphicsEllipseItem item(10.0, 20.0, 80.0, 60.0);
    QRectF r = item.rect();
    EXPECT_NEAR(r.x(), 10.0, 0.01);
    EXPECT_NEAR(r.y(), 20.0, 0.01);
    EXPECT_NEAR(r.width(), 80.0, 0.01);
    EXPECT_NEAR(r.height(), 60.0, 0.01);
}

// =========================================================================
// type() returns EllipseType
// =========================================================================

TEST(CGraphicsEllipseItemFunc2Test, Type_ReturnsEllipseType)
{
    CGraphicsEllipseItem item(0, 0, 100, 80);
    EXPECT_EQ(item.type(), EllipseType);
}

// =========================================================================
// getSelfOrgShape() — contains center, excludes bounding-rect corners
// =========================================================================

TEST(CGraphicsEllipseItemFunc2Test, GetSelfOrgShape_ContainsCenter)
{
    CGraphicsEllipseItem item(0, 0, 100, 80);
    QPainterPath path = item.getSelfOrgShape();
    EXPECT_FALSE(path.isEmpty());

    // The ellipse inscribed in rect(0,0,100,80) has its center at (50,40).
    EXPECT_TRUE(path.contains(QPointF(50, 40)));
}

TEST(CGraphicsEllipseItemFunc2Test, GetSelfOrgShape_ExcludesCorners)
{
    CGraphicsEllipseItem item(0, 0, 100, 80);
    QPainterPath path = item.getSelfOrgShape();

    // The four corners of the bounding rect lie outside the inscribed ellipse.
    EXPECT_FALSE(path.contains(QPointF(0, 0)));
    EXPECT_FALSE(path.contains(QPointF(100, 0)));
    EXPECT_FALSE(path.contains(QPointF(0, 80)));
    EXPECT_FALSE(path.contains(QPointF(100, 80)));
}

TEST(CGraphicsEllipseItemFunc2Test, GetSelfOrgShape_BoundingRectMatchesItem)
{
    CGraphicsEllipseItem item(10, 20, 100, 80);
    QPainterPath path = item.getSelfOrgShape();
    QRectF br = path.boundingRect();
    EXPECT_NEAR(br.x(), 10.0, 1.0);
    EXPECT_NEAR(br.y(), 20.0, 1.0);
    EXPECT_NEAR(br.width(), 100.0, 1.0);
    EXPECT_NEAR(br.height(), 80.0, 1.0);
}

TEST(CGraphicsEllipseItemFunc2Test, GetSelfOrgShape_IncludesEdgeMidpoints)
{
    CGraphicsEllipseItem item(0, 0, 100, 80);
    QPainterPath path = item.getSelfOrgShape();

    // Points just inside the edge midpoints should be within the ellipse.
    // Top midpoint is (50, 0); a point just below it is inside.
    EXPECT_TRUE(path.contains(QPointF(50, 2)));
    // Left midpoint is (0, 40); a point just right of it is inside.
    EXPECT_TRUE(path.contains(QPointF(2, 40)));
}

// =========================================================================
// getGraphicsUnit() — head fields
// =========================================================================

TEST(CGraphicsEllipseItemFunc2Test, GetGraphicsUnit_HeadFieldsCorrect)
{
    CGraphicsEllipseItem item(10, 20, 80, 60);
    item.setPen(QPen(Qt::red, 3));
    item.setBrush(QBrush(Qt::blue));
    item.setPos(QPointF(5, 7));
    item.setZValue(42);

    CGraphicsUnit unit = item.getGraphicsUnit(EDuplicate);

    EXPECT_EQ(unit.head.dataType, EllipseType);
    EXPECT_EQ(unit.head.dataLength, static_cast<qint64>(sizeof(SGraphicsCircleUnitData)));
    EXPECT_EQ(unit.head.pen.color(), Qt::red);
    EXPECT_EQ(unit.head.brush.color(), Qt::blue);
    EXPECT_EQ(unit.head.pos, QPointF(5, 7));
    EXPECT_EQ(unit.head.zValue, 42);

    unit.release();
}

// =========================================================================
// getGraphicsUnit() — reson is recorded (unlike CGraphicsTriangleItem which Q_UNUSEDs it)
// =========================================================================

TEST(CGraphicsEllipseItemFunc2Test, GetGraphicsUnit_RecordsReason)
{
    CGraphicsEllipseItem item(0, 0, 50, 50);

    CGraphicsUnit unitDup = item.getGraphicsUnit(EDuplicate);
    EXPECT_EQ(unitDup.reson, EDuplicate);
    unitDup.release();

    CGraphicsUnit unitSave = item.getGraphicsUnit(ESaveToDDf);
    EXPECT_EQ(unitSave.reson, ESaveToDDf);
    unitSave.release();

    CGraphicsUnit unitUndo = item.getGraphicsUnit(EUndoRedo);
    EXPECT_EQ(unitUndo.reson, EUndoRedo);
    unitUndo.release();
}

// =========================================================================
// getGraphicsUnit() — ellipse rect data
// =========================================================================

TEST(CGraphicsEllipseItemFunc2Test, GetGraphicsUnit_StoresEllipseRectData)
{
    CGraphicsEllipseItem item(10, 20, 80, 60);

    CGraphicsUnit unit = item.getGraphicsUnit(ESaveToDDf);

    ASSERT_NE(unit.data.pCircle, nullptr);
    EXPECT_EQ(unit.data.pCircle->rect.topLeft, QPointF(10, 20));
    EXPECT_EQ(unit.data.pCircle->rect.bottomRight, QPointF(90, 80));

    unit.release();
}

// =========================================================================
// loadGraphicsUnit() — serialization roundtrip restores geometry and appearance
// =========================================================================

TEST(CGraphicsEllipseItemFunc2Test, LoadGraphicsUnit_RoundtripRestoresGeometry)
{
    CGraphicsEllipseItem source(10, 20, 80, 60);
    source.setPen(QPen(Qt::green, 2));
    source.setBrush(QBrush(Qt::yellow));

    CGraphicsUnit unit = source.getGraphicsUnit(EDuplicate);

    CGraphicsEllipseItem dest(0, 0, 1, 1);
    dest.loadGraphicsUnit(unit);

    EXPECT_EQ(dest.m_topLeftPoint, QPointF(10, 20));
    EXPECT_EQ(dest.m_bottomRightPoint, QPointF(90, 80));
    EXPECT_EQ(dest.pen().color(), Qt::green);
    EXPECT_EQ(dest.brush().color(), Qt::yellow);

    unit.release();
}

TEST(CGraphicsEllipseItemFunc2Test, LoadGraphicsUnit_RoundtripRestoresPosAndZValue)
{
    CGraphicsEllipseItem source(0, 0, 50, 50);
    source.setPos(QPointF(100, 200));
    source.setZValue(15);

    CGraphicsUnit unit = source.getGraphicsUnit(EDuplicate);

    CGraphicsEllipseItem dest(0, 0, 1, 1);
    dest.loadGraphicsUnit(unit);

    EXPECT_EQ(dest.pos(), QPointF(100, 200));
    EXPECT_EQ(dest.zValue(), 15);

    unit.release();
}

// =========================================================================
// loadGraphicsUnit() — null pCircle protection
// =========================================================================

TEST(CGraphicsEllipseItemFunc2Test, LoadGraphicsUnit_NullPCircle_DoesNotCrash)
{
    CGraphicsUnit unit;
    unit.head.dataType = EllipseType;

    CGraphicsEllipseItem dest(0, 0, 50, 50);
    QPointF origTL = dest.m_topLeftPoint;
    QPointF origBR = dest.m_bottomRightPoint;

    dest.loadGraphicsUnit(unit);

    // Geometry should be unchanged when pCircle is null.
    EXPECT_EQ(dest.m_topLeftPoint, origTL);
    EXPECT_EQ(dest.m_bottomRightPoint, origBR);
}
