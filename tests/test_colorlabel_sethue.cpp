// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for ColorLabel::setHue (issue V-5645).
 *
 * Source: src/widgets/colorlabel.cpp
 *
 * Target method: ColorLabel::setHue(int hue)
 * (ColorLabel::getColor already covered in testItems/test_colorlabel.cpp)
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QColor>

#define protected public
#define private public
#include "colorlabel.h"
#undef protected
#undef private

TEST(ColorLabelSetHue, SetValidHue_StoresValue)
{
    ColorLabel label;
    label.setHue(120);

    EXPECT_EQ(label.m_hue, 120);
}

TEST(ColorLabelSetHue, SetZeroHue)
{
    ColorLabel label;
    label.setHue(0);

    EXPECT_EQ(label.m_hue, 0);
}

TEST(ColorLabelSetHue, SetMaxHue)
{
    ColorLabel label;
    label.setHue(359);

    EXPECT_EQ(label.m_hue, 359);
}

TEST(ColorLabelSetHue, OverwritePreviousHue)
{
    ColorLabel label;
    label.setHue(100);
    label.setHue(200);

    EXPECT_EQ(label.m_hue, 200);
}
