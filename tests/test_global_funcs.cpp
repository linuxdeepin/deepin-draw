// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for Global and NSBlur methods (issue V-5575)
 *
 * Source: src/utils/global.cpp
 *
 * Target methods:
 *   Global::configPath, NSBlur::blurPixmap, NSBlur::blurImage
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QImage>
#include <QPixmap>
#include <QRect>
#include <QString>
#include <DStandardPaths>
DCORE_USE_NAMESPACE;

#define protected public
#define private public
#include "global.h"
#undef protected
#undef private

// =========================================================================
// Global::configPath
// =========================================================================

TEST(GlobalConfigPathTest, ReturnsAppConfigLocation)
{
    QString expected = DStandardPaths::writableLocation(
        QStandardPaths::AppConfigLocation);
    EXPECT_EQ(Global::configPath(), expected);
}

TEST(GlobalConfigPathTest, ReturnsNonEmpty)
{
    EXPECT_FALSE(Global::configPath().isEmpty());
}

TEST(GlobalConfigPathTest, ReturnsSameValueOnRepeatedCalls)
{
    QString first = Global::configPath();
    QString second = Global::configPath();
    EXPECT_EQ(first, second);
}

// =========================================================================
// NSBlur::blurPixmap
// =========================================================================

TEST(NSBlurBlurPixmapTest, NullPixmapReturnsNull)
{
    QPixmap nullPix;
    QPixmap result = NSBlur::blurPixmap(nullPix, 2, 0);
    EXPECT_TRUE(result.isNull());
}

TEST(NSBlurBlurPixmapTest, ValidPixmapTp0ReturnsSameSize)
{
    QPixmap pix(100, 100);
    pix.fill(Qt::red);
    QPixmap result = NSBlur::blurPixmap(pix, 2, 0);
    EXPECT_FALSE(result.isNull());
    EXPECT_EQ(result.width(), 100);
    EXPECT_EQ(result.height(), 100);
}

TEST(NSBlurBlurPixmapTest, ValidPixmapTp1ReturnsSameSize)
{
    QPixmap pix(100, 100);
    pix.fill(Qt::blue);
    QPixmap result = NSBlur::blurPixmap(pix, 2, 1);
    EXPECT_FALSE(result.isNull());
    EXPECT_EQ(result.width(), 100);
    EXPECT_EQ(result.height(), 100);
}

TEST(NSBlurBlurPixmapTest, ClipRectCropsPixmap)
{
    QPixmap pix(100, 100);
    pix.fill(Qt::green);
    QRect clipRect(10, 10, 50, 50);
    QPixmap result = NSBlur::blurPixmap(pix, 2, 0, clipRect);
    EXPECT_FALSE(result.isNull());
    EXPECT_EQ(result.width(), 50);
    EXPECT_EQ(result.height(), 50);
}

TEST(NSBlurBlurPixmapTest, InvalidClipRectIgnored)
{
    QPixmap pix(100, 100);
    pix.fill(Qt::yellow);
    QRect invalidRect;
    QPixmap result = NSBlur::blurPixmap(pix, 2, 0, invalidRect);
    EXPECT_FALSE(result.isNull());
    EXPECT_EQ(result.width(), 100);
    EXPECT_EQ(result.height(), 100);
}

// =========================================================================
// NSBlur::blurImage
// =========================================================================

TEST(NSBlurBlurImageTest, NullImageReturnsNull)
{
    QImage nullImg;
    QImage result = NSBlur::blurImage(nullImg, 2, 0);
    EXPECT_TRUE(result.isNull());
}

TEST(NSBlurBlurImageTest, ValidImageTp0ReturnsSameSize)
{
    QImage img(100, 100, QImage::Format_ARGB32);
    img.fill(Qt::red);
    QImage result = NSBlur::blurImage(img, 2, 0);
    EXPECT_FALSE(result.isNull());
    EXPECT_EQ(result.width(), 100);
    EXPECT_EQ(result.height(), 100);
}

TEST(NSBlurBlurImageTest, ValidImageTp1ReturnsSameSize)
{
    QImage img(100, 100, QImage::Format_ARGB32);
    img.fill(Qt::blue);
    QImage result = NSBlur::blurImage(img, 2, 1);
    EXPECT_FALSE(result.isNull());
    EXPECT_EQ(result.width(), 100);
    EXPECT_EQ(result.height(), 100);
}

TEST(NSBlurBlurImageTest, ClipRectCropsImage)
{
    QImage img(100, 100, QImage::Format_ARGB32);
    img.fill(Qt::green);
    QRect clipRect(10, 10, 50, 50);
    QImage result = NSBlur::blurImage(img, 2, 0, clipRect);
    EXPECT_FALSE(result.isNull());
    EXPECT_EQ(result.width(), 50);
    EXPECT_EQ(result.height(), 50);
}
