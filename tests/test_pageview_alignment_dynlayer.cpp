// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for PageView alignment extension (issue V-9581 §1).
 *
 * Source: src/frame/cgraphicsview.cpp
 *
 * Target lines:
 *   1548-1550: alignmentMovPos default branch — pass Qt::AlignCenter
 *              (AlignHCenter|AlignVCenter, matches no case) → returns (0,0)
 *   1507: updateSelectedItemsAlignment JDynamicLayer branch — when a selected
 *         item is a JDynamicLayer, operatingEnd is called after operating
 *
 * Construction:
 *   - For default branch: call alignmentMovPos with Qt::AlignCenter, assert (0,0).
 *   - For JDynamicLayer branch: create a JDynamicLayer, add it to the scene
 *     alongside a rect item, select both, call updateSelectedItemsAlignment,
 *     assert position changes.
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QImage>
#include <QPainter>
#include <QPainterPath>

#define protected public
#define private public
#include "publicApi.h"
#include "drawshape/globaldefine.h"
#include "drawshape/drawItems/cgraphicslayer.h"
#include "drawshape/drawItems/bzItems/cgraphicsrectitem.h"
#include "drawshape/drawItems/bzItems/cgraphicsitem.h"
#undef protected
#undef private

// ---------------------------------------------------------------------------
// alignmentMovPos — default branch with Qt::AlignCenter
// ---------------------------------------------------------------------------

TEST(PageViewAlignmentExtTest, AlignCenter_DefaultBranch_ReturnsZero)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    QRectF father(0, 0, 200, 200);
    QRectF child(50, 50, 40, 40);

    // Qt::AlignCenter == AlignHCenter | AlignVCenter, which does not match
    // any individual case in the switch → default branch returns (0, 0).
    QPointF d = view->alignmentMovPos(father, child, Qt::AlignCenter);
    EXPECT_NEAR(d.x(), 0.0, 0.01);
    EXPECT_NEAR(d.y(), 0.0, 0.01);
}

TEST(PageViewAlignmentExtTest, AlignCenter_AlreadyCentered_ReturnsZero)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    QRectF father(0, 0, 100, 100);
    QRectF child(25, 25, 50, 50);

    QPointF d = view->alignmentMovPos(father, child, Qt::AlignCenter);
    EXPECT_NEAR(d.x(), 0.0, 0.01);
    EXPECT_NEAR(d.y(), 0.0, 0.01);
}

// ---------------------------------------------------------------------------
// updateSelectedItemsAlignment — JDynamicLayer branch (line 1507)
// ---------------------------------------------------------------------------

TEST(PageViewAlignmentExtTest, UpdateAlignment_WithDynamicLayer_ChangesPosition)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    // Create a rectangle item.
    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(100, 100), QPoint(200, 200), false);
    ASSERT_GE(scene->getBzItems().count(), 1);

    // Create a JDynamicLayer, give it a valid image and rect, add to scene.
    QImage img(50, 50, QImage::Format_ARGB32);
    img.fill(Qt::red);
    auto *layer = new JDynamicLayer(img, JDynamicLayer::EImageType);
    layer->setRect(QRectF(300, 300, 50, 50));
    scene->addItem(layer);

    // Select both items.
    drawApp->setCurrentTool(selection);
    auto bzItems = scene->getBzItems();
    for (auto *item : bzItems) {
        scene->selectGroup()->add(item, false, false);
    }
    scene->selectGroup()->add(layer, false, false);

    QPointF layerPosBefore = layer->sceneBoundingRect().topLeft();

    // Call updateSelectedItemsAlignment — the JDynamicLayer branch (line 1507)
    // calls operatingEnd after operating.
    view->updateSelectedItemsAlignment(Qt::AlignLeft);
    qApp->processEvents();

    QPointF layerPosAfter = layer->sceneBoundingRect().topLeft();

    // The layer should have moved (aligned left to the scene rect left edge).
    // Assert the position changed.
    EXPECT_NE(layerPosBefore.x(), layerPosAfter.x());

    scene->removeItem(layer);
    delete layer;
}
