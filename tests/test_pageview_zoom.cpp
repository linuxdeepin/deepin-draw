// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for PageView zoom/scale system (src/frame/cgraphicsview.cpp).
 *
 * Covers (issue V-5981 §一):
 *   - getScale()           — initial default value
 *   - scale(s,...)         — set target scale, getScale() consistent; illegal
 *                            values (0, negative, over-limit) clamped
 *   - scaleWithCenter(f,p) — scale around a viewport point
 *   - zoomIn / zoomOut     — repeated calls change getScale() monotonically
 *                            within [0.1, 20]; EScaleCenter branches
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QWheelEvent>

#define protected public
#define private public
#include "publicApi.h"
#undef protected
#undef private

// Scale upper/lower bounds enforced by scaleWithCenter().
static const qreal kScaleMin = 0.1;
static const qreal kScaleMax = 20.0;

TEST(PageViewZoomTest, GetScale_DefaultIsOneAfterNewView)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    // A freshly created view keeps the default scale of 1.0.
    EXPECT_NEAR(view->getScale(), 1.0, 1e-6);
}

TEST(PageViewZoomTest, Scale_SetsGetScaleConsistent)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    view->scale(2.0);
    EXPECT_NEAR(view->getScale(), 2.0, 1e-3);

    view->scale(0.5);
    EXPECT_NEAR(view->getScale(), 0.5, 1e-3);
}

TEST(PageViewZoomTest, Scale_ZeroOrNegativeClampedToLowerBound)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    view->scale(0.0);
    EXPECT_GE(view->getScale(), kScaleMin);

    view->scale(-1.0);
    EXPECT_GE(view->getScale(), kScaleMin);
}

TEST(PageViewZoomTest, Scale_OverLimitClampedToUpperBound)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    view->scale(100.0);
    EXPECT_LE(view->getScale(), kScaleMax);
    EXPECT_NEAR(view->getScale(), kScaleMax, 1e-3);
}

TEST(PageViewZoomTest, ScaleWithCenter_PreservesViewportPointSceneCoord)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    // Pick a viewport point and record the scene coordinate it maps to.
    QPoint vp(view->viewport()->width() / 2, view->viewport()->height() / 2);
    QPointF sceneBefore = view->mapToScene(vp);

    // Scale up around that point; the same viewport point should keep
    // mapping to (close to) the same scene coordinate.
    view->scaleWithCenter(2.0, vp);
    QPointF sceneAfter = view->mapToScene(vp);

    EXPECT_NEAR(sceneAfter.x(), sceneBefore.x(), 1.0);
    EXPECT_NEAR(sceneAfter.y(), sceneBefore.y(), 1.0);
}

TEST(PageViewZoomTest, ScaleWithCenter_ClampsFactorToUpperBound)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    // From default 1.0, an extreme factor must clamp to the upper bound.
    view->scaleWithCenter(1000.0, QPoint(10, 10));
    EXPECT_NEAR(view->getScale(), kScaleMax, 1e-3);
}

TEST(PageViewZoomTest, ScaleWithCenter_ClampsFactorToLowerBound)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    view->scaleWithCenter(0.0, QPoint(10, 10));
    EXPECT_NEAR(view->getScale(), kScaleMin, 1e-3);
}

TEST(PageViewZoomTest, ZoomOut_IncreasesScaleMonotonically)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    qreal prev = view->getScale();
    for (int i = 0; i < 5; ++i) {
        view->zoomOut(PageView::EViewCenter, QPoint());
        qreal cur = view->getScale();
        EXPECT_GE(cur, prev - 1e-9);  // non-decreasing
        EXPECT_LE(cur, kScaleMax + 1e-6);
        prev = cur;
    }
}

TEST(PageViewZoomTest, ZoomIn_DecreasesScaleMonotonically)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    // Move to a higher scale first so zoomIn has room to decrease.
    view->scale(5.0);
    qreal prev = view->getScale();
    for (int i = 0; i < 5; ++i) {
        view->zoomIn(PageView::EViewCenter, QPoint());
        qreal cur = view->getScale();
        EXPECT_LE(cur, prev + 1e-9);  // non-increasing
        EXPECT_GE(cur, kScaleMin - 1e-6);
        prev = cur;
    }
}

TEST(PageViewZoomTest, ZoomOut_AtUpperBoundStaysAtBound)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    view->scale(kScaleMax);
    view->zoomOut(PageView::EViewCenter, QPoint());
    EXPECT_NEAR(view->getScale(), kScaleMax, 1e-3);
}

TEST(PageViewZoomTest, ZoomIn_AtLowerBoundStaysAtBound)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    view->scale(kScaleMin);
    view->zoomIn(PageView::EViewCenter, QPoint());
    EXPECT_NEAR(view->getScale(), kScaleMin, 1e-3);
}

TEST(PageViewZoomTest, Scale_EMousePosBranchNoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    // Exercise the EMousePos branch of scale(); no crash = pass.
    view->scale(1.5, PageView::EMousePos, QPoint());
    SUCCEED();
}

TEST(PageViewZoomTest, Scale_ESceneCenterBranchNoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    view->scale(1.5, PageView::ESceneCenter, QPoint());
    EXPECT_NEAR(view->getScale(), 1.5, 1e-3);
}

TEST(PageViewZoomTest, Scale_ECustomViewPosBranchNoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    QPoint vp(20, 20);
    view->scale(1.5, PageView::ECustomViewPos, vp);
    EXPECT_NEAR(view->getScale(), 1.5, 1e-3);
}
