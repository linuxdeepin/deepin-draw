// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QImage>
#include <QIcon>
#include <QPainter>
#include <QPixmap>
#include <QSize>
#include <QString>
#include <QStyleOptionButton>

#define protected public
#define private public
#include "toolbutton.h"

/*
 * ToolButton extends QPushButton with two visual styles:
 *   - BUTTON_STYLE: a plain push button (optionally hiding its text)
 *   - MENU_STYLE  : a custom-drawn button rendering an icon + text
 *
 * sizeHint() and paintEvent() branch on m_style / m_bShowText. All of the
 * branches are reachable through the public API (ctor style argument, the
 * setBtnStyle/setShowText/setText/setIcon setters, sizeHint() and the inherited
 * render()), so no private-member access is required here. paintEvent() is
 * exercised by rendering the widget onto an offscreen QImage (which issues a
 * paint event), keeping the tests headless-safe.
 */

// ---- sizeHint() ----

// BUTTON_STYLE + showText: delegates to QPushButton::sizeHint() and widens by 9px.
TEST(ToolButtonSizeHintTest, ButtonStyleWithShowTextReturnsEnlargedWidth)
{
    ToolButton btn(nullptr, BUTTON_STYLE);
    btn.setShowText(true);
    btn.setText(QStringLiteral("Hello"));

    // Qualified base-class call bypasses the virtual override: expected = QPushButton::sizeHint() + 9px width.
    const QSize base = btn.QPushButton::sizeHint();
    const QSize sz = btn.sizeHint();
    EXPECT_EQ(sz.width(), base.width() + 9);
    EXPECT_EQ(sz.height(), base.height());
}

// BUTTON_STYLE + !showText, no icon, no menu: the size collapses to (0, 0).
TEST(ToolButtonSizeHintTest, ButtonStyleWithoutShowTextIsIconBased)
{
    ToolButton btn(nullptr, BUTTON_STYLE);
    btn.setShowText(false);

    const QSize sz = btn.sizeHint();
    EXPECT_EQ(sz.width(), 0);
    EXPECT_EQ(sz.height(), 0);
}

// MENU_STYLE: derives a non-trivial size from the (placeholder) text metrics.
TEST(ToolButtonSizeHintTest, MenuStyleReturnsValidSize)
{
    ToolButton btn(nullptr, MENU_STYLE);
    btn.setShowText(false);
    btn.setText(QStringLiteral("Menu"));

    // Expected size as documented: with no icon and text cleared by
    // !m_bShowText, the placeholder "XXXX" text metrics feed
    // QStyle::sizeFromContents(CT_PushButton).
    QStyleOptionButton opt;
    btn.initStyleOption(&opt);
    opt.text = "";
    const QSize textSize = btn.fontMetrics().size(Qt::TextShowMnemonic, QStringLiteral("XXXX"));
    opt.rect.setSize(textSize);
    const QSize expected = btn.style()->sizeFromContents(QStyle::CT_PushButton, &opt, textSize, &btn);

    const QSize sz = btn.sizeHint();
    EXPECT_EQ(sz.width(), expected.width());
    EXPECT_EQ(sz.height(), expected.height());
}

// ---- paintEvent() (rendered onto an offscreen QImage, which issues a paint event) ----

namespace {
// Renders `w` onto a fresh QImage of its current size; returns true if the
// render completed (i.e. paintEvent() ran without crashing).
bool renderWithoutCrash(QWidget &w)
{
    if (w.size().isEmpty())
        w.resize(80, 32);
    QImage img(w.size(), QImage::Format_ARGB32);
    img.fill(Qt::transparent);
    QPainter p(&img);
    w.render(&p);
    return !img.isNull();
}
}  // namespace

TEST(ToolButtonPaintEventTest, ButtonStyleWithShowTextRendersWithoutCrash)
{
    ToolButton btn(nullptr, BUTTON_STYLE);
    btn.setShowText(true);
    btn.setText(QStringLiteral("Save"));
    btn.resize(80, 30);
    EXPECT_TRUE(renderWithoutCrash(btn));
}

TEST(ToolButtonPaintEventTest, ButtonStyleWithoutShowTextRendersWithoutCrash)
{
    ToolButton btn(nullptr, BUTTON_STYLE);
    btn.setShowText(false);
    btn.resize(40, 40);
    EXPECT_TRUE(renderWithoutCrash(btn));
}

TEST(ToolButtonPaintEventTest, MenuStyleRendersIconAndTextWithoutCrash)
{
    ToolButton btn(nullptr, MENU_STYLE);
    btn.setShowText(false);
    btn.setText(QStringLiteral("Tool"));
    QPixmap iconPix(16, 16);
    iconPix.fill(Qt::red);
    btn.setIcon(QIcon(iconPix));
    btn.resize(120, 40);
    EXPECT_TRUE(renderWithoutCrash(btn));
}
