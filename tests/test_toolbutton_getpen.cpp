// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for ToolButton::getPen (toolbutton.cpp).
 *
 * Target function (from issue V-5568):
 *   - ToolButton::getPen(const QStyleOptionButton)
 *
 * getPen returns a QPen based on the option's state flags and the
 * current DTK theme type. The function is public.
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QStyleOptionButton>
#include <QPen>
#include <QColor>

#include <DGuiApplicationHelper>

DGUI_USE_NAMESPACE

#define protected public
#define private public
#include "toolbutton.h"
#undef protected
#undef private

TEST(ToolButtonGetPenTest, DisabledStateReturnsPen)
{
    ToolButton btn(nullptr, MENU_STYLE);
    QStyleOptionButton opt;
    opt.state = QStyle::State_None; // disabled (no State_Enabled)

    QPen pen = btn.getPen(opt);
    EXPECT_NE(pen.color(), QColor(Qt::transparent));
}

TEST(ToolButtonGetPenTest, MouseOverStateReturnsPen)
{
    ToolButton btn(nullptr, MENU_STYLE);
    QStyleOptionButton opt;
    opt.state = QStyle::State_Enabled | QStyle::State_MouseOver;

    QPen pen = btn.getPen(opt);
    EXPECT_NE(pen.color(), QColor(Qt::transparent));
}

TEST(ToolButtonGetPenTest, SunkenStateReturnsPen)
{
    ToolButton btn(nullptr, MENU_STYLE);
    QStyleOptionButton opt;
    opt.state = QStyle::State_Enabled | QStyle::State_Sunken;

    QPen pen = btn.getPen(opt);
    EXPECT_NE(pen.color(), QColor(Qt::transparent));
}

TEST(ToolButtonGetPenTest, NormalEnabledStateReturnsPen)
{
    ToolButton btn(nullptr, MENU_STYLE);
    QStyleOptionButton opt;
    opt.state = QStyle::State_Enabled;

    QPen pen = btn.getPen(opt);
    EXPECT_NE(pen.color(), QColor(Qt::transparent));
}

TEST(ToolButtonGetPenTest, LightThemeReturnsConsistentColor)
{
    ToolButton btn(nullptr, MENU_STYLE);

    // Force light theme (themeType == 1)
    DGuiApplicationHelper::instance()->setPaletteType(DGuiApplicationHelper::LightType);

    QStyleOptionButton opt;
    opt.state = QStyle::State_Enabled;

    QPen pen = btn.getPen(opt);
    // In light theme with enabled+normal state, the color should be "#343434"
    EXPECT_EQ(pen.color(), QColor("#343434"));
}
