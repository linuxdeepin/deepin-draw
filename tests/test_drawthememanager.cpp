// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for DrawThemeManager (issue V-6403).
 *
 * Source: src/widgets/drawthememanager.cpp
 *
 * Target methods:
 *   instance(), m_currentTheme default, themeChanged signal
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QSignalSpy>
#include <QString>

#define protected public
#define private public
#include "widgets/drawthememanager.h"
#undef protected
#undef private

// ---- instance() singleton ----

TEST(DrawThemeManagerTest, InstanceReturnsNonNull)
{
    DrawThemeManager *tm = DrawThemeManager::instance();
    EXPECT_NE(tm, nullptr);
}

TEST(DrawThemeManagerTest, InstanceReturnsSamePointer)
{
    DrawThemeManager *first = DrawThemeManager::instance();
    DrawThemeManager *second = DrawThemeManager::instance();
    EXPECT_EQ(first, second);
}

// ---- default theme ----

TEST(DrawThemeManagerTest, DefaultThemeIsLight)
{
    DrawThemeManager *tm = DrawThemeManager::instance();
    EXPECT_EQ(tm->m_currentTheme, QString("light"));
}

// ---- themeChanged signal ----

TEST(DrawThemeManagerTest, ThemeChangedSignalConnectionIsValid)
{
    DrawThemeManager *tm = DrawThemeManager::instance();
    QSignalSpy spy(tm, &DrawThemeManager::themeChanged);
    EXPECT_TRUE(spy.isValid());
}

TEST(DrawThemeManagerTest, ThemeChangedSignalCountInitiallyZero)
{
    DrawThemeManager *tm = DrawThemeManager::instance();
    QSignalSpy spy(tm, &DrawThemeManager::themeChanged);
    EXPECT_EQ(spy.count(), 0);
}
