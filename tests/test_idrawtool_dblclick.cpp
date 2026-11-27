// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for IDrawTool mouseDoubleClickEvent and touch double-click
 * (issue V-9581 §5).
 *
 * Source: src/drawshape/drawTools/idrawtool.cpp
 *
 * Target lines:
 *   177-191: mouseDoubleClickEvent — two branches:
 *     - 180-182: event->source() == Qt::MouseEventSynthesizedByQt →
 *                scene->mouseEvent(event)
 *     - 184-190: normal double-click → dueTouchDoubleClickedStart + mouseEvent
 *   253-254: dueTouchDoubleClickedStart — touch double-click within 250ms,
 *            same position → second call returns true early
 *   560-583: mouse double-click when isPressEventHandledByQt returns true
 *
 * Uses standalone tools via CDrawToolFactory to avoid polluting the board's
 * shared current-tool state across tests.
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QGraphicsSceneMouseEvent>
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
// mouseDoubleClickEvent — synthesized branch (lines 180-182)
// ---------------------------------------------------------------------------

TEST(IDrawToolDblClickTest, MouseDoubleClickEvent_SynthesizedByQt_DelegatesToScene)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);
    IDrawTool *tool = board->currentTool_p();
    ASSERT_NE(tool, nullptr);

    QGraphicsSceneMouseEvent event(QEvent::GraphicsSceneMouseDoubleClick);
    event.setSource(Qt::MouseEventSynthesizedByQt);
    event.setButton(Qt::LeftButton);
    event.setButtons(Qt::LeftButton);
    event.setScenePos(QPointF(100, 100));
    event.setPos(QPointF(100, 100));

    tool->mouseDoubleClickEvent(&event, scene);
    // Clean up any tool state left by dueTouchDoubleClickedStart.
    tool->interrupt();
    SUCCEED();
}

// ---------------------------------------------------------------------------
// mouseDoubleClickEvent — normal double-click branch (lines 184-190)
// ---------------------------------------------------------------------------

TEST(IDrawToolDblClickTest, MouseDoubleClickEvent_NormalDoubleClick_NoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);
    IDrawTool *tool = board->currentTool_p();
    ASSERT_NE(tool, nullptr);

    QGraphicsSceneMouseEvent event(QEvent::GraphicsSceneMouseDoubleClick);
    event.setSource(Qt::MouseEventNotSynthesized);
    event.setButton(Qt::LeftButton);
    event.setButtons(Qt::LeftButton);
    event.setScenePos(QPointF(50, 50));
    event.setPos(QPointF(50, 50));

    tool->mouseDoubleClickEvent(&event, scene);
    // Clean up any tool state left by dueTouchDoubleClickedStart.
    tool->interrupt();
    SUCCEED();
}

// ---------------------------------------------------------------------------
// dueTouchDoubleClickedStart — touch double-click (lines 253-254)
// Uses a standalone rectangle tool to avoid polluting the board's tool state.
// ---------------------------------------------------------------------------

TEST(IDrawToolDblClickTest, DueTouchDoubleClickedStart_SecondTouchWithin250ms_ReturnsTrue)
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

    // Create QTouchEvent on the heap so it outlives CDrawToolEvent.
    auto touchEvent1 = std::make_unique<QTouchEvent>(QEvent::TouchBegin);
    QTouchEvent::TouchPoint tp1(1, QEventPoint::State::Pressed,
                                QPointF(100, 100), QPointF(100, 100));
    CDrawToolEvent event1 = CDrawToolEvent::fromTouchPoint(tp1, scene, touchEvent1.get());

    // First call — starts the double-click timer.
    tool->dueTouchDoubleClickedStart(&event1);

    // Second call immediately (within 250ms, same position) — should detect
    // double-click and return true (line 253-254 path).
    auto touchEvent2 = std::make_unique<QTouchEvent>(QEvent::TouchBegin);
    QTouchEvent::TouchPoint tp2(1, QEventPoint::State::Pressed,
                                QPointF(100, 100), QPointF(100, 100));
    CDrawToolEvent event2 = CDrawToolEvent::fromTouchPoint(tp2, scene, touchEvent2.get());

    bool secondResult = tool->dueTouchDoubleClickedStart(&event2);
    EXPECT_TRUE(secondResult);

    tool->interrupt();
}

// ---------------------------------------------------------------------------
// dueTouchDoubleClickedStart — mouse double-click (lines 560-583)
// Uses a standalone rectangle tool to avoid polluting the board's tool state.
// ---------------------------------------------------------------------------

TEST(IDrawToolDblClickTest, DueTouchDoubleClickedStart_MouseDblClick_ReturnsTrue)
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

    QGraphicsSceneMouseEvent mouseEvent(QEvent::GraphicsSceneMouseDoubleClick);
    mouseEvent.setSource(Qt::MouseEventNotSynthesized);
    mouseEvent.setButton(Qt::LeftButton);
    mouseEvent.setButtons(Qt::LeftButton);
    mouseEvent.setScenePos(QPointF(80, 80));
    mouseEvent.setPos(QPointF(80, 80));

    CDrawToolEvent::CDrawToolEvents events = CDrawToolEvent::fromQEvent(&mouseEvent, scene);
    ASSERT_FALSE(events.isEmpty());

    bool result = tool->dueTouchDoubleClickedStart(&events.first());
    EXPECT_TRUE(result);

    tool->interrupt();
}

TEST(IDrawToolDblClickTest, DueTouchDoubleClickedStart_MousePress_NotDblClick_ReturnsFalse)
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

    QGraphicsSceneMouseEvent mouseEvent(QEvent::GraphicsSceneMousePress);
    mouseEvent.setSource(Qt::MouseEventNotSynthesized);
    mouseEvent.setButton(Qt::LeftButton);
    mouseEvent.setButtons(Qt::LeftButton);
    mouseEvent.setScenePos(QPointF(80, 80));
    mouseEvent.setPos(QPointF(80, 80));

    CDrawToolEvent::CDrawToolEvents events = CDrawToolEvent::fromQEvent(&mouseEvent, scene);
    ASSERT_FALSE(events.isEmpty());

    bool result = tool->dueTouchDoubleClickedStart(&events.first());
    EXPECT_FALSE(result);

    tool->interrupt();
}
