// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CGraphicsRectItem, CGraphicsEllipseItem, and
 * CGraphicsTriangleItem (issue V-5697)
 *
 * Target methods:
 *   CGraphicsRectItem (7):
 *     setXYRedius/getXRedius, loadGraphicsRectUnit,
 *     getSelfOrgShape, incLength, loadGraphicsUnit, getGraphicsUnit
 *   CGraphicsEllipseItem (3):
 *     getSelfOrgShape, loadGraphicsUnit, getGraphicsUnit
 *   CGraphicsTriangleItem (4):
 *     calcPoints_helper, getSelfOrgShape,
 *     loadGraphicsUnit, getGraphicsUnit
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
#include "drawshape/drawItems/bzItems/cgraphicsrectitem.h"
#include "drawshape/drawItems/bzItems/cgraphicsellipseitem.h"
#include "drawshape/drawItems/bzItems/cgraphicstriangleitem.h"
#include "drawshape/drawItems/bzItems/cgraphicsitem.h"
#include "drawshape/sitemdata.h"
#include "drawshape/globaldefine.h"
#undef protected
#undef private

// =========================================================================
// CGraphicsRectItem — setXYRedius / getXRedius
// =========================================================================

TEST(CGraphicsRectItemFuncTest, SetXYRedius_NotPreview_StoresValues)
{
    CGraphicsRectItem item(QRectF(0, 0, 100, 80));
    item.setXYRedius(10, 20, false);

    EXPECT_EQ(item.getXRedius(), 10);
}

TEST(CGraphicsRectItemFuncTest, SetXYRedius_NotPreview_StoresBothAxes)
{
    CGraphicsRectItem item(QRectF(0, 0, 100, 80));
    item.setXYRedius(5, 15, false);

    EXPECT_EQ(item.getXRedius(), 5);
    EXPECT_EQ(item.m_yRedius, 15);
}

TEST(CGraphicsRectItemFuncTest, SetXYRedius_Preview_DoesNotChangeActual)
{
    CGraphicsRectItem item(QRectF(0, 0, 100, 80));
    item.setXYRedius(10, 20, false);
    item.setXYRedius(30, 40, true);

    // Preview mode should not modify m_xRedius / m_yRedius
    EXPECT_EQ(item.getXRedius(), 10);
    EXPECT_EQ(item.m_yRedius, 20);
    EXPECT_EQ(item.m_rediusForPreview, 30);
    EXPECT_TRUE(item.m_isPreviewRedius);
}

TEST(CGraphicsRectItemFuncTest, GetXRedius_DefaultIsZero)
{
    CGraphicsRectItem item(QRectF(0, 0, 100, 80));
    EXPECT_EQ(item.getXRedius(), 0);
}

// =========================================================================
// CGraphicsRectItem — loadGraphicsRectUnit
// =========================================================================

TEST(CGraphicsRectItemFuncTest, LoadGraphicsRectUnit_StoresCornerPoints)
{
    CGraphicsRectItem item(QRectF(0, 0, 10, 10));

    SGraphicsRectUnitData data;
    data.topLeft = QPointF(5, 5);
    data.bottomRight = QPointF(50, 50);
    data.xRedius = 8;
    data.yRedius = 12;

    item.loadGraphicsRectUnit(data);

    EXPECT_EQ(item.m_topLeftPoint, QPointF(5, 5));
    EXPECT_EQ(item.m_bottomRightPoint, QPointF(50, 50));
    EXPECT_EQ(item.m_xRedius, 8);
    EXPECT_EQ(item.m_yRedius, 12);
}

TEST(CGraphicsRectItemFuncTest, LoadGraphicsRectUnit_SetsPreviewFalse)
{
    CGraphicsRectItem item(QRectF(0, 0, 10, 10));

    SGraphicsRectUnitData data;
    data.topLeft = QPointF(0, 0);
    data.bottomRight = QPointF(100, 100);
    data.xRedius = 0;
    data.yRedius = 0;

    item.loadGraphicsRectUnit(data);

    EXPECT_FALSE(item.m_isPreviewRedius);
}

// =========================================================================
// CGraphicsRectItem — getSelfOrgShape
// =========================================================================

TEST(CGraphicsRectItemFuncTest, GetSelfOrgShape_ReturnsRoundedRectPath)
{
    CGraphicsRectItem item(QRectF(0, 0, 100, 80));
    item.setXYRedius(10, 10, false);

    QPainterPath path = item.getSelfOrgShape();
    EXPECT_FALSE(path.isEmpty());
}

TEST(CGraphicsRectItemFuncTest, GetSelfOrgShape_ZeroRadius_IsRectLike)
{
    CGraphicsRectItem item(QRectF(0, 0, 100, 80));

    QPainterPath path = item.getSelfOrgShape();
    EXPECT_FALSE(path.isEmpty());
    QRectF br = path.boundingRect();
    EXPECT_NEAR(br.width(), 100.0, 1.0);
    EXPECT_NEAR(br.height(), 80.0, 1.0);
}

// =========================================================================
// CGraphicsRectItem — incLength
// =========================================================================

TEST(CGraphicsRectItemFuncTest, IncLength_ReturnsPositiveValue)
{
    CGraphicsRectItem item(QRectF(0, 0, 100, 80));
    item.setPen(QPen(Qt::black, 2));

    qreal len = item.incLength();
    EXPECT_GT(len, 0.0);
}

// =========================================================================
// CGraphicsRectItem — getGraphicsUnit
// =========================================================================

TEST(CGraphicsRectItemFuncTest, GetGraphicsUnit_ReturnsCorrectDataType)
{
    CGraphicsRectItem item(QRectF(10, 20, 80, 60));
    item.setXYRedius(5, 10, false);

    CGraphicsUnit unit = item.getGraphicsUnit(EDuplicate);

    EXPECT_EQ(unit.head.dataType, RectType);
    unit.release();
}

TEST(CGraphicsRectItemFuncTest, GetGraphicsUnit_StoresRectData)
{
    CGraphicsRectItem item(QRectF(10, 20, 80, 60));
    item.setXYRedius(5, 10, false);

    CGraphicsUnit unit = item.getGraphicsUnit(ESaveToDDf);

    ASSERT_NE(unit.data.pRect, nullptr);
    EXPECT_EQ(unit.data.pRect->topLeft, QPointF(10, 20));
    EXPECT_EQ(unit.data.pRect->bottomRight, QPointF(90, 80));
    EXPECT_EQ(unit.data.pRect->xRedius, 5);
    EXPECT_EQ(unit.data.pRect->yRedius, 10);
    unit.release();
}

// =========================================================================
// CGraphicsRectItem — loadGraphicsUnit
// =========================================================================

TEST(CGraphicsRectItemFuncTest, LoadGraphicsUnit_RestoresRectData)
{
    CGraphicsRectItem source(QRectF(10, 20, 80, 60));
    source.setXYRedius(5, 10, false);

    CGraphicsUnit unit = source.getGraphicsUnit(EDuplicate);

    CGraphicsRectItem dest(QRectF(0, 0, 1, 1));
    dest.loadGraphicsUnit(unit);

    EXPECT_EQ(dest.m_topLeftPoint, QPointF(10, 20));
    EXPECT_EQ(dest.m_bottomRightPoint, QPointF(90, 80));
    EXPECT_EQ(dest.m_xRedius, 5);
    EXPECT_EQ(dest.m_yRedius, 10);

    unit.release();
}

TEST(CGraphicsRectItemFuncTest, LoadGraphicsUnit_NullData_OnlyLoadsHead)
{
    CGraphicsUnit unit;
    unit.head.dataType = RectType;

    CGraphicsRectItem dest(QRectF(0, 0, 50, 50));
    QPointF origTL = dest.m_topLeftPoint;
    QPointF origBR = dest.m_bottomRightPoint;

    dest.loadGraphicsUnit(unit);

    // With null pRect, rect data should remain unchanged
    EXPECT_EQ(dest.m_topLeftPoint, origTL);
    EXPECT_EQ(dest.m_bottomRightPoint, origBR);
}

// =========================================================================
// CGraphicsEllipseItem — getSelfOrgShape
// =========================================================================

TEST(CGraphicsEllipseItemFuncTest, GetSelfOrgShape_ReturnsEllipsePath)
{
    CGraphicsEllipseItem item(0, 0, 100, 80);

    QPainterPath path = item.getSelfOrgShape();
    EXPECT_FALSE(path.isEmpty());
}

TEST(CGraphicsEllipseItemFuncTest, GetSelfOrgShape_BoundingRectMatchesItemRect)
{
    CGraphicsEllipseItem item(10, 20, 100, 80);

    QPainterPath path = item.getSelfOrgShape();
    QRectF br = path.boundingRect();
    EXPECT_NEAR(br.x(), 10.0, 1.0);
    EXPECT_NEAR(br.y(), 20.0, 1.0);
    EXPECT_NEAR(br.width(), 100.0, 1.0);
    EXPECT_NEAR(br.height(), 80.0, 1.0);
}

// =========================================================================
// CGraphicsEllipseItem — getGraphicsUnit
// =========================================================================

TEST(CGraphicsEllipseItemFuncTest, GetGraphicsUnit_ReturnsCorrectDataType)
{
    CGraphicsEllipseItem item(10, 20, 80, 60);

    CGraphicsUnit unit = item.getGraphicsUnit(EDuplicate);

    EXPECT_EQ(unit.head.dataType, EllipseType);
    unit.release();
}

TEST(CGraphicsEllipseItemFuncTest, GetGraphicsUnit_StoresEllipseData)
{
    CGraphicsEllipseItem item(10, 20, 80, 60);

    CGraphicsUnit unit = item.getGraphicsUnit(ESaveToDDf);

    ASSERT_NE(unit.data.pCircle, nullptr);
    EXPECT_EQ(unit.data.pCircle->rect.topLeft, QPointF(10, 20));
    EXPECT_EQ(unit.data.pCircle->rect.bottomRight, QPointF(90, 80));
    unit.release();
}

// =========================================================================
// CGraphicsEllipseItem — loadGraphicsUnit
// =========================================================================

TEST(CGraphicsEllipseItemFuncTest, LoadGraphicsUnit_RestoresEllipseData)
{
    CGraphicsEllipseItem source(10, 20, 80, 60);
    CGraphicsUnit unit = source.getGraphicsUnit(EDuplicate);

    CGraphicsEllipseItem dest(0, 0, 1, 1);
    dest.loadGraphicsUnit(unit);

    EXPECT_EQ(dest.m_topLeftPoint, QPointF(10, 20));
    EXPECT_EQ(dest.m_bottomRightPoint, QPointF(90, 80));

    unit.release();
}

TEST(CGraphicsEllipseItemFuncTest, LoadGraphicsUnit_NullData_OnlyLoadsHead)
{
    CGraphicsUnit unit;
    unit.head.dataType = EllipseType;

    CGraphicsEllipseItem dest(0, 0, 50, 50);
    QPointF origTL = dest.m_topLeftPoint;

    dest.loadGraphicsUnit(unit);

    EXPECT_EQ(dest.m_topLeftPoint, origTL);
}

// =========================================================================
// CGraphicsTriangleItem — calcPoints_helper
// =========================================================================

TEST(CGraphicsTriangleItemFuncTest, CalcPointsHelper_ZeroOffset_ReturnsThreePoints)
{
    CGraphicsTriangleItem item(QRectF(0, 0, 100, 80));

    QVector<QPointF> points;
    item.calcPoints_helper(points, 0.0);

    EXPECT_EQ(points.size(), 3);
}

TEST(CGraphicsTriangleItemFuncTest, CalcPointsHelper_NonZeroOffset_ReturnsThreePoints)
{
    CGraphicsTriangleItem item(QRectF(0, 0, 100, 80));

    QVector<QPointF> points;
    item.calcPoints_helper(points, 5.0);

    EXPECT_EQ(points.size(), 3);
}

TEST(CGraphicsTriangleItemFuncTest, CalcPointsHelper_TopPointAtTopCenter)
{
    CGraphicsTriangleItem item(QRectF(0, 0, 100, 80));

    QVector<QPointF> points;
    item.calcPoints_helper(points, 0.0);

    // The top vertex should be at the horizontal center of the rect
    EXPECT_NEAR(points[0].x(), 50.0, 1.0);
}

// =========================================================================
// CGraphicsTriangleItem — getSelfOrgShape
// =========================================================================

TEST(CGraphicsTriangleItemFuncTest, GetSelfOrgShape_ReturnsNonEmptyPath)
{
    CGraphicsTriangleItem item(QRectF(0, 0, 100, 80));

    QPainterPath path = item.getSelfOrgShape();
    EXPECT_FALSE(path.isEmpty());
}

TEST(CGraphicsTriangleItemFuncTest, GetSelfOrgShape_BoundingRectMatchesItem)
{
    CGraphicsTriangleItem item(QRectF(10, 20, 100, 80));

    QPainterPath path = item.getSelfOrgShape();
    QRectF br = path.boundingRect();
    EXPECT_NEAR(br.width(), 100.0, 5.0);
    EXPECT_NEAR(br.height(), 80.0, 5.0);
}

// =========================================================================
// CGraphicsTriangleItem — getGraphicsUnit
// =========================================================================

TEST(CGraphicsTriangleItemFuncTest, GetGraphicsUnit_ReturnsCorrectDataType)
{
    CGraphicsTriangleItem item(QRectF(10, 20, 80, 60));

    CGraphicsUnit unit = item.getGraphicsUnit(EDuplicate);

    EXPECT_EQ(unit.head.dataType, TriangleType);
    unit.release();
}

TEST(CGraphicsTriangleItemFuncTest, GetGraphicsUnit_StoresTriangleData)
{
    CGraphicsTriangleItem item(QRectF(10, 20, 80, 60));

    CGraphicsUnit unit = item.getGraphicsUnit(EDuplicate);

    ASSERT_NE(unit.data.pTriangle, nullptr);
    EXPECT_EQ(unit.data.pTriangle->rect.topLeft, QPointF(10, 20));
    EXPECT_EQ(unit.data.pTriangle->rect.bottomRight, QPointF(90, 80));
    unit.release();
}

// =========================================================================
// CGraphicsTriangleItem — loadGraphicsUnit
// =========================================================================

TEST(CGraphicsTriangleItemFuncTest, LoadGraphicsUnit_RestoresTriangleData)
{
    CGraphicsTriangleItem source(QRectF(10, 20, 80, 60));
    CGraphicsUnit unit = source.getGraphicsUnit(EDuplicate);

    CGraphicsTriangleItem dest(QRectF(0, 0, 1, 1));
    dest.loadGraphicsUnit(unit);

    EXPECT_EQ(dest.m_topLeftPoint, QPointF(10, 20));
    EXPECT_EQ(dest.m_bottomRightPoint, QPointF(90, 80));

    unit.release();
}

TEST(CGraphicsTriangleItemFuncTest, LoadGraphicsUnit_NullData_OnlyLoadsHead)
{
    CGraphicsUnit unit;
    unit.head.dataType = TriangleType;

    CGraphicsTriangleItem dest(QRectF(0, 0, 50, 50));
    QPointF origTL = dest.m_topLeftPoint;

    dest.loadGraphicsUnit(unit);

    EXPECT_EQ(dest.m_topLeftPoint, origTL);
}
