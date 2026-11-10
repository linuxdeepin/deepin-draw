// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for PickColorWidget color accessors and theme
 * (src/frame/AttributesWidgets/private/pickcolorwidget.cpp).
 *
 * Covers (issue V-8025 §4):
 *   - color() — return current color (curColor)
 *   - setColor() — set color, emit colorChanged signal
 *   - setTheme() — delegate to picker theme
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QColor>
#include <QSignalSpy>

#define protected public
#define private public
#include "pickcolorwidget.h"
#include "publicApi.h"
#undef protected
#undef private

// ===========================================================================
// PickColorWidget::color — return current color
// ===========================================================================

TEST(PickColorWidgetColor, ReturnsColorAfterConstruction)
{
    PickColorWidget w(nullptr);

    // After construction, color() should return a QColor (may be invalid
    // if no default color is set — that is acceptable).
    QColor c;
    EXPECT_NO_FATAL_FAILURE(c = w.color());
    EXPECT_EQ(c, w.color());
}

TEST(PickColorWidgetColor, ReturnsColorSetBySetColor)
{
    PickColorWidget w(nullptr);

    QColor testColor(100, 200, 50);
    w.setColor(testColor, false);

    // After setColor, color() should return the color we set.
    QColor result = w.color();
    EXPECT_EQ(result, testColor);
}

TEST(PickColorWidgetColor, ReflectsLatestSetColor)
{
    PickColorWidget w(nullptr);

    w.setColor(QColor(10, 20, 30), false);
    EXPECT_EQ(w.color(), QColor(10, 20, 30));

    w.setColor(QColor(200, 100, 50), false);
    EXPECT_EQ(w.color(), QColor(200, 100, 50));
}

// ===========================================================================
// PickColorWidget::setColor — set color and emit colorChanged signal
// ===========================================================================

TEST(PickColorWidgetSetColor, EmitsColorChanged_WhenInternalChanged)
{
    PickColorWidget w(nullptr);

    QSignalSpy spy(&w, &PickColorWidget::colorChanged);
    ASSERT_TRUE(spy.isValid());

    // setColor with internalChanged=true should emit colorChanged.
    QColor newColor(255, 0, 128);
    w.setColor(newColor, true);

    // Exactly one colorChanged emission is expected for one actual change.
    ASSERT_EQ(spy.count(), 1);

    // Verify the emitted color matches.
    QList<QVariant> args = spy.takeFirst();
    EXPECT_EQ(args.at(0).value<QColor>(), newColor);
}

TEST(PickColorWidgetSetColor, DoesNotEmitColorChanged_WhenNotInternalChanged)
{
    PickColorWidget w(nullptr);

    QSignalSpy spy(&w, &PickColorWidget::colorChanged);
    ASSERT_TRUE(spy.isValid());

    // setColor with internalChanged=false should NOT emit colorChanged.
    QColor newColor(0, 255, 0);
    w.setColor(newColor, false);

    EXPECT_EQ(spy.count(), 0);
}

TEST(PickColorWidgetSetColor, SameColor_NoSignal)
{
    PickColorWidget w(nullptr);

    // First set a color.
    QColor c(50, 100, 150);
    w.setColor(c, false);

    QSignalSpy spy(&w, &PickColorWidget::colorChanged);
    ASSERT_TRUE(spy.isValid());

    // Set the same color again — should not emit colorChanged.
    w.setColor(c, true);
    EXPECT_EQ(spy.count(), 0);
}

TEST(PickColorWidgetSetColor, DifferentColors_EmitsEachTime)
{
    PickColorWidget w(nullptr);

    QSignalSpy spy(&w, &PickColorWidget::colorChanged);
    ASSERT_TRUE(spy.isValid());

    w.setColor(QColor(10, 10, 10), true);
    w.setColor(QColor(20, 20, 20), true);
    w.setColor(QColor(30, 30, 30), true);

    // Three actual changes must emit exactly three signals.
    EXPECT_EQ(spy.count(), 3);
}

TEST(PickColorWidgetSetColor, InvalidColor_NoCrash)
{
    PickColorWidget w(nullptr);

    // Setting an invalid color should not crash.
    w.setColor(QColor(), true);
    SUCCEED();
}

// ===========================================================================
// PickColorWidget::setTheme — delegate to picker theme
// ===========================================================================

TEST(PickColorWidgetSetTheme, LightTheme_NoCrash)
{
    PickColorWidget w(nullptr);

    // Theme 0 typically represents light theme.
    w.setTheme(0);
    SUCCEED();
}

TEST(PickColorWidgetSetTheme, DarkTheme_NoCrash)
{
    PickColorWidget w(nullptr);

    // Theme 1 typically represents dark theme.
    w.setTheme(1);
    SUCCEED();
}

TEST(PickColorWidgetSetTheme, MultipleThemeSwitches_NoCrash)
{
    PickColorWidget w(nullptr);

    w.setTheme(0);
    w.setTheme(1);
    w.setTheme(0);
    w.setTheme(1);

    SUCCEED();
}

TEST(PickColorWidgetSetTheme, PickerNotNullAfterConstruction)
{
    PickColorWidget w(nullptr);

    // setTheme delegates to m_picker->setTheme(), so m_picker must exist.
    EXPECT_NE(w.m_picker, nullptr);
}
