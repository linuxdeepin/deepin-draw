// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CPictureItem (issue V-5697)
 *
 * Target methods (9):
 *   setAngle, setPixmap/pixmap, isBlurEnable,
 *   getSelfOrgShape, isPosPenetrable, isRectPenetrable,
 *   loadGraphicsUnit, getGraphicsUnit
 *
 * Source: src/drawshape/drawItems/bzItems/cpictureitem.cpp
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QtCore/QtGlobal>
#include <QPen>
#include <QPointF>
#include <QPixmap>
#include <QImage>
#include <QPainterPath>
#include <QRectF>
#include <QByteArray>

#define protected public
#define private public
#include "drawshape/drawItems/bzItems/cpictureitem.h"
#include "drawshape/drawItems/bzItems/cgraphicsrectitem.h"
#include "drawshape/drawItems/bzItems/cgraphicsitem.h"
#include "drawshape/sitemdata.h"
#include "drawshape/globaldefine.h"
#undef protected
#undef private

// Helper to create a test pixmap
static QPixmap makeTestPixmap(int w = 50, int h = 50)
{
    QPixmap pix(w, h);
    pix.fill(Qt::red);
    return pix;
}

// =========================================================================
// setAngle
// =========================================================================

TEST(CPictureItemFuncTest, SetAngle_StoresValue)
{
    CPictureItem item(QRectF(0, 0, 100, 80), makeTestPixmap());
    item.setAngle(45.0);
    EXPECT_DOUBLE_EQ(item.m_angle, 45.0);
}

TEST(CPictureItemFuncTest, SetAngle_ZeroValue)
{
    CPictureItem item(QRectF(0, 0, 100, 80), makeTestPixmap());
    item.setAngle(0.0);
    EXPECT_DOUBLE_EQ(item.m_angle, 0.0);
}

TEST(CPictureItemFuncTest, SetAngle_NegativeValue)
{
    CPictureItem item(QRectF(0, 0, 100, 80), makeTestPixmap());
    item.setAngle(-90.0);
    EXPECT_DOUBLE_EQ(item.m_angle, -90.0);
}

// =========================================================================
// setPixmap / pixmap
// =========================================================================

TEST(CPictureItemFuncTest, SetPixmap_UpdatesPixmap)
{
    CPictureItem item(QRectF(0, 0, 100, 80), makeTestPixmap());
    QPixmap newPix(30, 30);
    newPix.fill(Qt::blue);
    item.setPixmap(newPix);

    EXPECT_EQ(item.pixmap().width(), 30);
    EXPECT_EQ(item.pixmap().height(), 30);
}

TEST(CPictureItemFuncTest, Pixmap_ReturnsReference)
{
    QPixmap origPix = makeTestPixmap(60, 40);
    CPictureItem item(QRectF(0, 0, 100, 80), origPix);

    QPixmap &ref = item.pixmap();
    EXPECT_EQ(ref.width(), 60);
    EXPECT_EQ(ref.height(), 40);
}

TEST(CPictureItemFuncTest, SetPixmap_ReplaceWithDifferentSize)
{
    CPictureItem item(QRectF(0, 0, 100, 80), makeTestPixmap(50, 50));
    item.setPixmap(makeTestPixmap(100, 200));

    EXPECT_EQ(item.pixmap().width(), 100);
    EXPECT_EQ(item.pixmap().height(), 200);
}

// =========================================================================
// isBlurEnable
// =========================================================================

TEST(CPictureItemFuncTest, IsBlurEnable_ReturnsTrue)
{
    CPictureItem item(QRectF(0, 0, 100, 80), makeTestPixmap());
    EXPECT_TRUE(item.isBlurEnable());
}

// =========================================================================
// getSelfOrgShape
// =========================================================================

TEST(CPictureItemFuncTest, GetSelfOrgShape_ReturnsNonEmptyPath)
{
    CPictureItem item(QRectF(0, 0, 100, 80), makeTestPixmap());
    QPainterPath path = item.getSelfOrgShape();
    EXPECT_FALSE(path.isEmpty());
}

TEST(CPictureItemFuncTest, GetSelfOrgShape_MatchesItemRect)
{
    CPictureItem item(QRectF(10, 20, 100, 80), makeTestPixmap());
    QPainterPath path = item.getSelfOrgShape();
    QRectF br = path.boundingRect();

    EXPECT_NEAR(br.x(), 10.0, 1.0);
    EXPECT_NEAR(br.y(), 20.0, 1.0);
    EXPECT_NEAR(br.width(), 100.0, 1.0);
    EXPECT_NEAR(br.height(), 80.0, 1.0);
}

TEST(CPictureItemFuncTest, GetSelfOrgShape_IsSimpleRect)
{
    CPictureItem item(QRectF(0, 0, 100, 80), makeTestPixmap());
    QPainterPath path = item.getSelfOrgShape();
    // A simple rect path has 5 elements (4 lines + close)
    EXPECT_EQ(path.elementCount(), 5);
}

// =========================================================================
// isPosPenetrable
// =========================================================================

TEST(CPictureItemFuncTest, IsPosPenetrable_ReturnsFalse)
{
    CPictureItem item(QRectF(0, 0, 100, 80), makeTestPixmap());
    EXPECT_FALSE(item.isPosPenetrable(QPointF(50, 40)));
}

TEST(CPictureItemFuncTest, IsPosPenetrable_AnyPosition_ReturnsFalse)
{
    CPictureItem item(QRectF(0, 0, 100, 80), makeTestPixmap());
    EXPECT_FALSE(item.isPosPenetrable(QPointF(0, 0)));
    EXPECT_FALSE(item.isPosPenetrable(QPointF(100, 80)));
}

// =========================================================================
// isRectPenetrable
// =========================================================================

TEST(CPictureItemFuncTest, IsRectPenetrable_ReturnsFalse)
{
    CPictureItem item(QRectF(0, 0, 100, 80), makeTestPixmap());
    EXPECT_FALSE(item.isRectPenetrable(QRectF(10, 10, 50, 50)));
}

TEST(CPictureItemFuncTest, IsRectPenetrable_AnyRect_ReturnsFalse)
{
    CPictureItem item(QRectF(0, 0, 100, 80), makeTestPixmap());
    EXPECT_FALSE(item.isRectPenetrable(QRectF(0, 0, 100, 80)));
    EXPECT_FALSE(item.isRectPenetrable(QRectF(200, 200, 10, 10)));
}

// =========================================================================
// getGraphicsUnit
// =========================================================================

TEST(CPictureItemFuncTest, GetGraphicsUnit_ReturnsCorrectDataType)
{
    CPictureItem item(QRectF(10, 20, 80, 60), makeTestPixmap());
    CGraphicsUnit unit = item.getGraphicsUnit(EDuplicate);

    EXPECT_EQ(unit.head.dataType, PictureType);
    unit.release();
}

TEST(CPictureItemFuncTest, GetGraphicsUnit_StoresPictureData)
{
    CPictureItem item(QRectF(10, 20, 80, 60), makeTestPixmap());
    CGraphicsUnit unit = item.getGraphicsUnit(EDuplicate);

    ASSERT_NE(unit.data.pPic, nullptr);
    EXPECT_EQ(unit.data.pPic->rect.topLeft, QPointF(10, 20));
    EXPECT_EQ(unit.data.pPic->rect.bottomRight, QPointF(90, 80));
    unit.release();
}

TEST(CPictureItemFuncTest, GetGraphicsUnit_EDuplicate_StoresImageData)
{
    CPictureItem item(QRectF(0, 0, 50, 50), makeTestPixmap());
    CGraphicsUnit unit = item.getGraphicsUnit(EDuplicate);

    ASSERT_NE(unit.data.pPic, nullptr);
    EXPECT_FALSE(unit.data.pPic->image.isNull());
    unit.release();
}

TEST(CPictureItemFuncTest, GetGraphicsUnit_NonDuplicate_OmitsImageData)
{
    CPictureItem item(QRectF(0, 0, 50, 50), makeTestPixmap());
    CGraphicsUnit unit = item.getGraphicsUnit(ENormal);

    ASSERT_NE(unit.data.pPic, nullptr);
    // Image data should not be populated for non-duplicate reasons
    EXPECT_TRUE(unit.data.pPic->image.isNull());
    unit.release();
}

// =========================================================================
// loadGraphicsUnit
// =========================================================================

TEST(CPictureItemFuncTest, LoadGraphicsUnit_RestoresRectData)
{
    CPictureItem source(QRectF(10, 20, 80, 60), makeTestPixmap());
    CGraphicsUnit unit = source.getGraphicsUnit(EDuplicate);

    CPictureItem dest(QRectF(0, 0, 1, 1), makeTestPixmap());
    dest.loadGraphicsUnit(unit);

    QRectF r = dest.rect();
    EXPECT_NEAR(r.x(), 10.0, 1.0);
    EXPECT_NEAR(r.y(), 20.0, 1.0);
    EXPECT_NEAR(r.width(), 80.0, 1.0);
    EXPECT_NEAR(r.height(), 60.0, 1.0);

    unit.release();
}

TEST(CPictureItemFuncTest, LoadGraphicsUnit_EDuplicate_RestoresPixmap)
{
    CPictureItem source(QRectF(0, 0, 50, 50), makeTestPixmap(30, 30));
    CGraphicsUnit unit = source.getGraphicsUnit(EDuplicate);

    CPictureItem dest(QRectF(0, 0, 1, 1), makeTestPixmap(10, 10));
    dest.loadGraphicsUnit(unit);

    EXPECT_EQ(dest.pixmap().width(), 30);
    EXPECT_EQ(dest.pixmap().height(), 30);

    unit.release();
}

TEST(CPictureItemFuncTest, LoadGraphicsUnit_RestoresFlipFlags)
{
    CPictureItem source(QRectF(0, 0, 50, 50), makeTestPixmap());
    source._flipHorizontal = true;
    source._flipVertical = true;
    CGraphicsUnit unit = source.getGraphicsUnit(EDuplicate);

    CPictureItem dest(QRectF(0, 0, 1, 1), makeTestPixmap());
    dest.loadGraphicsUnit(unit);

    EXPECT_TRUE(dest._flipHorizontal);
    EXPECT_TRUE(dest._flipVertical);

    unit.release();
}

TEST(CPictureItemFuncTest, LoadGraphicsUnit_NullData_OnlyLoadsHead)
{
    CGraphicsUnit unit;
    unit.head.dataType = PictureType;

    CPictureItem dest(QRectF(0, 0, 50, 50), makeTestPixmap());
    QRectF origRect = dest.rect();

    dest.loadGraphicsUnit(unit);

    // Rect should remain unchanged when data.pPic is null
    EXPECT_EQ(dest.rect(), origRect);
}
