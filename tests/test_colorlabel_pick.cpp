// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for ColorLabel methods (issue V-7429)
 *
 * Source: src/widgets/colorlabel.cpp
 *
 * Target methods:
 *   pickColor(QPoint pos, bool picked) — emits pickedColor or signalPreViewColor
 *   getColor(qreal h, qreal s, qreal v) — HSV → RGB conversion
 *
 * Note: getColor() uses a non-standard HSV-to-RGB channel mapping:
 *   hi=0: (p, q, v)  hi=1: (t, p, v)  hi=2: (v, p, q)
 *   hi=3: (v, t, p)  hi=4: (q, v, p)  hi=5: (p, v, t)
 * Expected values below are computed from the actual implementation.
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QSignalSpy>
#include <QColor>
#include <QPoint>
#include <QtTest>

#define protected public
#define private public
#include "colorlabel.h"
#undef protected
#undef private

// =========================================================================
// ColorLabel::getColor (HSV → RGB conversion)
// =========================================================================

TEST(ColorLabelGetColor, RedHueReturnsRed)
{
    ColorLabel label;
    // h=0, s=1, v=1 → hi=0, p=0, q=1, t=0 → QColor(p, q, v) = (0, 255, 255)
    QColor result = label.getColor(0.0, 1.0, 1.0);
    EXPECT_EQ(result, QColor(0, 255, 255));
}

TEST(ColorLabelGetColor, FullSaturationValueAtHue60)
{
    ColorLabel label;
    // h=60, s=1, v=1 → hi=1, f=0, p=0, q=1, t=0 → QColor(t, p, v) = (0, 0, 255)
    QColor result = label.getColor(60.0, 1.0, 1.0);
    EXPECT_EQ(result.red(), 0);
    EXPECT_EQ(result.green(), 0);
    EXPECT_EQ(result.blue(), 255);
}

TEST(ColorLabelGetColor, ZeroSaturationReturnsGray)
{
    ColorLabel label;
    // s=0 → p=q=t=v, so all channels equal v
    QColor result = label.getColor(0.0, 0.0, 1.0);
    int r = result.red(), g = result.green(), b = result.blue();
    EXPECT_EQ(r, g);
    EXPECT_EQ(g, b);
}

TEST(ColorLabelGetColor, ZeroValueReturnsBlack)
{
    ColorLabel label;
    QColor result = label.getColor(120.0, 1.0, 0.0);
    EXPECT_EQ(result, QColor(0, 0, 0));
}

TEST(ColorLabelGetColor, Hue120ReturnsExpectedColor)
{
    ColorLabel label;
    // h=120, s=1, v=1 → hi=2, f=0, p=0, q=1, t=0 → QColor(v, p, q) = (255, 0, 255)
    QColor result = label.getColor(120.0, 1.0, 1.0);
    EXPECT_EQ(result.red(), 255);
    EXPECT_EQ(result.green(), 0);
    EXPECT_EQ(result.blue(), 255);
}

TEST(ColorLabelGetColor, Hue180ReturnsExpectedColor)
{
    ColorLabel label;
    // h=180, s=1, v=1 → hi=3, f=0, p=0, q=1, t=0 → QColor(v, t, p) = (255, 0, 0)
    QColor result = label.getColor(180.0, 1.0, 1.0);
    EXPECT_EQ(result.red(), 255);
    EXPECT_EQ(result.green(), 0);
    EXPECT_EQ(result.blue(), 0);
}

TEST(ColorLabelGetColor, Hue240ReturnsExpectedColor)
{
    ColorLabel label;
    // h=240, s=1, v=1 → hi=4, f=0, p=0, q=1, t=0 → QColor(q, v, p) = (255, 255, 0)
    QColor result = label.getColor(240.0, 1.0, 1.0);
    EXPECT_EQ(result.red(), 255);
    EXPECT_EQ(result.green(), 255);
    EXPECT_EQ(result.blue(), 0);
}

TEST(ColorLabelGetColor, Hue300ReturnsExpectedColor)
{
    ColorLabel label;
    // h=300, s=1, v=1 → hi=5, f=0, p=0, q=1, t=0 → QColor(p, v, t) = (0, 255, 0)
    QColor result = label.getColor(300.0, 1.0, 1.0);
    EXPECT_EQ(result.red(), 0);
    EXPECT_EQ(result.green(), 255);
    EXPECT_EQ(result.blue(), 0);
}

// =========================================================================
// ColorLabel::pickColor
// =========================================================================

TEST(ColorLabelPickColor, InvalidNegativePositionDoesNotCrash)
{
    ColorLabel label;
    label.resize(100, 100);
    label.show();
    QTest::qWait(50);

    QSignalSpy pickedSpy(&label, &ColorLabel::pickedColor);
    QSignalSpy previewSpy(&label, &ColorLabel::signalPreViewColor);

    label.pickColor(QPoint(-1, -1), true);

    EXPECT_EQ(pickedSpy.count(), 0);
    EXPECT_EQ(previewSpy.count(), 0);
}

TEST(ColorLabelPickColor, OutOfBoundsPositionDoesNotCrash)
{
    ColorLabel label;
    label.resize(100, 100);
    label.show();
    QTest::qWait(50);

    QSignalSpy pickedSpy(&label, &ColorLabel::pickedColor);

    label.pickColor(QPoint(200, 200), true);

    EXPECT_EQ(pickedSpy.count(), 0);
}

TEST(ColorLabelPickColor, PickedTrueEmitsPickedColor)
{
    ColorLabel label;
    label.resize(100, 100);
    label.show();
    QTest::qWait(50);

    QSignalSpy spy(&label, &ColorLabel::pickedColor);
    label.pickColor(QPoint(50, 50), true);

    EXPECT_EQ(spy.count(), 1);
    QList<QVariant> args = spy.takeFirst();
    EXPECT_TRUE(args.at(0).canConvert<QColor>());
}

TEST(ColorLabelPickColor, PickedFalseEmitsPreViewColor)
{
    ColorLabel label;
    label.resize(100, 100);
    label.show();
    QTest::qWait(50);

    QSignalSpy spy(&label, &ColorLabel::signalPreViewColor);
    label.pickColor(QPoint(50, 50), false);

    EXPECT_EQ(spy.count(), 1);
    QList<QVariant> args = spy.takeFirst();
    EXPECT_TRUE(args.at(0).canConvert<QColor>());
}

TEST(ColorLabelPickColor, SetsM_pickedColor)
{
    ColorLabel label;
    label.resize(100, 100);
    label.show();
    QTest::qWait(50);

    label.pickColor(QPoint(10, 10), true);
    // m_pickedColor should have been set to some valid color
    EXPECT_TRUE(label.m_pickedColor.isValid());
}
