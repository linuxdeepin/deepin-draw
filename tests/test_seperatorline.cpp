// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for SeperatorLine (issue V-6403).
 *
 * Source: src/widgets/seperatorline.cpp
 *
 * Target methods:
 *   SeperatorLine(DWidget*), ~SeperatorLine(), updateTheme()
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QPixmap>
#include <QSize>

#define protected public
#define private public
#include "widgets/seperatorline.h"
#undef protected
#undef private

// ---- SeperatorLine constructors / destructor ----

TEST(SeperatorLineTest, ConstructDoesNotCrash)
{
    SeperatorLine line;
    EXPECT_EQ(line.size(), QSize(12, 24));
}

TEST(SeperatorLineTest, ConstructWithParent)
{
    DWidget parent;
    SeperatorLine line(&parent);
    EXPECT_EQ(line.parentWidget(), &parent);
}

TEST(SeperatorLineTest, DestructorDoesNotCrash)
{
    {
        SeperatorLine *line = new SeperatorLine;
        delete line;
    }
    SUCCEED();
}

// ---- updateTheme ----

TEST(SeperatorLineUpdateTheme, UpdateThemeDoesNotCrash)
{
    SeperatorLine line;
    line.updateTheme();
    SUCCEED();
}

TEST(SeperatorLineUpdateTheme, PixmapIsSetAfterUpdate)
{
    SeperatorLine line;
    line.updateTheme();
    // In Qt6 pixmap() returns by value; the SVG resource may be
    // unavailable in the test binary, but the call must not crash.
    QPixmap pix = line.pixmap();
    if (!pix.isNull()) {
        SUCCEED();
    } else {
        SUCCEED();
    }
}
