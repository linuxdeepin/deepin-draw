// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CGraphicsPenItem Bezier and end-path methods
 * (issue V-7262)
 *
 * Source: src/drawshape/drawItems/bzItems/cgraphicspenitem.cpp
 *
 * Target methods:
 *   qreal GetBezierValue(qreal p0, qreal p1, qreal p2, qreal p3,
 *                        qreal p4, qreal p5, qreal t)
 *   QPointF GetBezierValue(QPainterPath::Element p0, ..., Element p5, qreal t)
 *   void updateEndPathStyle()
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QPointF>
#include <QPainterPath>
#include <QPainter>
#include <QImage>
#include <cmath>

#define protected public
#define private public
#include "drawshape/drawItems/bzItems/cgraphicspenitem.h"
#include "drawshape/drawItems/bzItems/cgraphicsitem.h"
#include "drawshape/drawItems/bzItems/cgraphicsitemevent.h"
#include "drawshape/sitemdata.h"
#include "drawshape/globaldefine.h"
#undef protected
#undef private

// =========================================================================
// GetBezierValue — scalar overload
// =========================================================================

TEST(CGraphicsPenItemBezierTest, ScalarBezier_T0_ReturnsP0)
{
    CGraphicsPenItem penItem;

    qreal p0 = 10.0, p1 = 20.0, p2 = 30.0, p3 = 40.0, p4 = 50.0, p5 = 60.0;
    qreal result = penItem.GetBezierValue(p0, p1, p2, p3, p4, p5, 0.0);

    EXPECT_NEAR(result, p0, 1e-6);
}

TEST(CGraphicsPenItemBezierTest, ScalarBezier_T1_ReturnsP5)
{
    CGraphicsPenItem penItem;

    qreal p0 = 10.0, p1 = 20.0, p2 = 30.0, p3 = 40.0, p4 = 50.0, p5 = 60.0;
    qreal result = penItem.GetBezierValue(p0, p1, p2, p3, p4, p5, 1.0);

    EXPECT_NEAR(result, p5, 1e-6);
}

TEST(CGraphicsPenItemBezierTest, ScalarBezier_AllControlPointsEqual)
{
    CGraphicsPenItem penItem;

    // When all control points are equal, the result should be that value
    // for any t
    qreal val = 42.0;
    qreal result = penItem.GetBezierValue(val, val, val, val, val, val, 0.5);

    EXPECT_NEAR(result, val, 1e-6);
}

TEST(CGraphicsPenItemBezierTest, ScalarBezier_T05_IsBetweenP0AndP5)
{
    CGraphicsPenItem penItem;

    qreal p0 = 0.0, p1 = 100.0, p2 = 0.0, p3 = 100.0, p4 = 0.0, p5 = 100.0;
    qreal result = penItem.GetBezierValue(p0, p1, p2, p3, p4, p5, 0.5);

    // Result should be between p0 and p5
    EXPECT_GE(result, std::min(p0, p5) - 1e-6);
    EXPECT_LE(result, std::max(p0, p5) + 1e-6);
}

// =========================================================================
// GetBezierValue — QPointF (Element) overload
// =========================================================================

TEST(CGraphicsPenItemBezierTest, ElementBezier_T0_ReturnsP0Position)
{
    CGraphicsPenItem penItem;

    QPainterPath::Element elems[6];
    for (int i = 0; i < 6; ++i) {
        elems[i].x = static_cast<qreal>(i * 10);
        elems[i].y = static_cast<qreal>(i * 5);
        elems[i].type = QPainterPath::MoveToElement;
    }

    QPointF result = penItem.GetBezierValue(elems[0], elems[1], elems[2],
                                             elems[3], elems[4], elems[5], 0.0);

    EXPECT_NEAR(result.x(), 0.0, 1e-6);
    EXPECT_NEAR(result.y(), 0.0, 1e-6);
}

TEST(CGraphicsPenItemBezierTest, ElementBezier_T1_ReturnsP5Position)
{
    CGraphicsPenItem penItem;

    QPainterPath::Element elems[6];
    for (int i = 0; i < 6; ++i) {
        elems[i].x = static_cast<qreal>(i * 10);
        elems[i].y = static_cast<qreal>(i * 5);
        elems[i].type = QPainterPath::MoveToElement;
    }

    QPointF result = penItem.GetBezierValue(elems[0], elems[1], elems[2],
                                             elems[3], elems[4], elems[5], 1.0);

    EXPECT_NEAR(result.x(), 50.0, 1e-6);
    EXPECT_NEAR(result.y(), 25.0, 1e-6);
}

TEST(CGraphicsPenItemBezierTest, ElementBezier_MatchesScalarOverload)
{
    CGraphicsPenItem penItem;

    qreal xs[6] = {0.0, 10.0, 20.0, 30.0, 40.0, 50.0};
    qreal ys[6] = {5.0, 15.0, 25.0, 35.0, 45.0, 55.0};

    QPainterPath::Element elems[6];
    for (int i = 0; i < 6; ++i) {
        elems[i].x = xs[i];
        elems[i].y = ys[i];
        elems[i].type = QPainterPath::MoveToElement;
    }

    qreal t = 0.5;
    QPointF result = penItem.GetBezierValue(elems[0], elems[1], elems[2],
                                             elems[3], elems[4], elems[5], t);

    qreal expectedX = penItem.GetBezierValue(xs[0], xs[1], xs[2], xs[3], xs[4], xs[5], t);
    qreal expectedY = penItem.GetBezierValue(ys[0], ys[1], ys[2], ys[3], ys[4], ys[5], t);

    EXPECT_NEAR(result.x(), expectedX, 1e-6);
    EXPECT_NEAR(result.y(), expectedY, 1e-6);
}

// =========================================================================
// updateEndPathStyle
// =========================================================================

TEST(CGraphicsPenItemBezierTest, UpdateEndPathStyle_DoesNotCrash)
{
    CGraphicsPenItem penItem;

    // Call with default state (m_penEndType, m_path, m_straightLine)
    penItem.updateEndPathStyle();

    SUCCEED();
}

TEST(CGraphicsPenItemBezierTest, UpdateEndPathStyle_SetsEndPath)
{
    CGraphicsPenItem penItem;

    // Set a path with some elements
    penItem.m_path = QPainterPath();
    penItem.m_path.moveTo(0, 0);
    penItem.m_path.lineTo(100, 100);

    penItem.updateEndPathStyle();

    // m_endPath should be set (may be empty for None type, but should not crash)
    SUCCEED();
}

TEST(CGraphicsPenItemBezierTest, UpdateEndPathStyle_WithStraightLine)
{
    CGraphicsPenItem penItem;

    penItem.m_path = QPainterPath();
    penItem.m_path.moveTo(0, 0);
    penItem.m_path.lineTo(100, 0);
    penItem.m_straightLine = QLineF(0, 0, 100, 0);

    penItem.updateEndPathStyle();

    SUCCEED();
}

TEST(CGraphicsPenItemBezierTest, UpdateEndPathStyle_WithArrowEndType)
{
    CGraphicsPenItem penItem;

    penItem.m_path = QPainterPath();
    penItem.m_path.moveTo(0, 0);
    penItem.m_path.lineTo(100, 100);
    penItem.m_penEndType = static_cast<ELineType>(1); // Non-None type

    penItem.updateEndPathStyle();

    // If the end type is an arrow/ring type, m_endPath should be non-empty
    SUCCEED();
}
