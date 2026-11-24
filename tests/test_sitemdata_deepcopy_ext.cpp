// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CGraphicsUnit::deepCopy remaining type branches
 * (issue V-9255 §1 — complements test_cgraphicsunit.cpp)
 *
 * Source: src/drawshape/sitemdata.h (inline)
 *
 * Covered types (valid + null source):
 *   TriangleType, PolygonalStarType, PolygonType, PenType,
 *   PictureType, BlurType, MgrType
 * Plus TextType null-source branch.
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QtCore/QtGlobal>
#include <QPen>
#include <QBrush>
#include <QPointF>
#include <QImage>
#include <QPainterPath>

#define protected public
#define private public
#include "drawshape/sitemdata.h"
#include "drawshape/globaldefine.h"
#undef protected
#undef private

// =========================================================================
// TriangleType
// =========================================================================

TEST(SitemdataDeepCopyExt, TriangleType_ValidSource_CopiesData)
{
    CGraphicsUnit source;
    source.head.dataType = TriangleType;
    source.head.pos = QPointF(5, 5);
    source.data.pTriangle = new SGraphicsTriangleUnitData;
    source.data.pTriangle->rect.topLeft = QPointF(1, 2);
    source.data.pTriangle->rect.bottomRight = QPointF(3, 4);

    CGraphicsUnit dest;
    CGraphicsUnit::deepCopy(dest, source);

    ASSERT_NE(dest.data.pTriangle, nullptr);
    EXPECT_EQ(dest.head.dataType, TriangleType);
    EXPECT_EQ(dest.data.pTriangle->rect.topLeft, QPointF(1, 2));
    EXPECT_EQ(dest.data.pTriangle->rect.bottomRight, QPointF(3, 4));

    source.release();
    dest.release();
}

TEST(SitemdataDeepCopyExt, TriangleType_NullSource_ReleasesDest)
{
    CGraphicsUnit source;
    source.head.dataType = TriangleType;
    source.data.pTriangle = nullptr;

    CGraphicsUnit dest;
    dest.data.pTriangle = new SGraphicsTriangleUnitData;

    CGraphicsUnit::deepCopy(dest, source);
    EXPECT_EQ(dest.data.pTriangle, nullptr);
}

// =========================================================================
// PolygonalStarType
// =========================================================================

TEST(SitemdataDeepCopyExt, PolygonalStarType_ValidSource_CopiesData)
{
    CGraphicsUnit source;
    source.head.dataType = PolygonalStarType;
    source.data.pPolygonStar = new SGraphicsPolygonStarUnitData;
    source.data.pPolygonStar->anchorNum = 5;
    source.data.pPolygonStar->radius = 20;

    CGraphicsUnit dest;
    CGraphicsUnit::deepCopy(dest, source);

    ASSERT_NE(dest.data.pPolygonStar, nullptr);
    EXPECT_EQ(dest.head.dataType, PolygonalStarType);
    EXPECT_EQ(dest.data.pPolygonStar->anchorNum, 5);
    EXPECT_EQ(dest.data.pPolygonStar->radius, 20);

    source.release();
    dest.release();
}

TEST(SitemdataDeepCopyExt, PolygonalStarType_NullSource_ReleasesDest)
{
    CGraphicsUnit source;
    source.head.dataType = PolygonalStarType;
    source.data.pPolygonStar = nullptr;

    CGraphicsUnit dest;
    dest.data.pPolygonStar = new SGraphicsPolygonStarUnitData;

    CGraphicsUnit::deepCopy(dest, source);
    EXPECT_EQ(dest.data.pPolygonStar, nullptr);
}

// =========================================================================
// PolygonType
// =========================================================================

TEST(SitemdataDeepCopyExt, PolygonType_ValidSource_CopiesData)
{
    CGraphicsUnit source;
    source.head.dataType = PolygonType;
    source.data.pPolygon = new SGraphicsPolygonUnitData;
    source.data.pPolygon->pointNum = 6;

    CGraphicsUnit dest;
    CGraphicsUnit::deepCopy(dest, source);

    ASSERT_NE(dest.data.pPolygon, nullptr);
    EXPECT_EQ(dest.head.dataType, PolygonType);
    EXPECT_EQ(dest.data.pPolygon->pointNum, 6);

    source.release();
    dest.release();
}

TEST(SitemdataDeepCopyExt, PolygonType_NullSource_ReleasesDest)
{
    CGraphicsUnit source;
    source.head.dataType = PolygonType;
    source.data.pPolygon = nullptr;

    CGraphicsUnit dest;
    dest.data.pPolygon = new SGraphicsPolygonUnitData;

    CGraphicsUnit::deepCopy(dest, source);
    EXPECT_EQ(dest.data.pPolygon, nullptr);
}

// =========================================================================
// PenType
// =========================================================================

TEST(SitemdataDeepCopyExt, PenType_ValidSource_CopiesData)
{
    CGraphicsUnit source;
    source.head.dataType = PenType;
    source.data.pPen = new SGraphicsPenUnitData;
    source.data.pPen->start_type = soildArrow;
    source.data.pPen->end_type = soildArrow;

    CGraphicsUnit dest;
    CGraphicsUnit::deepCopy(dest, source);

    ASSERT_NE(dest.data.pPen, nullptr);
    EXPECT_EQ(dest.head.dataType, PenType);
    EXPECT_EQ(dest.data.pPen->start_type, soildArrow);
    EXPECT_EQ(dest.data.pPen->end_type, soildArrow);

    source.release();
    dest.release();
}

TEST(SitemdataDeepCopyExt, PenType_NullSource_ReleasesDest)
{
    CGraphicsUnit source;
    source.head.dataType = PenType;
    source.data.pPen = nullptr;

    CGraphicsUnit dest;
    dest.data.pPen = new SGraphicsPenUnitData;

    CGraphicsUnit::deepCopy(dest, source);
    EXPECT_EQ(dest.data.pPen, nullptr);
}

// =========================================================================
// TextType — null source branch (valid already covered by existing test)
// =========================================================================

TEST(SitemdataDeepCopyExt, TextType_NullSource_ReleasesDest)
{
    CGraphicsUnit source;
    source.head.dataType = TextType;
    source.data.pText = nullptr;

    CGraphicsUnit dest;
    dest.data.pText = new SGraphicsTextUnitData;

    CGraphicsUnit::deepCopy(dest, source);
    EXPECT_EQ(dest.data.pText, nullptr);
}

// =========================================================================
// PictureType
// =========================================================================

TEST(SitemdataDeepCopyExt, PictureType_ValidSource_CopiesData)
{
    CGraphicsUnit source;
    source.head.dataType = PictureType;
    source.data.pPic = new SGraphicsPictureUnitData;
    source.data.pPic->srcByteArry = QByteArray("picturedata");
    source.data.pPic->flipHorizontal = true;
    source.data.pPic->flipVertical = false;

    CGraphicsUnit dest;
    CGraphicsUnit::deepCopy(dest, source);

    ASSERT_NE(dest.data.pPic, nullptr);
    EXPECT_EQ(dest.head.dataType, PictureType);
    EXPECT_EQ(dest.data.pPic->srcByteArry, QByteArray("picturedata"));
    EXPECT_TRUE(dest.data.pPic->flipHorizontal);
    EXPECT_FALSE(dest.data.pPic->flipVertical);

    source.release();
    dest.release();
}

TEST(SitemdataDeepCopyExt, PictureType_NullSource_ReleasesDest)
{
    CGraphicsUnit source;
    source.head.dataType = PictureType;
    source.data.pPic = nullptr;

    CGraphicsUnit dest;
    dest.data.pPic = new SGraphicsPictureUnitData;

    CGraphicsUnit::deepCopy(dest, source);
    EXPECT_EQ(dest.data.pPic, nullptr);
}

// =========================================================================
// BlurType
// =========================================================================

TEST(SitemdataDeepCopyExt, BlurType_ValidSource_CopiesData)
{
    CGraphicsUnit source;
    source.head.dataType = BlurType;
    source.data.pBlur = new SGraphicsBlurUnitData;
    source.data.pBlur->effect = 1;
    source.data.pBlur->data.start_type = soildArrow;

    CGraphicsUnit dest;
    CGraphicsUnit::deepCopy(dest, source);

    ASSERT_NE(dest.data.pBlur, nullptr);
    EXPECT_EQ(dest.head.dataType, BlurType);
    EXPECT_EQ(dest.data.pBlur->effect, 1);
    EXPECT_EQ(dest.data.pBlur->data.start_type, soildArrow);

    source.release();
    dest.release();
}

TEST(SitemdataDeepCopyExt, BlurType_NullSource_ReleasesDest)
{
    CGraphicsUnit source;
    source.head.dataType = BlurType;
    source.data.pBlur = nullptr;

    CGraphicsUnit dest;
    dest.data.pBlur = new SGraphicsBlurUnitData;

    CGraphicsUnit::deepCopy(dest, source);
    EXPECT_EQ(dest.data.pBlur, nullptr);
}

// =========================================================================
// MgrType
// =========================================================================

TEST(SitemdataDeepCopyExt, MgrType_ValidSource_CopiesData)
{
    CGraphicsUnit source;
    source.head.dataType = MgrType;
    source.data.pGroup = new SGraphicsGroupUnitData;
    source.data.pGroup->name = "testgroup";
    source.data.pGroup->groupType = 2;

    CGraphicsUnit dest;
    CGraphicsUnit::deepCopy(dest, source);

    ASSERT_NE(dest.data.pGroup, nullptr);
    EXPECT_EQ(dest.head.dataType, MgrType);
    EXPECT_EQ(dest.data.pGroup->name, QString("testgroup"));
    EXPECT_EQ(dest.data.pGroup->groupType, 2);

    source.release();
    dest.release();
}

TEST(SitemdataDeepCopyExt, MgrType_NullSource_ReleasesDest)
{
    CGraphicsUnit source;
    source.head.dataType = MgrType;
    source.data.pGroup = nullptr;

    CGraphicsUnit dest;
    dest.data.pGroup = new SGraphicsGroupUnitData;

    CGraphicsUnit::deepCopy(dest, source);
    EXPECT_EQ(dest.data.pGroup, nullptr);
}
