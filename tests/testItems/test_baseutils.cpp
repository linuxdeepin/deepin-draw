// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QColor>
#include <QCursor>
#include <QPixmap>
#include <QList>

#define protected public
#define private public
#include "publicApi.h"
#include "utils/baseutils.h"
#undef protected
#undef private

/*
 * Branch map — test_baseutils.cpp
 *
 * Module 5: baseutils (src/utils/baseutils.h/.cpp)
 *   specifiedColorList()  B1 returns exactly 32 colors
 *                         B2 first color is white with alpha==0
 *                         B3 all colors are valid QColor
 *   pickColorCursor()     B4 cursor pixmap non-null
 *                         B5 pixmap size 11x11
 *   CutRation enum        B6 all 5 values usable in switch
 *
 * NOTE: stringWidth() is declared but implementation is commented out —
 *       must NOT be called (link failure).
 */

#if TEST_BASEUTILS_ITEM

// ---------------------------------------------------------------------------
// specifiedColorList()
// ---------------------------------------------------------------------------

TEST(SpecifiedColorListTest, Returns32Colors)
{
    QList<QColor> colors = specifiedColorList();
    EXPECT_EQ(colors.size(), 32);
}

TEST(SpecifiedColorListTest, FirstColorIsTransparentWhite)
{
    QList<QColor> colors = specifiedColorList();
    ASSERT_FALSE(colors.isEmpty());
    QColor first = colors.first();
    EXPECT_EQ(first.red(), 255);
    EXPECT_EQ(first.green(), 255);
    EXPECT_EQ(first.blue(), 255);
    EXPECT_EQ(first.alpha(), 0);
}

TEST(SpecifiedColorListTest, AllColorsValid)
{
    QList<QColor> colors = specifiedColorList();
    for (const QColor &color : colors) {
        EXPECT_TRUE(color.isValid());
    }
}

TEST(SpecifiedColorListTest, LastColorIsBlack)
{
    QList<QColor> colors = specifiedColorList();
    ASSERT_FALSE(colors.isEmpty());
    QColor last = colors.last();
    EXPECT_EQ(last, QColor("#000000"));
}

// ---------------------------------------------------------------------------
// pickColorCursor()
// ---------------------------------------------------------------------------

TEST(PickColorCursorTest, ReturnsValidCursor)
{
    QCursor cursor = pickColorCursor();
    QPixmap pix = cursor.pixmap();
    EXPECT_FALSE(pix.isNull());
}

TEST(PickColorCursorTest, PixmapSizeIs11x11)
{
    QCursor cursor = pickColorCursor();
    QPixmap pix = cursor.pixmap();
    EXPECT_EQ(pix.width(), 11);
    EXPECT_EQ(pix.height(), 11);
}

// ---------------------------------------------------------------------------
// CutRation enum
// ---------------------------------------------------------------------------

TEST(CutRationEnumTest, AllValuesUsableInSwitch)
{
    CutRation values[] = {Ration4_3, Ration8_5, Ration16_9, Ration1_1, FreeRation};
    for (CutRation r : values) {
        int result = 0;
        switch (r) {
        case Ration4_3:   result = 1; break;
        case Ration8_5:   result = 2; break;
        case Ration16_9:  result = 3; break;
        case Ration1_1:   result = 4; break;
        case FreeRation:  result = 5; break;
        }
        EXPECT_NE(result, 0);
    }
}

TEST(CutRationEnumTest, DistinctValues)
{
    EXPECT_NE(Ration4_3, Ration8_5);
    EXPECT_NE(Ration8_5, Ration16_9);
    EXPECT_NE(Ration16_9, Ration1_1);
    EXPECT_NE(Ration1_1, FreeRation);
}

#endif // TEST_BASEUTILS_ITEM
