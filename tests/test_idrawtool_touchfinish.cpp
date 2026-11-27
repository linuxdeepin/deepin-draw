// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for IDrawTool toolDoFinish cache and toolDoUpdate sensitivity
 * (issue V-9581 §5).
 *
 * Source: src/drawshape/drawTools/idrawtool.cpp
 *
 * Target lines:
 *   460-463: toolDoFinish — disables view cache after item creation finishes
 *   355-360, 381-384: toolDoUpdate — touch sensitivity radius check;
 *                     inside radius → skip decide (lines 381-383);
 *                     outside radius → do decide (lines 356-359)
 *
 * Uses standalone tools via CDrawToolFactory for touch sensitivity tests to
 * avoid polluting the board's shared current-tool state.
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QTouchEvent>
#include <memory>

#define protected public
#define private public
#include "drawshape/drawTools/idrawtool.h"
#include "drawshape/drawTools/idrawtoolevent.h"
#include "drawshape/drawTools/cdrawtoolfactory.h"
#include "drawshape/globaldefine.h"
#include "drawshape/cdrawscene.h"
#include "publicApi.h"
#undef protected
#undef private

// ---------------------------------------------------------------------------
// toolDoFinish — cache disable (lines 460-463)
// ---------------------------------------------------------------------------

TEST(IDrawToolTouchFinishTest, CreateItemByMouse_DisablesCacheAfterFinish)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    // Enable cache before creating an item.
    view->setCacheEnable(true);
    ASSERT_TRUE(view->isCacheEnabled());

    // Create a rectangle item — toolDoFinish should disable cache (line 460-463).
    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(100, 100), QPoint(200, 200), false);

    // After the item is created and the tool finishes, cache should be off.
    EXPECT_FALSE(view->isCacheEnabled());
}

TEST(IDrawToolTouchFinishTest, CreateItemByMouse_CacheAlreadyOff_StaysOff)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    view->setCacheEnable(false);
    ASSERT_FALSE(view->isCacheEnabled());

    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(50, 50), QPoint(150, 150), false);

    EXPECT_FALSE(view->isCacheEnabled());
}

// ---------------------------------------------------------------------------
// toolDoUpdate — touch sensitivity radius (lines 355-360, 381-384)
// Uses standalone tools to avoid cross-test state pollution.
// ---------------------------------------------------------------------------

TEST(IDrawToolTouchFinishTest, TouchSensitivity_InsideRadius_AcceptsPosition)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    std::unique_ptr<IDrawTool> tool(CDrawToolFactory::Create(rectangle));
    ASSERT_NE(tool, nullptr);
    tool->setDrawBoard(board);

    tool->setTouchSensitiveRadius(20);
    EXPECT_EQ(tool->_touchSensitiveRadius, 20);

    // Create touch events — QTouchEvent must outlive CDrawToolEvent.
    QTouchEvent touchBegin(QEvent::TouchBegin);
    QTouchEvent::TouchPoint tp1(1, QEventPoint::State::Pressed,
                                QPointF(100, 100), QPointF(100, 100));
    CDrawToolEvent eventBegin = CDrawToolEvent::fromTouchPoint(tp1, scene, &touchBegin);
    tool->toolDoStart(&eventBegin);

    // Touch update 5px away — within the 20px radius → doDecide = false (lines 381-383).
    QTouchEvent touchUpdate(QEvent::TouchUpdate);
    QTouchEvent::TouchPoint tp2(1, QEventPoint::State::Updated,
                                QPointF(105, 100), QPointF(105, 100));
    CDrawToolEvent eventUpdate = CDrawToolEvent::fromTouchPoint(tp2, scene, &touchUpdate);
    tool->toolDoUpdate(&eventUpdate);
    SUCCEED();

    // Clean up.
    QTouchEvent touchEnd(QEvent::TouchEnd);
    QTouchEvent::TouchPoint tp3(1, QEventPoint::State::Released,
                                QPointF(105, 100), QPointF(105, 100));
    CDrawToolEvent eventEnd = CDrawToolEvent::fromTouchPoint(tp3, scene, &touchEnd);
    tool->toolDoFinish(&eventEnd);
    tool->interrupt();
}

TEST(IDrawToolTouchFinishTest, TouchSensitivity_OutsideRadius_DecidesOperation)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    std::unique_ptr<IDrawTool> tool(CDrawToolFactory::Create(rectangle));
    ASSERT_NE(tool, nullptr);
    tool->setDrawBoard(board);

    tool->setTouchSensitiveRadius(20);

    QTouchEvent touchBegin(QEvent::TouchBegin);
    QTouchEvent::TouchPoint tp1(1, QEventPoint::State::Pressed,
                                QPointF(100, 100), QPointF(100, 100));
    CDrawToolEvent eventBegin = CDrawToolEvent::fromTouchPoint(tp1, scene, &touchBegin);
    tool->toolDoStart(&eventBegin);

    // Touch update 50px away — outside the 20px radius → doDecide = true (lines 356-359).
    QTouchEvent touchUpdate(QEvent::TouchUpdate);
    QTouchEvent::TouchPoint tp2(1, QEventPoint::State::Updated,
                                QPointF(150, 100), QPointF(150, 100));
    CDrawToolEvent eventUpdate = CDrawToolEvent::fromTouchPoint(tp2, scene, &touchUpdate);
    tool->toolDoUpdate(&eventUpdate);
    SUCCEED();

    // Clean up.
    QTouchEvent touchEnd(QEvent::TouchEnd);
    QTouchEvent::TouchPoint tp3(1, QEventPoint::State::Released,
                                QPointF(150, 100), QPointF(150, 100));
    CDrawToolEvent eventEnd = CDrawToolEvent::fromTouchPoint(tp3, scene, &touchEnd);
    tool->toolDoFinish(&eventEnd);
    tool->interrupt();
}

TEST(IDrawToolTouchFinishTest, TouchSensitivity_ZeroRadius_AlwaysDecides)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    std::unique_ptr<IDrawTool> tool(CDrawToolFactory::Create(rectangle));
    ASSERT_NE(tool, nullptr);
    tool->setDrawBoard(board);

    // Zero radius means no sensitivity check — always decides (line 355 check
    // _touchSensitiveRadius != 0 is false → doDecide stays true).
    tool->setTouchSensitiveRadius(0);

    QTouchEvent touchBegin(QEvent::TouchBegin);
    QTouchEvent::TouchPoint tp1(1, QEventPoint::State::Pressed,
                                QPointF(100, 100), QPointF(100, 100));
    CDrawToolEvent eventBegin = CDrawToolEvent::fromTouchPoint(tp1, scene, &touchBegin);
    tool->toolDoStart(&eventBegin);

    // Small move — with zero radius, it still decides.
    QTouchEvent touchUpdate(QEvent::TouchUpdate);
    QTouchEvent::TouchPoint tp2(1, QEventPoint::State::Updated,
                                QPointF(102, 100), QPointF(102, 100));
    CDrawToolEvent eventUpdate = CDrawToolEvent::fromTouchPoint(tp2, scene, &touchUpdate);
    tool->toolDoUpdate(&eventUpdate);
    SUCCEED();

    // Clean up.
    QTouchEvent touchEnd(QEvent::TouchEnd);
    QTouchEvent::TouchPoint tp3(1, QEventPoint::State::Released,
                                QPointF(102, 100), QPointF(102, 100));
    CDrawToolEvent eventEnd = CDrawToolEvent::fromTouchPoint(tp3, scene, &touchEnd);
    tool->toolDoFinish(&eventEnd);
    tool->interrupt();
}
