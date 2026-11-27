// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for PageView eventFilter space-drag and gestureEvent (issue V-9581 §1).
 *
 * Source: src/frame/cgraphicsview.cpp
 *
 * Target lines:
 *   1697-1699, 1703-1724: eventFilter — space key + left button drag scrolls
 *                         the view and calls blockMouseMoveEvent(true/false)
 *   1757: gestureEvent → pinchTriggered path via QGestureEvent
 *
 * Construction:
 *   - For eventFilter: set _spaceKeyPressed=true, set a small view size with
 *     larger scene rect so scrollbars are non-zero, send mouse press + move
 *     events to viewport, assert scrollbar values change and
 *     isBlockMouseMoveEvent() is false after.
 *   - For gestureEvent: create a QGestureEvent with a TestPinchGestureExt,
 *     send it to the view, assert no crash and the view handles it.
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QApplication>
#include <QMouseEvent>
#include <QGestureEvent>
#include <QScrollBar>
#include <QPinchGesture>

#define protected public
#define private public
#include "publicApi.h"
#include "drawshape/globaldefine.h"
#include "drawshape/cdrawscene.h"
#undef protected
#undef private

// QPinchGesture has a protected constructor; subclass to expose it.
class TestPinchGestureExt : public QPinchGesture
{
    Q_OBJECT
public:
    explicit TestPinchGestureExt(QObject *parent = nullptr)
        : QPinchGesture(parent) {}
};

// ---------------------------------------------------------------------------
// eventFilter — space key + left button drag scrolls view
// ---------------------------------------------------------------------------

TEST(PageViewEventFilterExtTest, SpacePlusLeftDrag_ScrollsView)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    // Set the scene rect large enough so scrollbars have a non-zero range.
    scene->setSceneRect(0, 0, 2000, 2000);

    // Set a small fixed view size so scrollbars are needed.
    view->resize(200, 200);
    qApp->processEvents();

    // Enable space key pressed state.
    view->_spaceKeyPressed = true;

    int hBefore = view->horizontalScrollBar()->value();
    int vBefore = view->verticalScrollBar()->value();

    // Send a mouse press to the viewport.
    QPoint pressPos(100, 100);
    QPoint globalPress = view->viewport()->mapToGlobal(pressPos);
    QMouseEvent pressEvent(QEvent::MouseButtonPress, pressPos, globalPress,
                           Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(view->viewport(), &pressEvent);
    qApp->processEvents();

    // Send a mouse move — should trigger scroll.
    QPoint movePos(60, 70);
    QPoint globalMove = view->viewport()->mapToGlobal(movePos);
    QMouseEvent moveEvent(QEvent::MouseMove, movePos, globalMove,
                          Qt::NoButton, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(view->viewport(), &moveEvent);
    qApp->processEvents();

    int hAfter = view->horizontalScrollBar()->value();
    int vAfter = view->verticalScrollBar()->value();

    // At least one scrollbar value should have changed (horizontal or vertical).
    bool scrolled = (hAfter != hBefore) || (vAfter != vBefore);
    EXPECT_TRUE(scrolled);

    // After the move event completes, blockMouseMoveEvent should be false.
    EXPECT_FALSE(scene->isBlockMouseMoveEvent());

    // Clean up.
    view->_spaceKeyPressed = false;
}


// ---------------------------------------------------------------------------
// gestureEvent → pinchTriggered (line 1757)
// ---------------------------------------------------------------------------

TEST(PageViewGestureExtTest, GestureEvent_WithPinchGesture_NoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    // Create a pinch gesture and wrap it in a gesture event.
    TestPinchGestureExt *pinch = new TestPinchGestureExt;
    pinch->setScaleFactor(1.5);
    pinch->setChangeFlags(QPinchGesture::ScaleFactorChanged);

    QGestureEvent gestureEvent({pinch});
    view->gestureEvent(&gestureEvent);

    // The view should have handled the gesture without crashing.
    SUCCEED();

    delete pinch;
}

TEST(PageViewGestureExtTest, GestureEvent_WithRotationChange_NoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    TestPinchGestureExt *pinch = new TestPinchGestureExt;
    pinch->setRotationAngle(45.0);
    pinch->setLastRotationAngle(0.0);
    pinch->setChangeFlags(QPinchGesture::RotationAngleChanged);

    QGestureEvent gestureEvent({pinch});
    view->gestureEvent(&gestureEvent);

    SUCCEED();

    delete pinch;
}

#include "test_pageview_eventfilter_gesture.moc"
