// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CDrawToolEvent methods (issue V-5381)
 *
 * Source: src/drawshape/drawTools/idrawtoolevent.cpp
 *
 * Target methods (not already covered by test_cdrawtoolevent.cpp):
 *   CDrawToolEvent: constructor, fromTouchPoint, isAccepted,
 *                   pos, scene, uuid, view
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#define protected public
#define private public
#include "drawshape/drawTools/idrawtoolevent.h"
#include "drawshape/globaldefine.h"
#include "publicApi.h"
#undef protected
#undef private

#include <QTouchEvent>
#include <QEventPoint>
#include <QApplication>

// =========================================================================
// CDrawToolEvent constructor
// =========================================================================

TEST(CDrawToolEventCtor, DefaultConstructor_AllZeros)
{
    CDrawToolEvent event;

    EXPECT_EQ(event.pos(CDrawToolEvent::EScenePos), QPointF(0, 0));
    EXPECT_EQ(event.pos(CDrawToolEvent::EViewportPos), QPointF(0, 0));
    EXPECT_EQ(event.pos(CDrawToolEvent::EGlobelPos), QPointF(0, 0));
    EXPECT_EQ(event.scene(), nullptr);
}

TEST(CDrawToolEventCtor, WithPositions_SetsPosArray)
{
    QPointF vPos(10, 20);
    QPointF scenePos(30, 40);
    QPointF globelPos(50, 60);

    CDrawToolEvent event(vPos, scenePos, globelPos);

    EXPECT_EQ(event.pos(CDrawToolEvent::EViewportPos), vPos);
    EXPECT_EQ(event.pos(CDrawToolEvent::EScenePos), scenePos);
    EXPECT_EQ(event.pos(CDrawToolEvent::EGlobelPos), globelPos);
}

TEST(CDrawToolEventCtor, WithScene_SetsScenePointer)
{
    createNewViewByShortcutKey();
    PageScene *scene = getCurView()->drawScene();
    ASSERT_NE(scene, nullptr);

    CDrawToolEvent event(QPointF(1, 2), QPointF(3, 4),
                         QPointF(5, 6), scene);

    EXPECT_EQ(event.scene(), scene);
}

// =========================================================================
// CDrawToolEvent::pos
// =========================================================================

TEST(CDrawToolEventPos, ValidTypes_ReturnStoredValues)
{
    QPointF vPos(10, 20);
    QPointF scenePos(30, 40);
    QPointF globelPos(50, 60);

    CDrawToolEvent event(vPos, scenePos, globelPos);

    EXPECT_EQ(event.pos(CDrawToolEvent::EScenePos), scenePos);
    EXPECT_EQ(event.pos(CDrawToolEvent::EViewportPos), vPos);
    EXPECT_EQ(event.pos(CDrawToolEvent::EGlobelPos), globelPos);
}

TEST(CDrawToolEventPos, OutOfRangeType_ReturnsZeroPointF)
{
    CDrawToolEvent event(QPointF(10, 20), QPointF(30, 40),
                         QPointF(50, 60));

    EXPECT_EQ(event.pos(CDrawToolEvent::PosTypeCount), QPointF(0, 0));
}

// =========================================================================
// CDrawToolEvent::uuid
// =========================================================================

TEST(CDrawToolEventUuid, DefaultConstructor_UuidIsZero)
{
    CDrawToolEvent event;

    EXPECT_EQ(event.uuid(), 0);
}

// =========================================================================
// CDrawToolEvent::scene
// =========================================================================

TEST(CDrawToolEventScene, DefaultConstructor_ReturnsNull)
{
    CDrawToolEvent event;

    EXPECT_EQ(event.scene(), nullptr);
}

TEST(CDrawToolEventScene, WithScene_ReturnsSamePointer)
{
    createNewViewByShortcutKey();
    PageScene *scene = getCurView()->drawScene();
    ASSERT_NE(scene, nullptr);

    CDrawToolEvent event(QPointF(1, 2), QPointF(3, 4),
                         QPointF(5, 6), scene);

    EXPECT_EQ(event.scene(), scene);
}

// =========================================================================
// CDrawToolEvent::view
// =========================================================================

TEST(CDrawToolEventView, NullScene_ReturnsNull)
{
    CDrawToolEvent event;

    EXPECT_EQ(event.view(), nullptr);
}

TEST(CDrawToolEventView, WithScene_ReturnsDrawView)
{
    createNewViewByShortcutKey();
    PageScene *scene = getCurView()->drawScene();
    ASSERT_NE(scene, nullptr);

    CDrawToolEvent event(QPointF(1, 2), QPointF(3, 4),
                         QPointF(5, 6), scene);

    PageView *view = event.view();
    EXPECT_NE(view, nullptr);
}

// =========================================================================
// CDrawToolEvent::isAccepted / setAccepted
// =========================================================================

TEST(CDrawToolEventAccepted, DefaultIsAcceptedTrue)
{
    CDrawToolEvent event;

    EXPECT_TRUE(event.isAccepted());
}

TEST(CDrawToolEventAccepted, SetFalse_GetReturnsFalse)
{
    CDrawToolEvent event;

    event.setAccepted(false);
    EXPECT_FALSE(event.isAccepted());
}

TEST(CDrawToolEventAccepted, SetTrueAgain_GetReturnsTrue)
{
    CDrawToolEvent event;
    event.setAccepted(false);

    event.setAccepted(true);
    EXPECT_TRUE(event.isAccepted());
}

// =========================================================================
// CDrawToolEvent::fromTouchPoint
// =========================================================================

TEST(CDrawToolEventFromTouchPoint, ValidTouchPoint_SetsPositionsAndUuid)
{
    createNewViewByShortcutKey();
    PageScene *scene = getCurView()->drawScene();
    ASSERT_NE(scene, nullptr);

    QTouchEvent::TouchPoint tp(42, QEventPoint::State::Pressed,
                               QPointF(30, 40), QPointF(50, 60));

    CDrawToolEvent event = CDrawToolEvent::fromTouchPoint(tp, scene);

    EXPECT_EQ(event.uuid(), 42);
    EXPECT_EQ(event.pos(CDrawToolEvent::EScenePos), QPointF(30, 40));
    EXPECT_EQ(event.pos(CDrawToolEvent::EGlobelPos), QPointF(50, 60));
    EXPECT_EQ(event.scene(), scene);
}

TEST(CDrawToolEventFromTouchPoint, NullOrgEvent_KbModsFromGlobalApp)
{
    createNewViewByShortcutKey();
    PageScene *scene = getCurView()->drawScene();
    ASSERT_NE(scene, nullptr);

    QTouchEvent::TouchPoint tp(7, QEventPoint::State::Pressed,
                               QPointF(10, 10), QPointF(20, 20));

    CDrawToolEvent event = CDrawToolEvent::fromTouchPoint(tp, scene, nullptr);

    EXPECT_EQ(event.uuid(), 7);
    EXPECT_EQ(event.orgQtEvent(), nullptr);
}

TEST(CDrawToolEventFromTouchPoint, WithOrgEvent_SetsOrgEvent)
{
    createNewViewByShortcutKey();
    PageScene *scene = getCurView()->drawScene();
    ASSERT_NE(scene, nullptr);

    QTouchEvent::TouchPoint tp(3, QEventPoint::State::Pressed,
                               QPointF(5, 5), QPointF(15, 15));

    QTouchEvent touchEvent(QEvent::TouchBegin);
    CDrawToolEvent event = CDrawToolEvent::fromTouchPoint(tp, scene, &touchEvent);

    EXPECT_EQ(event.orgQtEvent(), &touchEvent);
}
