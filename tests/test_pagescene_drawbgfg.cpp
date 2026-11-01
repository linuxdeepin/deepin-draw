// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for PageScene drawBackground and drawForeground
 * (issue V-7262)
 *
 * Source: src/drawshape/cdrawscene.cpp
 *
 * Target methods:
 *   void drawBackground(QPainter *painter, const QRectF &rect) override
 *   void drawForeground(QPainter *painter, const QRectF &rect) override
 *
 * drawBackground: calls ensureAttribution(), QGraphicsScene::drawBackground(),
 *                 then fills sceneRect with bgColor().
 * drawForeground: checks bDrawForeground flag, calls pTool->drawMore() if
 *                 tool exists, then QGraphicsScene::drawForeground().
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QImage>
#include <QPainter>
#include <QColor>
#include <QRectF>
#include <memory>

#define protected public
#define private public
#include "drawshape/cdrawscene.h"
#include "drawshape/drawItems/bzItems/cgraphicsitem.h"
#include "drawshape/drawItems/bzItems/cgraphicsrectitem.h"
#include "drawshape/globaldefine.h"
#include "publicApi.h"
#undef protected
#undef private

// =========================================================================
// drawBackground
// =========================================================================

TEST(PageSceneDrawBgFgTest, DrawBackground_EmptyScene_DoesNotCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    QImage image(400, 300, QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    QPainter painter(&image);

    scene->drawBackground(&painter, QRectF(0, 0, 400, 300));
    painter.end();

    EXPECT_FALSE(painter.isActive());
}

TEST(PageSceneDrawBgFgTest, DrawBackground_FillsWithBgColor)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    // Set a known background color
    QColor testColor(100, 150, 200);
    scene->setBgColor(testColor);

    // Set scene rect to match our image
    scene->setSceneRect(0, 0, 200, 200);

    QImage image(200, 200, QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    QPainter painter(&image);

    scene->drawBackground(&painter, QRectF(0, 0, 200, 200));
    painter.end();
    EXPECT_FALSE(painter.isActive());

    // The background should be filled with bgColor
    // Check a pixel in the center of the scene rect
    QRgb pixel = image.pixel(100, 100);
    EXPECT_EQ(qRed(pixel), 100);
    EXPECT_EQ(qGreen(pixel), 150);
    EXPECT_EQ(qBlue(pixel), 200);
}

TEST(PageSceneDrawBgFgTest, DrawBackground_WithItems_DoesNotCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    // Add an item to the scene; the scene takes ownership.
    auto rect = std::make_unique<CGraphicsRectItem>(10, 10, 80, 80, nullptr);
    scene->addItem(rect.release());

    QImage image(200, 200, QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    QPainter painter(&image);

    scene->drawBackground(&painter, QRectF(0, 0, 200, 200));
    painter.end();

    EXPECT_FALSE(painter.isActive());
}

// =========================================================================
// drawForeground
// =========================================================================

TEST(PageSceneDrawBgFgTest, DrawForeground_EmptyScene_DoesNotCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    QImage image(400, 300, QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    QPainter painter(&image);

    scene->drawForeground(&painter, QRectF(0, 0, 400, 300));
    painter.end();

    EXPECT_FALSE(painter.isActive());
}

TEST(PageSceneDrawBgFgTest, DrawForeground_WithBDrawForegroundTrue_DoesNotCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    scene->bDrawForeground = true;

    QImage image(200, 200, QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    QPainter painter(&image);

    scene->drawForeground(&painter, QRectF(0, 0, 200, 200));
    painter.end();

    EXPECT_FALSE(painter.isActive());
}

TEST(PageSceneDrawBgFgTest, DrawForeground_WithBDrawForegroundFalse_DoesNotCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    scene->bDrawForeground = false;

    QImage image(200, 200, QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    QPainter painter(&image);

    scene->drawForeground(&painter, QRectF(0, 0, 200, 200));
    painter.end();

    EXPECT_FALSE(painter.isActive());
}

TEST(PageSceneDrawBgFgTest, DrawForeground_WithItems_DoesNotCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    auto rect = std::make_unique<CGraphicsRectItem>(10, 10, 80, 80, nullptr);
    scene->addItem(rect.release());

    QImage image(200, 200, QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    QPainter painter(&image);

    scene->drawForeground(&painter, QRectF(0, 0, 200, 200));
    painter.end();

    EXPECT_FALSE(painter.isActive());
}

// =========================================================================
// Combined drawBackground + drawForeground
// =========================================================================

TEST(PageSceneDrawBgFgTest, DrawBackgroundThenForeground_DoesNotCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    scene->setBgColor(QColor(255, 255, 255));
    scene->setSceneRect(0, 0, 200, 200);

    auto rect = std::make_unique<CGraphicsRectItem>(10, 10, 80, 80, nullptr);
    scene->addItem(rect.release());

    QImage image(200, 200, QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    QPainter painter(&image);

    scene->drawBackground(&painter, QRectF(0, 0, 200, 200));
    scene->drawForeground(&painter, QRectF(0, 0, 200, 200));

    painter.end();
    EXPECT_FALSE(painter.isActive());

    // Background should be white
    QRgb pixel = image.pixel(100, 100);
    EXPECT_EQ(qAlpha(pixel), 255);
}
