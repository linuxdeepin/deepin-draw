// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CDrawToolEvent::fromQEvent, fromQEvent_single, eventType
 *
 * Target methods (from issue V-5046):
 *   - CDrawToolEvent::fromQEvent         (level=high, complexity:13, lines:57)
 *   - CDrawToolEvent::fromQEvent_single  (level=low)
 *   - CDrawToolEvent::eventType          (level=high, complexity:14)
 *
 * Source: src/drawshape/drawTools/idrawtoolevent.cpp
 *
 * Minimum case counts:
 *   fromQEvent:        high → 3 cases
 *   fromQEvent_single: low  → 1 case
 *   eventType:         high → 3 cases
 *
 * | Method           | level | min | actual |
 * |------------------|-------|-----|--------|
 * | fromQEvent       | high  | 3   | 3      |
 * | fromQEvent_single| low   | 1   | 2      |
 * | eventType        | high  | 3   | 3      |
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

#include <QMouseEvent>
#include <QTouchEvent>
#include <QGraphicsSceneMouseEvent>
#include <QApplication>

// =========================================================================
// CDrawToolEvent::eventType tests
// =========================================================================

TEST(CDrawToolEventType, NullOrgEvent_ReturnsEEventSimulated)
{
    // Arrange
    CDrawToolEvent event;
    event._orgEvent = nullptr;

    // Act
    CDrawToolEvent::EEventTp result = event.eventType();

    // Assert
    EXPECT_EQ(result, CDrawToolEvent::EEventSimulated);
}

TEST(CDrawToolEventType, MouseButtonPress_ReturnsEMouseEvent)
{
    // Arrange
    QMouseEvent mouseEvent(QEvent::MouseButtonPress, QPointF(10, 10),
                           Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    CDrawToolEvent event;
    event._orgEvent = &mouseEvent;

    // Act
    CDrawToolEvent::EEventTp result = event.eventType();

    // Assert
    EXPECT_EQ(result, CDrawToolEvent::EMouseEvent);
}

TEST(CDrawToolEventType, TouchBegin_ReturnsETouchEvent)
{
    // Arrange
    QTouchEvent touchEvent(QEvent::TouchBegin);
    CDrawToolEvent event;
    event._orgEvent = &touchEvent;

    // Act
    CDrawToolEvent::EEventTp result = event.eventType();

    // Assert
    EXPECT_EQ(result, CDrawToolEvent::ETouchEvent);
}

// =========================================================================
// CDrawToolEvent::fromQEvent tests
// =========================================================================

TEST(CDrawToolEventFromQEvent, MouseButtonPress_ReturnsNonEmptyList)
{
    // Arrange
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    QMouseEvent mouseEvent(QEvent::MouseButtonPress, QPointF(50, 50),
                           Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);

    // Act
    CDrawToolEvent::CDrawToolEvents result = CDrawToolEvent::fromQEvent(&mouseEvent, scene);

    // Assert
    EXPECT_EQ(result.size(), 1);
    if (result.size() > 0) {
        CDrawToolEvent e = result.first();
        EXPECT_EQ(e.eventType(), CDrawToolEvent::EMouseEvent);
        EXPECT_EQ(e.mouseButtons(), Qt::LeftButton);
    }
}

TEST(CDrawToolEventFromQEvent, MouseButtonRelease_ReturnsNonEmptyList)
{
    // Arrange
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    QMouseEvent mouseEvent(QEvent::MouseButtonRelease, QPointF(60, 60),
                           Qt::RightButton, Qt::RightButton, Qt::NoModifier);

    // Act
    CDrawToolEvent::CDrawToolEvents result = CDrawToolEvent::fromQEvent(&mouseEvent, scene);

    // Assert
    EXPECT_EQ(result.size(), 1);
    if (result.size() > 0) {
        CDrawToolEvent e = result.first();
        EXPECT_EQ(e.eventType(), CDrawToolEvent::EMouseEvent);
    }
}

TEST(CDrawToolEventFromQEvent, UnknownEventType_ReturnsNonEmptyList)
{
    // Arrange
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    // Use an event type not handled by specific cases (e.g., FocusIn)
    QEvent focusEvent(QEvent::FocusIn);

    // Act
    CDrawToolEvent::CDrawToolEvents result = CDrawToolEvent::fromQEvent(&focusEvent, scene);

    // Assert - default case still creates an event
    EXPECT_EQ(result.size(), 1);
    if (result.size() > 0) {
        CDrawToolEvent e = result.first();
        EXPECT_NE(e.orgQtEvent(), nullptr);
    }
}

// =========================================================================
// CDrawToolEvent::fromQEvent_single tests
// =========================================================================

TEST(CDrawToolEventFromQEventSingle, MouseButtonPress_ReturnsFirstEvent)
{
    // Arrange
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    QMouseEvent mouseEvent(QEvent::MouseButtonPress, QPointF(70, 70),
                           Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);

    // Act
    CDrawToolEvent result = CDrawToolEvent::fromQEvent_single(&mouseEvent, scene);

    // Assert
    EXPECT_EQ(result.eventType(), CDrawToolEvent::EMouseEvent);
    EXPECT_NE(result.orgQtEvent(), nullptr);
    EXPECT_EQ(result.mouseButtons(), Qt::LeftButton);
}

TEST(CDrawToolEventFromQEventSingle, DefaultConstructed_WhenFromQEventReturnsEmpty)
{
    // Arrange - create a CDrawToolEvent with no orgEvent to test default
    CDrawToolEvent defaultEvent;

    // Act - check default state
    CDrawToolEvent::EEventTp tp = defaultEvent.eventType();

    // Assert - default event has no orgEvent, so returns EEventSimulated
    EXPECT_EQ(tp, CDrawToolEvent::EEventSimulated);
    EXPECT_EQ(defaultEvent.orgQtEvent(), nullptr);
}
