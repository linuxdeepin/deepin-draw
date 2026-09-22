// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CGraphicsUnit::deepCopy and CGraphicsUnit::release
 *
 * Target methods (from issue V-5046):
 *   - CGraphicsUnit::deepCopy  (level=high, complexity:35, lines:134)
 *   - CGraphicsUnit::release   (level=high, complexity:26, lines:81)
 *
 * Source: src/drawshape/sitemdata.h (inline)
 *
 * Minimum case counts:
 *   deepCopy: high + complexity≥20 → 3+1 = 4 cases
 *   release:  high + complexity≥20 → 3+1 = 4 cases
 *
 * | Method              | level | min | actual |
 * |---------------------|-------|-----|--------|
 * | deepCopy            | high  | 4   | 4      |
 * | release             | high  | 4   | 4      |
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

// Include Qt headers BEFORE #define private to avoid breaking Qt internals.
#include <QtCore/QtGlobal>
#include <QPen>
#include <QBrush>
#include <QPointF>

#define protected public
#define private public
#include "drawshape/sitemdata.h"
#include "drawshape/globaldefine.h"
#undef protected
#undef private

// =========================================================================
// CGraphicsUnit::deepCopy tests
// =========================================================================

TEST(CGraphicsUnitDeepCopy, RectType_ValidSource_CopiesDataAndHead)
{
    // Arrange
    CGraphicsUnit source;
    source.head.dataType = RectType;
    source.head.pos = QPointF(10, 20);
    source.head.rotate = 45.0;
    source.data.pRect = new SGraphicsRectUnitData;
    source.data.pRect->topLeft = QPointF(1, 2);
    source.data.pRect->bottomRight = QPointF(3, 4);

    CGraphicsUnit dest;

    // Act
    CGraphicsUnit::deepCopy(dest, source);

    // Assert
    ASSERT_NE(dest.data.pRect, nullptr);
    EXPECT_EQ(dest.head.dataType, RectType);
    EXPECT_EQ(dest.head.pos, QPointF(10, 20));
    EXPECT_DOUBLE_EQ(dest.head.rotate, 45.0);
    EXPECT_EQ(dest.data.pRect->topLeft, QPointF(1, 2));
    EXPECT_EQ(dest.data.pRect->bottomRight, QPointF(3, 4));

    source.release();
    dest.release();
}

TEST(CGraphicsUnitDeepCopy, EllipseType_NullSource_ReleasesDest)
{
    // Arrange
    CGraphicsUnit source;
    source.head.dataType = EllipseType;
    source.data.pCircle = nullptr;  // null source data

    CGraphicsUnit dest;
    dest.head.dataType = EllipseType;
    dest.data.pCircle = new SGraphicsCircleUnitData;

    // Act
    CGraphicsUnit::deepCopy(dest, source);

    // Assert - dest should be released (pCircle set to null by memset)
    EXPECT_EQ(dest.data.pCircle, nullptr);
}

TEST(CGraphicsUnitDeepCopy, LineType_ValidSource_CopiesData)
{
    // Arrange
    CGraphicsUnit source;
    source.head.dataType = LineType;
    source.data.pLine = new SGraphicsLineUnitData;
    source.data.pLine->point1 = QPointF(0, 0);
    source.data.pLine->point2 = QPointF(100, 100);

    CGraphicsUnit dest;

    // Act
    CGraphicsUnit::deepCopy(dest, source);

    // Assert
    ASSERT_NE(dest.data.pLine, nullptr);
    EXPECT_EQ(dest.head.dataType, LineType);
    EXPECT_EQ(dest.data.pLine->point1, QPointF(0, 0));
    EXPECT_EQ(dest.data.pLine->point2, QPointF(100, 100));

    source.release();
    dest.release();
}

TEST(CGraphicsUnitDeepCopy, RectType_NullSource_ReleasesDest)
{
    // Arrange
    CGraphicsUnit source;
    source.head.dataType = RectType;
    source.data.pRect = nullptr;  // null source data

    CGraphicsUnit dest;
    dest.data.pRect = new SGraphicsRectUnitData;

    // Act
    CGraphicsUnit::deepCopy(dest, source);

    // Assert - dest should be released (pRect set to null by memset)
    EXPECT_EQ(dest.data.pRect, nullptr);
}

// =========================================================================
// CGraphicsUnit::release tests
// =========================================================================

TEST(CGraphicsUnitRelease, RectType_DeletesDataAndZerosUnion)
{
    // Arrange
    CGraphicsUnit unit;
    unit.head.dataType = RectType;
    unit.data.pRect = new SGraphicsRectUnitData;
    unit.data.pRect->topLeft = QPointF(5, 5);

    // Act
    unit.release();

    // Assert - memset zeros the entire union, so pRect should be null
    EXPECT_EQ(unit.data.pRect, nullptr);
}

TEST(CGraphicsUnitRelease, EllipseType_DeletesDataAndZerosUnion)
{
    // Arrange
    CGraphicsUnit unit;
    unit.head.dataType = EllipseType;
    unit.data.pCircle = new SGraphicsCircleUnitData;

    // Act
    unit.release();

    // Assert
    EXPECT_EQ(unit.data.pCircle, nullptr);
}

TEST(CGraphicsUnitRelease, TextType_DeletesDataAndZerosUnion)
{
    // Arrange
    CGraphicsUnit unit;
    unit.head.dataType = TextType;
    unit.data.pText = new SGraphicsTextUnitData;

    // Act
    unit.release();

    // Assert
    EXPECT_EQ(unit.data.pText, nullptr);
}

TEST(CGraphicsUnitRelease, CalledTwice_NoCrash)
{
    // Arrange
    CGraphicsUnit unit;
    unit.head.dataType = PenType;
    unit.data.pPen = new SGraphicsPenUnitData;

    // Act
    unit.release();
    // Second call should not crash (data already zeroed)
    unit.release();

    // Assert
    EXPECT_EQ(unit.data.pPen, nullptr);
}
