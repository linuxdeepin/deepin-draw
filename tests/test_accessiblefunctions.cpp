// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for getIntelAccessibleName() (accessiblefunctions.h).
 *
 * Target function (from issue V-5568):
 *   - getIntelAccessibleName(QWidget *w, QAccessible::Role r,
 *                           QString fallback)
 *
 * The function generates a unique accessible name for a widget.
 * It is guarded by #ifdef ENABLE_ACCESSIBILITY, so we must define
 * that macro before including the header.
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QApplication>
#include <QWidget>
#include <QAccessible>
#include <memory>

#define ENABLE_ACCESSIBILITY
#include "utils/accessiblefunctions.h"

TEST(GetIntelAccessibleNameTest, NonEmptyFallback_ReturnsFallback)
{
    QWidget w;
    QString result = getIntelAccessibleName(&w, QAccessible::Button,
                                            "myButton");
    EXPECT_FALSE(result.isEmpty());
    EXPECT_TRUE(result.contains("myButton"));
}

TEST(GetIntelAccessibleNameTest, EmptyFallback_UsesAccessibleName)
{
    QWidget w;
    w.setAccessibleName("customName");
    QString result = getIntelAccessibleName(&w, QAccessible::Button,
                                            "");
    EXPECT_FALSE(result.isEmpty());
    EXPECT_TRUE(result.contains("customName"));
}

TEST(GetIntelAccessibleNameTest, SameWidget_ReturnsCachedName)
{
    QWidget w;
    QString first = getIntelAccessibleName(&w, QAccessible::Button,
                                           "cacheTest");
    QString second = getIntelAccessibleName(&w, QAccessible::Button,
                                            "cacheTest");
    // Second call should return cached name (same value)
    EXPECT_EQ(first.toStdString(), second.toStdString());
}

TEST(GetIntelAccessibleNameTest, NonEmptyResult_AlwaysReturned)
{
    QWidget w1;
    QWidget w2;
    QWidget w3;

    QString r1 = getIntelAccessibleName(&w1, QAccessible::Button,
                                        "btn");
    QString r2 = getIntelAccessibleName(&w2, QAccessible::Button,
                                        "btn");
    QString r3 = getIntelAccessibleName(&w3, QAccessible::Button,
                                        "btn");

    // All results should be non-empty
    EXPECT_FALSE(r1.isEmpty());
    EXPECT_FALSE(r2.isEmpty());
    EXPECT_FALSE(r3.isEmpty());
}

TEST(GetIntelAccessibleNameTest, StaticTextRole_ReturnsNonEmpty)
{
    QWidget w;
    QString result = getIntelAccessibleName(&w, QAccessible::StaticText,
                                            "staticTextUnique");
    EXPECT_FALSE(result.isEmpty());
}
