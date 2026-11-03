// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for ToolButton::setAlignment (issue V-7429)
 *
 * Source: src/widgets/toolbutton.cpp
 *
 * Target method:
 *   setAlignment(Qt::Alignment alignment) — sets m_alignment
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <Qt>

#define protected public
#define private public
#include "toolbutton.h"
#undef protected
#undef private

TEST(ToolButtonSetAlignment, AlignLeftStoredCorrectly)
{
    ToolButton btn(nullptr, BUTTON_STYLE);
    btn.setAlignment(Qt::AlignLeft);
    EXPECT_EQ(btn.m_alignment, Qt::AlignLeft);
}

TEST(ToolButtonSetAlignment, AlignRightStoredCorrectly)
{
    ToolButton btn(nullptr, BUTTON_STYLE);
    btn.setAlignment(Qt::AlignRight);
    EXPECT_EQ(btn.m_alignment, Qt::AlignRight);
}

TEST(ToolButtonSetAlignment, AlignCenterStoredCorrectly)
{
    ToolButton btn(nullptr, BUTTON_STYLE);
    btn.setAlignment(Qt::AlignCenter);
    EXPECT_EQ(btn.m_alignment, Qt::AlignCenter);
}

TEST(ToolButtonSetAlignment, DefaultIsAlignLeft)
{
    ToolButton btn(nullptr, BUTTON_STYLE);
    // Default alignment should be AlignLeft per the default parameter.
    EXPECT_EQ(btn.m_alignment, Qt::AlignLeft);
}

TEST(ToolButtonSetAlignment, MultipleChangesOverwrite)
{
    ToolButton btn(nullptr, BUTTON_STYLE);
    btn.setAlignment(Qt::AlignLeft);
    btn.setAlignment(Qt::AlignRight);
    btn.setAlignment(Qt::AlignCenter);
    EXPECT_EQ(btn.m_alignment, Qt::AlignCenter);
}
