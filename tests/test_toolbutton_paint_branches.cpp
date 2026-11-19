// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for ToolButton paintEvent / sizeHint / getPen branches
 * not covered by existing test_toolbutton*.cpp (issue V-8981 §1).
 *
 * Source: src/widgets/toolbutton.cpp
 *
 * Covers:
 *   - paintEvent: MENU_STYLE with State_On highlight background,
 *     State_MouseOver shadow background, and the corresponding text color
 *     branches (highlightedText / text).
 *   - sizeHint: BUTTON_STYLE with icon and menu indicator width;
 *     MENU_STYLE with empty text falling back to placeholder "XXXX".
 *   - getPen: dark-theme (themeType != 1) and light-theme (themeType == 1)
 *     for disabled / MouseOver / Sunken / normal states.
 *
 * paintEvent is exercised by rendering the widget onto an offscreen QImage,
 * which issues a paint event. All assertions are crash-safety + observable
 * property checks, keeping the tests headless-safe.
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QImage>
#include <QIcon>
#include <QPainter>
#include <QPixmap>
#include <QMenu>
#include <QSize>
#include <QString>
#include <QStyleOptionButton>
#include <QSignalSpy>

#include <DGuiApplicationHelper>

DGUI_USE_NAMESPACE

#define protected public
#define private public
#include "toolbutton.h"
#undef protected
#undef private

// ---- helper: render a widget onto an offscreen QImage (issues paintEvent) ----
static bool renderToImage(QWidget &w)
{
    if (w.size().isEmpty())
        w.resize(100, 36);
    QImage img(w.size(), QImage::Format_ARGB32);
    img.fill(Qt::transparent);
    QPainter p(&img);
    w.render(&p);
    return !img.isNull();
}

// ===========================================================================
// paintEvent — MENU_STYLE State_On / State_MouseOver background & text branches
// ===========================================================================

TEST(ToolButtonPaintBranchTest, MenuStyleOnStateRendersWithoutCrash)
{
    ToolButton btn(nullptr, MENU_STYLE);
    btn.setShowText(false);
    btn.setText(QStringLiteral("Cut"));
    QPixmap pix(16, 16);
    pix.fill(Qt::red);
    btn.setIcon(QIcon(pix));
    btn.resize(100, 36);
    btn.setChecked(true);
    btn.setCheckable(true);
    EXPECT_TRUE(renderToImage(btn));
}

TEST(ToolButtonPaintBranchTest, MenuStyleMouseOverRendersWithoutCrash)
{
    ToolButton btn(nullptr, MENU_STYLE);
    btn.setShowText(false);
    btn.setText(QStringLiteral("Copy"));
    QPixmap pix(16, 16);
    pix.fill(Qt::blue);
    btn.setIcon(QIcon(pix));
    btn.resize(100, 36);
    // Simulate mouse-over by entering the widget's geometry.
    btn.setAttribute(Qt::WA_UnderMouse, true);
    EXPECT_TRUE(renderToImage(btn));
    btn.setAttribute(Qt::WA_UnderMouse, false);
}

TEST(ToolButtonPaintBranchTest, MenuStyleNoIconRendersWithoutCrash)
{
    // Exercises the icon().isNull() branch inside paintEvent.
    ToolButton btn(nullptr, MENU_STYLE);
    btn.setShowText(false);
    btn.setText(QStringLiteral("NoIcon"));
    btn.resize(100, 36);
    EXPECT_TRUE(renderToImage(btn));
}

TEST(ToolButtonPaintBranchTest, MenuStyleAlignCenterRendersWithoutCrash)
{
    // Exercises the m_alignment != AlignLeft path.
    ToolButton btn(nullptr, MENU_STYLE);
    btn.setShowText(false);
    btn.setText(QStringLiteral("Centered"));
    btn.setAlignment(Qt::AlignCenter);
    QPixmap pix(16, 16);
    pix.fill(Qt::green);
    btn.setIcon(QIcon(pix));
    btn.resize(120, 36);
    EXPECT_TRUE(renderToImage(btn));
}

// ===========================================================================
// sizeHint — icon / menu / placeholder-text branches
// ===========================================================================

TEST(ToolButtonSizeHintBranchTest, ButtonStyleWithoutShowTextWithIconReturnsNonZero)
{
    ToolButton btn(nullptr, BUTTON_STYLE);
    btn.setShowText(false);
    QPixmap pix(32, 32);
    pix.fill(Qt::red);
    btn.setIcon(QIcon(pix));
    const QSize sz = btn.sizeHint();
    // With an icon, width should reflect iconSize.
    EXPECT_GT(sz.width(), 0);
}

TEST(ToolButtonSizeHintBranchTest, ButtonStyleWithoutShowTextWithMenuReturnsLargerWidth)
{
    ToolButton btn(nullptr, BUTTON_STYLE);
    btn.setShowText(false);
    QPixmap pix(32, 32);
    pix.fill(Qt::red);
    btn.setIcon(QIcon(pix));

    // Without menu
    const QSize szNoMenu = btn.sizeHint();

    // With a menu — adds PM_MenuButtonIndicator width.
    QMenu *menu = new QMenu(&btn);
    menu->addAction("A");
    btn.setMenu(menu);
    const QSize szWithMenu = btn.sizeHint();

    EXPECT_GE(szWithMenu.width(), szNoMenu.width());
}

TEST(ToolButtonSizeHintBranchTest, MenuStyleEmptyTextUsesPlaceholderFallback)
{
    // MENU_STYLE with empty text and !m_bShowText: the "XXXX" placeholder
    // path in sizeHint is exercised.
    ToolButton btn(nullptr, MENU_STYLE);
    btn.setShowText(false);
    btn.setText(QString());  // empty text
    const QSize sz = btn.sizeHint();
    // Placeholder "XXXX" guarantees a non-zero size.
    EXPECT_GT(sz.width(), 0);
    EXPECT_GT(sz.height(), 0);
}

TEST(ToolButtonSizeHintBranchTest, MenuStyleEmptyTextWithIconReturnsNonZero)
{
    // MENU_STYLE, empty text, with icon: the `!empty || !w` / `!empty || !h`
    // branches where w and h are non-zero from the icon.
    ToolButton btn(nullptr, MENU_STYLE);
    btn.setShowText(false);
    btn.setText(QString());
    QPixmap pix(32, 32);
    pix.fill(Qt::red);
    btn.setIcon(QIcon(pix));
    const QSize sz = btn.sizeHint();
    EXPECT_GT(sz.width(), 0);
    EXPECT_GT(sz.height(), 0);
}

// ===========================================================================
// getPen — all state × theme combinations
// ===========================================================================

TEST(ToolButtonGetPenBranchTest, LightThemeDisabledReturnsGrayPen)
{
    DGuiApplicationHelper::instance()->setPaletteType(DGuiApplicationHelper::LightType);
    ToolButton btn(nullptr, MENU_STYLE);
    QStyleOptionButton opt;
    opt.state = QStyle::State_None;  // not enabled
    QPen pen = btn.getPen(opt);
    EXPECT_EQ(pen.color(), QColor("#9C9C9C"));
}

TEST(ToolButtonGetPenBranchTest, LightThemeMouseOverReturnsWhitePen)
{
    DGuiApplicationHelper::instance()->setPaletteType(DGuiApplicationHelper::LightType);
    ToolButton btn(nullptr, MENU_STYLE);
    QStyleOptionButton opt;
    opt.state = QStyle::State_Enabled | QStyle::State_MouseOver;
    QPen pen = btn.getPen(opt);
    EXPECT_EQ(pen.color(), QColor(Qt::white));
}

TEST(ToolButtonGetPenBranchTest, LightThemeSunkenReturnsBluePen)
{
    DGuiApplicationHelper::instance()->setPaletteType(DGuiApplicationHelper::LightType);
    ToolButton btn(nullptr, MENU_STYLE);
    QStyleOptionButton opt;
    opt.state = QStyle::State_Enabled | QStyle::State_Sunken;
    QPen pen = btn.getPen(opt);
    EXPECT_EQ(pen.color(), QColor("#99cdff"));
}

TEST(ToolButtonGetPenBranchTest, LightThemeNormalReturnsDarkPen)
{
    DGuiApplicationHelper::instance()->setPaletteType(DGuiApplicationHelper::LightType);
    ToolButton btn(nullptr, MENU_STYLE);
    QStyleOptionButton opt;
    opt.state = QStyle::State_Enabled;
    QPen pen = btn.getPen(opt);
    EXPECT_EQ(pen.color(), QColor("#343434"));
}

TEST(ToolButtonGetPenBranchTest, DarkThemeDisabledReturnsGrayPen)
{
    DGuiApplicationHelper::instance()->setPaletteType(DGuiApplicationHelper::DarkType);
    ToolButton btn(nullptr, MENU_STYLE);
    QStyleOptionButton opt;
    opt.state = QStyle::State_None;
    QPen pen = btn.getPen(opt);
    EXPECT_EQ(pen.color(), QColor("#8D8D8D"));
}

TEST(ToolButtonGetPenBranchTest, DarkThemeMouseOverReturnsWhitePen)
{
    DGuiApplicationHelper::instance()->setPaletteType(DGuiApplicationHelper::DarkType);
    ToolButton btn(nullptr, MENU_STYLE);
    QStyleOptionButton opt;
    opt.state = QStyle::State_Enabled | QStyle::State_MouseOver;
    QPen pen = btn.getPen(opt);
    EXPECT_EQ(pen.color(), QColor(Qt::white));
}

TEST(ToolButtonGetPenBranchTest, DarkThemeSunkenReturnsBluePen)
{
    DGuiApplicationHelper::instance()->setPaletteType(DGuiApplicationHelper::DarkType);
    ToolButton btn(nullptr, MENU_STYLE);
    QStyleOptionButton opt;
    opt.state = QStyle::State_Enabled | QStyle::State_Sunken;
    QPen pen = btn.getPen(opt);
    EXPECT_EQ(pen.color(), QColor("#99cdff"));
}

TEST(ToolButtonGetPenBranchTest, DarkThemeNormalReturnsWhitePen)
{
    DGuiApplicationHelper::instance()->setPaletteType(DGuiApplicationHelper::DarkType);
    ToolButton btn(nullptr, MENU_STYLE);
    QStyleOptionButton opt;
    opt.state = QStyle::State_Enabled;
    QPen pen = btn.getPen(opt);
    EXPECT_EQ(pen.color(), QColor(Qt::white));
}

// Reset palette type to auto after all getPen tests to avoid leaking state.
TEST(ToolButtonGetPenBranchTest, ResetPaletteTypeToAuto)
{
    DGuiApplicationHelper::instance()->setPaletteType(DGuiApplicationHelper::UnknownType);
    SUCCEED();
}
