// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for ColorSlider::getColor (issue V-5645).
 *
 * Source: src/widgets/colorslider.cpp
 *
 * Target method: ColorSlider::getColor(qreal h, qreal s, qreal v)
 *
 * This is an HSV-to-RGB conversion with 6 hue sectors (hi = int(h/60) % 6).
 * Unlike ColorLabel::getColor, this version includes an explicit guard
 * for negative q values.
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QColor>

#include "colorslider.h"

// =========================================================================
// ColorSlider::getColor — six hue sectors
// =========================================================================

TEST(ColorSliderGetColor, FullSaturationAllSectors)
{
    struct Case { qreal h; int r, g, b; };
    // With s=1, v=1: p=0, q=(1-f), t=f
    // H0 (hi=0): (p,q,v) = (0, 1-f, 1)
    // H1 (hi=1): (t,p,v) = (f, 0, 1)
    // H2 (hi=2): (v,p,q) = (1, 0, 1-f)  -- note: no min clamp on 3rd channel
    // H3 (hi=3): (v,t,p) = (1, f, 0)
    // H4 (hi=4): (q,v,p) = (1-f, 1, 0)
    // H5 (hi=5): (p,v,t) = (0, 1, f)
    const Case cases[] = {
        {0,   0, 255, 255},   // H0
        {60,  0, 0,   255},   // H1
        {120, 255, 0, 255},   // H2
        {180, 255, 0, 0},     // H3
        {240, 255, 255, 0},   // H4
        {300, 0, 255, 0},     // H5
    };

    ColorSlider slider;
    for (const auto &c : cases) {
        QColor got = slider.getColor(c.h, 1.0, 1.0);
        EXPECT_EQ(got.red(), c.r) << "h=" << c.h;
        EXPECT_EQ(got.green(), c.g) << "h=" << c.h;
        EXPECT_EQ(got.blue(), c.b) << "h=" << c.h;
    }
}

TEST(ColorSliderGetColor, ZeroSaturation_GrayOutput)
{
    ColorSlider slider;
    const qreal hs[] = {0, 60, 120, 180, 240, 300, 359};
    for (qreal h : hs) {
        QColor got = slider.getColor(h, 0.0, 0.5);
        EXPECT_EQ(got.red(), got.green()) << "h=" << h;
        EXPECT_EQ(got.green(), got.blue()) << "h=" << h;
    }
}

TEST(ColorSliderGetColor, ZeroValue_BlackOutput)
{
    ColorSlider slider;
    QColor got = slider.getColor(120, 1.0, 0.0);
    EXPECT_EQ(got.red(), 0);
    EXPECT_EQ(got.green(), 0);
    EXPECT_EQ(got.blue(), 0);
}

TEST(ColorSliderGetColor, SectorBoundaries_NoCrash)
{
    ColorSlider slider;
    const qreal boundaries[] = {0, 60, 120, 180, 240, 300, 360};
    for (qreal h : boundaries) {
        QColor got = slider.getColor(h, 1.0, 1.0);
        EXPECT_GE(got.red(), 0);
        EXPECT_LE(got.red(), 255);
        EXPECT_GE(got.green(), 0);
        EXPECT_LE(got.green(), 255);
        EXPECT_GE(got.blue(), 0);
        EXPECT_LE(got.blue(), 255);
    }
}

TEST(ColorSliderGetColor, PartialSaturationCombos)
{
    ColorSlider slider;
    struct Case { qreal h, s, v; int r, g, b; };
    const Case cases[] = {
        {0,   0.5, 1.0, 127, 255, 255},  // H0
        {180, 0.5, 0.5, 127, 63, 63},    // H3
        {60,  1.0, 0.5, 0, 0, 127},      // H1
    };
    for (const auto &c : cases) {
        QColor got = slider.getColor(c.h, c.s, c.v);
        EXPECT_EQ(got.red(), c.r) << "h=" << c.h;
        EXPECT_EQ(got.green(), c.g) << "h=" << c.h;
        EXPECT_EQ(got.blue(), c.b) << "h=" << c.h;
    }
}
