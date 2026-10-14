// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CGraphicsLineItem (issue V-5697)
 *
 * Target methods (13):
 *   setLine, setLineStartType/getLineStartType,
 *   setLineEndType/getLineEndType, initLine,
 *   drawStart, drawEnd, calcVertexes,
 *   getSelfOrgShape, isPosPenetrable,
 *   loadGraphicsUnit, getGraphicsUnit
 *
 * Source: src/drawshape/drawItems/bzItems/cgraphicslineitem.cpp
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QtCore/QtGlobal>
#include <QPen>
#include <QPointF>
#include <QLineF>
#include <QPainterPath>

#define protected public
#define private public
#include "drawshape/drawItems/bzItems/cgraphicslineitem.h"
#include "drawshape/drawItems/bzItems/cgraphicsitem.h"
#include "drawshape/sitemdata.h"
#include "drawshape/globaldefine.h"
#undef protected
#undef private

// =========================================================================
// setLine
// =========================================================================

TEST(CGraphicsLineItemFuncTest, SetLine_UpdatesCoordinates)
{
    CGraphicsLineItem item(0, 0, 10, 10);
    item.setLine(5, 5, 50, 50);

    QLineF line = item.line();
    EXPECT_EQ(line.p1(), QPointF(5, 5));
    EXPECT_EQ(line.p2(), QPointF(50, 50));
}

TEST(CGraphicsLineItemFuncTest, SetLine_WithInitTrue_ResetsArrows)
{
    CGraphicsLineItem item(0, 0, 10, 10);
    item.setLineStartType(normalArrow);
    item.setLine(0, 0, 100, 100, true);

    // After init, line should be updated
    EXPECT_EQ(item.line().p1(), QPointF(0, 0));
    EXPECT_EQ(item.line().p2(), QPointF(100, 100));
}

// =========================================================================
// setLineStartType / getLineStartType
// =========================================================================

TEST(CGraphicsLineItemFuncTest, SetLineStartType_NormalArrow_GetReturnsSame)
{
    CGraphicsLineItem item(0, 0, 100, 100);
    item.setLineStartType(normalArrow);
    EXPECT_EQ(item.getLineStartType(), normalArrow);
}

TEST(CGraphicsLineItemFuncTest, SetLineStartType_SoildArrow_GetReturnsSame)
{
    CGraphicsLineItem item(0, 0, 100, 100);
    item.setLineStartType(soildArrow);
    EXPECT_EQ(item.getLineStartType(), soildArrow);
}

TEST(CGraphicsLineItemFuncTest, SetLineStartType_NormalRing_GetReturnsSame)
{
    CGraphicsLineItem item(0, 0, 100, 100);
    item.setLineStartType(normalRing);
    EXPECT_EQ(item.getLineStartType(), normalRing);
}

TEST(CGraphicsLineItemFuncTest, SetLineStartType_SoildRing_GetReturnsSame)
{
    CGraphicsLineItem item(0, 0, 100, 100);
    item.setLineStartType(soildRing);
    EXPECT_EQ(item.getLineStartType(), soildRing);
}

TEST(CGraphicsLineItemFuncTest, GetLineStartType_DefaultCanBeSetAndRetrieved)
{
    CGraphicsLineItem item(0, 0, 100, 100);
    item.setLineStartType(noneLine);
    EXPECT_EQ(item.getLineStartType(), noneLine);
}

// =========================================================================
// setLineEndType / getLineEndType
// =========================================================================

TEST(CGraphicsLineItemFuncTest, SetLineEndType_NormalArrow_GetReturnsSame)
{
    CGraphicsLineItem item(0, 0, 100, 100);
    item.setLineEndType(normalArrow);
    EXPECT_EQ(item.getLineEndType(), normalArrow);
}

TEST(CGraphicsLineItemFuncTest, SetLineEndType_SoildArrow_GetReturnsSame)
{
    CGraphicsLineItem item(0, 0, 100, 100);
    item.setLineEndType(soildArrow);
    EXPECT_EQ(item.getLineEndType(), soildArrow);
}

TEST(CGraphicsLineItemFuncTest, SetLineEndType_NormalRing_GetReturnsSame)
{
    CGraphicsLineItem item(0, 0, 100, 100);
    item.setLineEndType(normalRing);
    EXPECT_EQ(item.getLineEndType(), normalRing);
}

TEST(CGraphicsLineItemFuncTest, SetLineEndType_SoildRing_GetReturnsSame)
{
    CGraphicsLineItem item(0, 0, 100, 100);
    item.setLineEndType(soildRing);
    EXPECT_EQ(item.getLineEndType(), soildRing);
}

TEST(CGraphicsLineItemFuncTest, GetLineEndType_DefaultCanBeSetAndRetrieved)
{
    CGraphicsLineItem item(0, 0, 100, 100);
    item.setLineEndType(noneLine);
    EXPECT_EQ(item.getLineEndType(), noneLine);
}

// =========================================================================
// initLine
// =========================================================================

TEST(CGraphicsLineItemFuncTest, InitLine_CreatesHandleRects)
{
    CGraphicsLineItem item(0, 0, 100, 100);
    item.initLine();

    EXPECT_NE(item.leftTop, nullptr);
    EXPECT_NE(item.rightBottom, nullptr);
}

TEST(CGraphicsLineItemFuncTest, InitLine_SetsItemFlags)
{
    CGraphicsLineItem item(0, 0, 100, 100);
    item.initLine();

    EXPECT_TRUE(item.flags() & QGraphicsItem::ItemIsMovable);
    EXPECT_TRUE(item.flags() & QGraphicsItem::ItemIsSelectable);
    EXPECT_TRUE(item.flags() & QGraphicsItem::ItemSendsGeometryChanges);
    EXPECT_TRUE(item.acceptHoverEvents());
}

// =========================================================================
// drawStart
// =========================================================================

TEST(CGraphicsLineItemFuncTest, DrawStart_NoneLine_CreatesStartPath)
{
    CGraphicsLineItem item(0, 0, 100, 100);
    item.setLineStartType(noneLine);
    item.drawStart();

    EXPECT_GT(item.m_startPath.elementCount(), 0);
}

TEST(CGraphicsLineItemFuncTest, DrawStart_NormalArrow_CreatesNonEmptyPath)
{
    CGraphicsLineItem item(0, 0, 100, 100);
    item.setLineStartType(normalArrow);
    item.drawStart();

    EXPECT_FALSE(item.m_startPath.isEmpty());
}

TEST(CGraphicsLineItemFuncTest, DrawStart_SoildArrow_CreatesNonEmptyPath)
{
    CGraphicsLineItem item(0, 0, 100, 100);
    item.setLineStartType(soildArrow);
    item.drawStart();

    EXPECT_FALSE(item.m_startPath.isEmpty());
}

TEST(CGraphicsLineItemFuncTest, DrawStart_SamePoints_ReturnsEarly)
{
    CGraphicsLineItem item(10, 10, 10, 10);
    item.drawStart();

    // With identical points, the path should remain empty
    EXPECT_TRUE(item.m_startPath.isEmpty());
}

// =========================================================================
// drawEnd
// =========================================================================

TEST(CGraphicsLineItemFuncTest, DrawEnd_NoneLine_CreatesEndPath)
{
    CGraphicsLineItem item(0, 0, 100, 100);
    item.setLineEndType(noneLine);
    item.drawEnd();

    EXPECT_GT(item.m_endPath.elementCount(), 0);
}

TEST(CGraphicsLineItemFuncTest, DrawEnd_NormalArrow_CreatesNonEmptyPath)
{
    CGraphicsLineItem item(0, 0, 100, 100);
    item.setLineEndType(normalArrow);
    item.drawEnd();

    EXPECT_FALSE(item.m_endPath.isEmpty());
}

TEST(CGraphicsLineItemFuncTest, DrawEnd_SoildArrow_CreatesNonEmptyPath)
{
    CGraphicsLineItem item(0, 0, 100, 100);
    item.setLineEndType(soildArrow);
    item.drawEnd();

    EXPECT_FALSE(item.m_endPath.isEmpty());
}

TEST(CGraphicsLineItemFuncTest, DrawEnd_NormalRing_CreatesNonEmptyPath)
{
    CGraphicsLineItem item(0, 0, 100, 100);
    item.setLineEndType(normalRing);
    item.drawEnd();

    EXPECT_FALSE(item.m_endPath.isEmpty());
}

TEST(CGraphicsLineItemFuncTest, DrawEnd_SamePoints_ReturnsEarly)
{
    CGraphicsLineItem item(10, 10, 10, 10);
    item.drawEnd();

    EXPECT_TRUE(item.m_endPath.isEmpty());
}

// =========================================================================
// calcVertexes
// =========================================================================

TEST(CGraphicsLineItemFuncTest, CalcVertexes_UpdatesStartAndEndPaths)
{
    CGraphicsLineItem item(0, 0, 100, 100);
    item.setLineStartType(normalArrow);
    item.setLineEndType(soildArrow);
    item.calcVertexes();

    EXPECT_FALSE(item.m_startPath.isEmpty());
    EXPECT_FALSE(item.m_endPath.isEmpty());
}

TEST(CGraphicsLineItemFuncTest, CalcVertexes_NoneLineTypes_PathsNotEmpty)
{
    CGraphicsLineItem item(10, 20, 80, 90);
    item.setLineStartType(noneLine);
    item.setLineEndType(noneLine);
    item.calcVertexes();

    // With noneLine, paths are set to a single point (elementCount > 0)
    EXPECT_GT(item.m_startPath.elementCount(), 0);
    EXPECT_GT(item.m_endPath.elementCount(), 0);
}

// =========================================================================
// getSelfOrgShape
// =========================================================================

TEST(CGraphicsLineItemFuncTest, GetSelfOrgShape_ValidLine_ReturnsNonEmptyPath)
{
    CGraphicsLineItem item(0, 0, 100, 100);
    item.drawStart();
    item.drawEnd();

    QPainterPath path = item.getSelfOrgShape();
    EXPECT_FALSE(path.isEmpty());
}

TEST(CGraphicsLineItemFuncTest, GetSelfOrgShape_DefaultLine_ReturnsNonEmptyPath)
{
    CGraphicsLineItem item(0, 0, 100, 100);

    QPainterPath path = item.getSelfOrgShape();
    EXPECT_FALSE(path.isEmpty());
}

TEST(CGraphicsLineItemFuncTest, GetSelfOrgShape_NullLine_ReturnsEmptyPath)
{
    CGraphicsLineItem item(0, 0, 0, 0);

    QPainterPath path = item.getSelfOrgShape();
    EXPECT_TRUE(path.isEmpty());
}

// =========================================================================
// isPosPenetrable
// =========================================================================

TEST(CGraphicsLineItemFuncTest, IsPosPenetrable_AlwaysReturnsFalse)
{
    CGraphicsLineItem item(0, 0, 100, 100);

    EXPECT_FALSE(item.isPosPenetrable(QPointF(50, 50)));
}

TEST(CGraphicsLineItemFuncTest, IsPosPenetrable_AnyPosition_ReturnsFalse)
{
    CGraphicsLineItem item(0, 0, 100, 100);

    EXPECT_FALSE(item.isPosPenetrable(QPointF(0, 0)));
    EXPECT_FALSE(item.isPosPenetrable(QPointF(-10, -10)));
}

// =========================================================================
// getGraphicsUnit
// =========================================================================

TEST(CGraphicsLineItemFuncTest, GetGraphicsUnit_ReturnsCorrectDataType)
{
    CGraphicsLineItem item(10, 20, 80, 90);
    item.setLineStartType(normalArrow);
    item.setLineEndType(soildArrow);

    CGraphicsUnit unit = item.getGraphicsUnit(EDuplicate);

    EXPECT_EQ(unit.head.dataType, LineType);
    unit.release();
}

TEST(CGraphicsLineItemFuncTest, GetGraphicsUnit_StoresLinePoints)
{
    CGraphicsLineItem item(10, 20, 80, 90);

    CGraphicsUnit unit = item.getGraphicsUnit(ESaveToDDf);

    ASSERT_NE(unit.data.pLine, nullptr);
    EXPECT_EQ(unit.data.pLine->point1, QPointF(10, 20));
    EXPECT_EQ(unit.data.pLine->point2, QPointF(80, 90));
    unit.release();
}

TEST(CGraphicsLineItemFuncTest, GetGraphicsUnit_StoresLineTypes)
{
    CGraphicsLineItem item(0, 0, 100, 100);
    item.setLineStartType(normalArrow);
    item.setLineEndType(soildRing);

    CGraphicsUnit unit = item.getGraphicsUnit(EDuplicate);

    ASSERT_NE(unit.data.pLine, nullptr);
    EXPECT_EQ(unit.data.pLine->start_type, normalArrow);
    EXPECT_EQ(unit.data.pLine->end_type, soildRing);
    unit.release();
}

// =========================================================================
// loadGraphicsUnit
// =========================================================================

TEST(CGraphicsLineItemFuncTest, LoadGraphicsUnit_RestoresLineAndTypes)
{
    CGraphicsLineItem source(10, 20, 80, 90);
    source.setLineStartType(normalArrow);
    source.setLineEndType(soildArrow);

    CGraphicsUnit unit = source.getGraphicsUnit(EDuplicate);

    CGraphicsLineItem dest(0, 0, 1, 1);
    dest.loadGraphicsUnit(unit);

    EXPECT_EQ(dest.line().p1(), QPointF(10, 20));
    EXPECT_EQ(dest.line().p2(), QPointF(80, 90));
    EXPECT_EQ(dest.getLineStartType(), normalArrow);
    EXPECT_EQ(dest.getLineEndType(), soildArrow);

    unit.release();
}

TEST(CGraphicsLineItemFuncTest, LoadGraphicsUnit_NullLineData_OnlyLoadsHead)
{
    CGraphicsUnit unit;
    unit.head.dataType = LineType;

    CGraphicsLineItem dest(5, 5, 50, 50);
    QLineF origLine = dest.line();

    dest.loadGraphicsUnit(unit);

    // With null pLine, the line should remain unchanged
    EXPECT_EQ(dest.line(), origLine);
}

TEST(CGraphicsLineItemFuncTest, LoadGraphicsUnit_RoundTrip_PreservesData)
{
    CGraphicsLineItem source(15, 25, 75, 95);
    source.setLineStartType(soildRing);
    source.setLineEndType(normalRing);

    CGraphicsUnit unit = source.getGraphicsUnit(EDuplicate);

    CGraphicsLineItem dest(0, 0, 1, 1);
    dest.loadGraphicsUnit(unit);

    CGraphicsUnit unit2 = dest.getGraphicsUnit(EDuplicate);

    ASSERT_NE(unit2.data.pLine, nullptr);
    EXPECT_EQ(unit2.data.pLine->point1, QPointF(15, 25));
    EXPECT_EQ(unit2.data.pLine->point2, QPointF(75, 95));
    EXPECT_EQ(unit2.data.pLine->start_type, soildRing);
    EXPECT_EQ(unit2.data.pLine->end_type, normalRing);

    unit.release();
    unit2.release();
}
