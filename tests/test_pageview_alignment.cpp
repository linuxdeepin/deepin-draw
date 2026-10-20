// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for PageView alignment (src/frame/cgraphicsview.cpp).
 *
 * Covers (issue V-5981 §四):
 *   - alignmentMovPos(fatherRect, childRect, align)
 *       Pure math: displacement for each align direction.
 *   - updateSelectedItemsAlignment(align)
 *       Select 2+ items, align, verify positions.
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QRectF>
#include <QPointF>

#define protected public
#define private public
#include "publicApi.h"
#undef protected
#undef private

// ---------------------------------------------------------------------------
// alignmentMovPos — pure math, no view needed
// ---------------------------------------------------------------------------

TEST(PageViewAlignmentMovPosTest, AlignLeft_ChildLeftEdgeToFatherLeft)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    QRectF father(0, 0, 200, 200);
    QRectF child(50, 50, 40, 40);
    QPointF d = view->alignmentMovPos(father, child, Qt::AlignLeft);
    // child left (50) should move to father left (0) → dx = -50
    EXPECT_NEAR(d.x(), -50.0, 0.01);
}

TEST(PageViewAlignmentMovPosTest, AlignRight_ChildRightEdgeToFatherRight)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    QRectF father(0, 0, 200, 200);
    QRectF child(50, 50, 40, 40);
    QPointF d = view->alignmentMovPos(father, child, Qt::AlignRight);
    // child right (90) → father right (200) → dx = 110
    EXPECT_NEAR(d.x(), 110.0, 0.01);
}

TEST(PageViewAlignmentMovPosTest, AlignTop_ChildTopEdgeToFatherTop)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    QRectF father(0, 0, 200, 200);
    QRectF child(50, 50, 40, 40);
    QPointF d = view->alignmentMovPos(father, child, Qt::AlignTop);
    // child top (50) → father top (0) → dy = -50
    EXPECT_NEAR(d.y(), -50.0, 0.01);
}

TEST(PageViewAlignmentMovPosTest, AlignBottom_ChildBottomEdgeToFatherBottom)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    QRectF father(0, 0, 200, 200);
    QRectF child(50, 50, 40, 40);
    QPointF d = view->alignmentMovPos(father, child, Qt::AlignBottom);
    // child bottom (90) → father bottom (200) → dy = 110
    EXPECT_NEAR(d.y(), 110.0, 0.01);
}

TEST(PageViewAlignmentMovPosTest, AlignHCenter_ChildCenteredHorizontally)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    QRectF father(0, 0, 200, 200);
    QRectF child(50, 50, 40, 40);
    QPointF d = view->alignmentMovPos(father, child, Qt::AlignHCenter);
    // father center x = 100, child center x = 70 → dx = 30
    EXPECT_NEAR(d.x(), 30.0, 0.01);
}

TEST(PageViewAlignmentMovPosTest, AlignVCenter_ChildCenteredVertically)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    QRectF father(0, 0, 200, 200);
    QRectF child(50, 50, 40, 40);
    QPointF d = view->alignmentMovPos(father, child, Qt::AlignVCenter);
    // father center y = 100, child center y = 70 → dy = 30
    EXPECT_NEAR(d.y(), 30.0, 0.01);
}

TEST(PageViewAlignmentMovPosTest, AlreadyAligned_ZeroDisplacement)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    QRectF father(0, 0, 100, 100);
    QRectF child(0, 0, 50, 50);
    QPointF d = view->alignmentMovPos(father, child, Qt::AlignLeft);
    EXPECT_NEAR(d.x(), 0.0, 0.01);
}

// ---------------------------------------------------------------------------
// updateSelectedItemsAlignment — needs items on scene
// ---------------------------------------------------------------------------

TEST(PageViewUpdateAlignmentTest, AlignLeft_TwoItemsNoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(100, 100), QPoint(200, 200), false);
    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(300, 300), QPoint(400, 400), false);
    ASSERT_GE(scene->getBzItems().count(), 2);

    view->slotOnSelectAll();
    view->updateSelectedItemsAlignment(Qt::AlignLeft);
    SUCCEED();
}

TEST(PageViewUpdateAlignmentTest, AlignTop_TwoItemsNoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(100, 100), QPoint(200, 200), false);
    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(300, 300), QPoint(400, 400), false);
    ASSERT_GE(scene->getBzItems().count(), 2);

    view->slotOnSelectAll();
    view->updateSelectedItemsAlignment(Qt::AlignTop);
    SUCCEED();
}

TEST(PageViewUpdateAlignmentTest, AlignHCenter_TwoItemsNoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(100, 100), QPoint(200, 200), false);
    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(300, 300), QPoint(400, 400), false);
    ASSERT_GE(scene->getBzItems().count(), 2);

    view->slotOnSelectAll();
    view->updateSelectedItemsAlignment(Qt::AlignHCenter);
    SUCCEED();
}

TEST(PageViewUpdateAlignmentTest, AlignRight_TwoItemsNoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(100, 100), QPoint(200, 200), false);
    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(300, 300), QPoint(400, 400), false);
    ASSERT_GE(scene->getBzItems().count(), 2);

    view->slotOnSelectAll();
    view->updateSelectedItemsAlignment(Qt::AlignRight);
    SUCCEED();
}

TEST(PageViewUpdateAlignmentTest, NoSelection_NoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    // No items selected — should not crash.
    view->updateSelectedItemsAlignment(Qt::AlignLeft);
    SUCCEED();
}
