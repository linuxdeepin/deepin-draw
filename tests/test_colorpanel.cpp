// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for ColorButton and ColorPanel (issue V-7155)
 *
 * Source: src/frame/AttributesWidgets/private/colorpanel.h/.cpp
 *
 * Branch map:
 *   B1  ColorButton constructor sets m_color, fixed size, checkable
 *   B2  ColorButton::color() returns the color passed to constructor
 *   B3  ColorButton::paintEvent — non-transparent color path (grab)
 *   B4  ColorButton::paintEvent — transparent (white alpha=0) path: diagonal line
 *   B5  ColorButton::paintEvent — checked state: blue border
 *   B6  ColorButton destructor (no crash)
 *   B7  ColorPanel constructor (initUI + initConnection)
 *   B8  ColorPanel::parentColorWidget() returns nullptr when parent isn't CColorPickWidget
 *   B9  ColorPanel::setColor(c, true, EChanged) → color() returns c
 *   B10 ColorPanel::setColor(c, true, EChanged) emits colorChanged signal
 *   B11 ColorPanel::setColor(c, true, EChangedBegin) does NOT set curColor
 *   B12 ColorPanel::setColor(c, false, EChanged) does NOT emit signal
 *   B13 ColorPanel m_colLineEdit is non-null after construction
 *   B14 ColorPanel::setExpandWidgetVisble(true/false) no crash
 *   B15 ColorPanel::setTheme(0) no crash
 *   B16 ColorPanel::setTheme(1) no crash
 *   B17 ColorPanel::showEvent via show() + grab()
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QSignalSpy>
#include <QColor>

#include "colorpanel.h"
#include "drawshape/globaldefine.h"

#if TEST_COLORPANEL_ITEM

// =========================================================================
// ColorButton
// =========================================================================

TEST(ColorButtonTest, Constructor_SetsColorFixedSizeCheckable)
{
    // B1: constructor sets the color, fixed size, checkable
    QColor c(Qt::red);
    ColorButton btn(c, nullptr);
    EXPECT_EQ(btn.color(), c);
    EXPECT_EQ(btn.size(), QSize(34, 34));
    EXPECT_TRUE(btn.isCheckable());
}

TEST(ColorButtonTest, Color_ReturnsConstructorColor)
{
    // B2: color() returns the color passed to constructor
    QColor c(100, 150, 200);
    ColorButton btn(c, nullptr);
    EXPECT_EQ(btn.color(), c);
}

TEST(ColorButtonTest, PaintEvent_NonTransparentColor_NoCrash)
{
    // B3: paintEvent — non-transparent color path
    ColorButton btn(QColor(50, 100, 150), nullptr);
    btn.resize(34, 34);
    QPixmap pix = btn.grab();
    EXPECT_FALSE(pix.isNull());
}

TEST(ColorButtonTest, PaintEvent_TransparentColor_NoCrash)
{
    // B4: paintEvent — transparent (white alpha=0) path: diagonal line
    QColor transparent(Qt::white);
    transparent.setAlpha(0);
    ColorButton btn(transparent, nullptr);
    btn.resize(34, 34);
    QPixmap pix = btn.grab();
    EXPECT_FALSE(pix.isNull());
}

TEST(ColorButtonTest, PaintEvent_CheckedState_NoCrash)
{
    // B5: paintEvent — checked state: blue border
    ColorButton btn(QColor(Qt::blue), nullptr);
    btn.setChecked(true);
    btn.resize(34, 34);
    QPixmap pix = btn.grab();
    EXPECT_FALSE(pix.isNull());
}

TEST(ColorButtonTest, Destructor_NoCrash)
{
    // B6: destructor
    {
        ColorButton btn(QColor(Qt::green), nullptr);
    }
    SUCCEED();
}

// =========================================================================
// ColorPanel
// =========================================================================

TEST(ColorPanelTest, Constructor_NoCrash)
{
    // B7: constructor (initUI + initConnection)
    ColorPanel panel(nullptr);
    SUCCEED();
}

TEST(ColorPanelTest, ParentColorWidget_ReturnsNullptr_WhenNotCColorPickWidget)
{
    // B8: parentColorWidget() returns nullptr when parent isn't CColorPickWidget
    ColorPanel panel(nullptr);
    EXPECT_EQ(panel.parentColorWidget(), nullptr);
}

TEST(ColorPanelTest, SetColor_EChanged_Roundtrip)
{
    // B9: setColor(c, true, EChanged) → color() returns c
    ColorPanel panel(nullptr);
    QColor c(Qt::red);
    panel.setColor(c, true, EChanged);
    EXPECT_EQ(panel.color(), c);
}

TEST(ColorPanelTest, SetColor_EChanged_EmitsColorChanged)
{
    // B10: setColor(c, true, EChanged) emits colorChanged signal
    ColorPanel panel(nullptr);
    QSignalSpy spy(&panel, &ColorPanel::colorChanged);
    QColor c(Qt::blue);
    panel.setColor(c, true, EChanged);
    EXPECT_EQ(spy.count(), 1);
    if (spy.count() > 0) {
        QList<QVariant> args = spy.takeFirst();
        EXPECT_EQ(args.at(0).value<QColor>(), c);
    }
}

TEST(ColorPanelTest, SetColor_EChangedBegin_DoesNotSetCurColor)
{
    // B11: setColor(c, true, EChangedBegin) does NOT set curColor
    // (phase != EChangedFinished && phase != EChanged), so color() stays at
    // the original value.
    ColorPanel panel(nullptr);
    QColor original = panel.color();
    QColor c(Qt::green);
    panel.setColor(c, true, EChangedBegin);
    EXPECT_EQ(panel.color(), original);
}

TEST(ColorPanelTest, SetColor_InternalChangedFalse_DoesNotEmit)
{
    // B12: setColor(c, false, EChanged) does NOT emit signal
    ColorPanel panel(nullptr);
    QSignalSpy spy(&panel, &ColorPanel::colorChanged);
    panel.setColor(QColor(Qt::yellow), false, EChanged);
    EXPECT_EQ(spy.count(), 0);
}

TEST(ColorPanelTest, ColLineEdit_IsNonNull)
{
    // B13: m_colLineEdit is non-null after construction
    // (initUI creates it as a child widget, so find it via the public Qt API)
    ColorPanel panel(nullptr);
    EXPECT_NE(panel.findChild<DLineEdit *>(), nullptr);
}

TEST(ColorPanelTest, SetExpandWidgetVisble_ToggleNoCrash)
{
    // B14: setExpandWidgetVisble(true/false) no crash
    ColorPanel panel(nullptr);
    panel.setExpandWidgetVisble(true);
    panel.setExpandWidgetVisble(false);
    panel.setExpandWidgetVisble(true);
    SUCCEED();
}

TEST(ColorPanelTest, SetTheme_0_NoCrash)
{
    // B15: setTheme(0)
    ColorPanel panel(nullptr);
    panel.setTheme(0);
    SUCCEED();
}

TEST(ColorPanelTest, SetTheme_1_NoCrash)
{
    // B16: setTheme(1)
    ColorPanel panel(nullptr);
    panel.setTheme(1);
    SUCCEED();
}

TEST(ColorPanelTest, ShowEvent_ViaShowAndGrab)
{
    // B17: showEvent via show() + grab()
    ColorPanel panel(nullptr);
    panel.resize(200, 300);
    panel.show();
    QPixmap pix = panel.grab();
    EXPECT_FALSE(pix.isNull());
    panel.hide();
}

#endif // TEST_COLORPANEL_ITEM
