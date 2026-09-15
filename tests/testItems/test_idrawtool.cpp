// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include "qteventcompat.h"

#define protected public
#define private public
#include "mainwindow.h"
#include "idrawtool.h"
#include "idrawtoolevent.h"
#include "csizehandlerect.h"
#include "globaldefine.h"
#include "cgraphicsview.h"
#include "ccentralwidget.h"
#include "application.h"
#include "clefttoolbar.h"
#include "toptoolbar.h"
#include "drawshape/cdrawscene.h"
#undef protected
#undef private

#include "publicApi.h"

#include <QTimer>
#include <QEvent>

#if TEST_IDRAWTOOL_ITEM

// =========================================================================
// Helper: obtain a valid scene from a freshly created view
// =========================================================================
static PageScene *getTestScene()
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    if (!view)
        return nullptr;
    return view->drawScene();
}

// =========================================================================
// getCursor tests
//   getCursor is dead code (#if 0 block) — always returns QCursor().
// =========================================================================

TEST(IDrawToolGetCursor, AllDirectionsWithoutMousePress)
{
    IDrawTool tool(rectangle);

    QCursor c1 = tool.getCursor(CSizeHandleRect::LeftTop, false, 0);
    EXPECT_EQ(c1.shape(), Qt::ArrowCursor);

    QCursor c2 = tool.getCursor(CSizeHandleRect::Top, false, 0);
    EXPECT_EQ(c2.shape(), Qt::ArrowCursor);

    QCursor c3 = tool.getCursor(CSizeHandleRect::RightTop, false, 0);
    EXPECT_EQ(c3.shape(), Qt::ArrowCursor);

    QCursor c4 = tool.getCursor(CSizeHandleRect::Right, false, 0);
    EXPECT_EQ(c4.shape(), Qt::ArrowCursor);

    QCursor c5 = tool.getCursor(CSizeHandleRect::RightBottom, false, 0);
    EXPECT_EQ(c5.shape(), Qt::ArrowCursor);

    QCursor c6 = tool.getCursor(CSizeHandleRect::Bottom, false, 0);
    EXPECT_EQ(c6.shape(), Qt::ArrowCursor);

    QCursor c7 = tool.getCursor(CSizeHandleRect::LeftBottom, false, 0);
    EXPECT_EQ(c7.shape(), Qt::ArrowCursor);

    QCursor c8 = tool.getCursor(CSizeHandleRect::Left, false, 0);
    EXPECT_EQ(c8.shape(), Qt::ArrowCursor);
}

TEST(IDrawToolGetCursor, RotationInRectNoneDirections)
{
    IDrawTool tool(pen);

    QCursor c1 = tool.getCursor(CSizeHandleRect::Rotation, false, 0);
    EXPECT_EQ(c1.shape(), Qt::ArrowCursor);

    QCursor c2 = tool.getCursor(CSizeHandleRect::InRect, false, 0);
    EXPECT_EQ(c2.shape(), Qt::ArrowCursor);

    QCursor c3 = tool.getCursor(CSizeHandleRect::None, false, 0);
    EXPECT_EQ(c3.shape(), Qt::ArrowCursor);
}

TEST(IDrawToolGetCursor, WithMouseLeftPress)
{
    IDrawTool tool(ellipse);

    QCursor c1 = tool.getCursor(CSizeHandleRect::LeftTop, true, 0);
    EXPECT_EQ(c1.shape(), Qt::ArrowCursor);

    QCursor c2 = tool.getCursor(CSizeHandleRect::Right, true, 0);
    EXPECT_EQ(c2.shape(), Qt::ArrowCursor);

    QCursor c3 = tool.getCursor(CSizeHandleRect::Bottom, true, 0);
    EXPECT_EQ(c3.shape(), Qt::ArrowCursor);

    QCursor c4 = tool.getCursor(CSizeHandleRect::InRect, true, 0);
    EXPECT_EQ(c4.shape(), Qt::ArrowCursor);
}

TEST(IDrawToolGetCursor, DifferentToolTypes)
{
    IDrawTool tool(line);

    QCursor c1 = tool.getCursor(CSizeHandleRect::InRect, false, 1);
    EXPECT_EQ(c1.shape(), Qt::ArrowCursor);

    QCursor c2 = tool.getCursor(CSizeHandleRect::InRect, true, 1);
    EXPECT_EQ(c2.shape(), Qt::ArrowCursor);

    QCursor c3 = tool.getCursor(CSizeHandleRect::None, false, 2);
    EXPECT_EQ(c3.shape(), Qt::ArrowCursor);
}

// =========================================================================
// toolDoStart tests
// =========================================================================

TEST(IDrawToolToolDoStart, SimulatedEventNoButton_NoRecordCreated)
{
    IDrawTool tool(rectangle);

    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    CDrawToolEvent event(QPointF(100, 100), QPointF(100, 100), QPointF(100, 100), scene);
    // Default: _msBtns = Qt::NoButton, _orgEvent = nullptr → eventType() = EEventSimulated
    tool.toolDoStart(&event);

    // Simulated event with no button → early return, no ITERecordInfo created
    EXPECT_TRUE(tool._allITERecordInfo.isEmpty());
}

TEST(IDrawToolToolDoStart, MouseLeftButton_CreatesRecord)
{
    IDrawTool tool(rectangle);
    tool.setDrawBoard(getMainWindow()->drawBoard());

    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    CDrawToolEvent event(QPointF(100, 100), QPointF(100, 100), QPointF(100, 100), scene);
    event._msBtns = Qt::LeftButton;

    tool.toolDoStart(&event);

    // Mouse left button → ITERecordInfo created with ENormal
    EXPECT_EQ(tool._allITERecordInfo.size(), 1);
    auto it = tool._allITERecordInfo.find(event.uuid());
    ASSERT_NE(it, tool._allITERecordInfo.end());
    EXPECT_EQ(it.value().eventLife, IDrawTool::ENormal);

    tool.clearITE();
}

TEST(IDrawToolToolDoStart, TouchEvent_CreatesRecord)
{
    IDrawTool tool(rectangle);
    tool.setDrawBoard(getMainWindow()->drawBoard());

    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    CDrawToolEvent event(QPointF(100, 100), QPointF(100, 100), QPointF(100, 100), scene);
    QEvent touchEvent(QEvent::TouchBegin);
    event._orgEvent = &touchEvent;

    // Stop any previously active timer from earlier tests
    tool.getTimerForDoubleCliked()->stop();

    tool.toolDoStart(&event);

    // Touch event → ITERecordInfo created with ENormal
    EXPECT_EQ(tool._allITERecordInfo.size(), 1);
    auto it = tool._allITERecordInfo.find(event.uuid());
    ASSERT_NE(it, tool._allITERecordInfo.end());
    EXPECT_EQ(it.value().eventLife, IDrawTool::ENormal);

    tool.clearITE();
    tool.getTimerForDoubleCliked()->stop();
}

TEST(IDrawToolToolDoStart, MaxTouchPointsExceeded_EDoNotthing)
{
    IDrawTool tool(rectangle);
    tool.setDrawBoard(getMainWindow()->drawBoard());

    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    // Pre-fill one active ITERecordInfo to reach max touch point count
    IDrawTool::ITERecordInfo preInfo;
    preInfo.eventLife = IDrawTool::ENormal;
    preInfo._scene = scene;
    tool._allITERecordInfo.insert(1, preInfo);

    CDrawToolEvent event(QPointF(100, 100), QPointF(100, 100), QPointF(100, 100), scene);
    event._msBtns = Qt::LeftButton;

    tool.toolDoStart(&event);

    // Max touch points exceeded → new record has EDoNotthing
    EXPECT_EQ(tool._allITERecordInfo.size(), 2);
    auto it = tool._allITERecordInfo.find(event.uuid());
    ASSERT_NE(it, tool._allITERecordInfo.end());
    EXPECT_EQ(it.value().eventLife, IDrawTool::EDoNotthing);

    tool.clearITE();
}

// =========================================================================
// toolDoUpdate tests
// =========================================================================

TEST(IDrawToolToolDoUpdate, EmptyRecords_CallsMouseHoverEvent)
{
    IDrawTool tool(rectangle);

    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    CDrawToolEvent event(QPointF(100, 100), QPointF(100, 100), QPointF(100, 100), scene);

    // Empty _allITERecordInfo → calls mouseHoverEvent → setCursor on scene
    tool.toolDoUpdate(&event);
    SUCCEED();
}

TEST(IDrawToolToolDoUpdate, EDoNotthing_AcceptsEvent)
{
    IDrawTool tool(rectangle);

    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    CDrawToolEvent event(QPointF(100, 100), QPointF(100, 100), QPointF(100, 100), scene);

    IDrawTool::ITERecordInfo info;
    info.eventLife = IDrawTool::EDoNotthing;
    info._scene = scene;
    info._curEvent = event;
    info._startEvent = event;
    tool._allITERecordInfo.insert(event.uuid(), info);

    tool.toolDoUpdate(&event);

    EXPECT_TRUE(event.isAccepted());

    tool.clearITE();
}

TEST(IDrawToolToolDoUpdate, EDoQtCoversion_RejectsEvent)
{
    IDrawTool tool(rectangle);

    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    CDrawToolEvent event(QPointF(100, 100), QPointF(100, 100), QPointF(100, 100), scene);

    IDrawTool::ITERecordInfo info;
    info.eventLife = IDrawTool::EDoQtCoversion;
    info._scene = scene;
    info._curEvent = event;
    info._startEvent = event;
    tool._allITERecordInfo.insert(event.uuid(), info);

    tool.toolDoUpdate(&event);

    EXPECT_FALSE(event.isAccepted());

    tool.clearITE();
}

TEST(IDrawToolToolDoUpdate, ENormal_NoMovement_CompletesWithoutError)
{
    IDrawTool tool(rectangle);
    tool.setDrawBoard(getMainWindow()->drawBoard());

    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    CDrawToolEvent event(QPointF(100, 100), QPointF(100, 100), QPointF(100, 100), scene);
    event._msBtns = Qt::LeftButton;

    // Use toolDoStart to create a properly initialized ITERecordInfo
    tool.toolDoStart(&event);
    ASSERT_EQ(tool._allITERecordInfo.size(), 1);

    // Call toolDoUpdate with the same event (no movement)
    tool.toolDoUpdate(&event);

    // Verify the record still exists and haveDecidedOperateType is set
    auto it = tool._allITERecordInfo.find(event.uuid());
    ASSERT_NE(it, tool._allITERecordInfo.end());
    EXPECT_TRUE(it.value().haveDecidedOperateType);

    tool.clearITE();
}

// =========================================================================
// toolDoFinish tests
//   toolDoFinish always calls event->view()->setFocus() at the end,
//   so the event must have a valid scene that returns a valid view.
// =========================================================================

TEST(IDrawToolToolDoFinish, EmptyRecords_RejectsEvent)
{
    IDrawTool tool(rectangle);

    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    CDrawToolEvent event(QPointF(100, 100), QPointF(100, 100), QPointF(100, 100), scene);

    // Empty _allITERecordInfo → setAccepted(false), then view()->setFocus()
    tool.toolDoFinish(&event);

    EXPECT_FALSE(event.isAccepted());
}

TEST(IDrawToolToolDoFinish, EDoNotthing_AcceptsEvent)
{
    IDrawTool tool(rectangle);
    tool.setDrawBoard(getMainWindow()->drawBoard());

    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    CDrawToolEvent event(QPointF(100, 100), QPointF(100, 100), QPointF(100, 100), scene);

    IDrawTool::ITERecordInfo info;
    info.eventLife = IDrawTool::EDoNotthing;
    info._scene = scene;
    info._curEvent = event;
    info._startEvent = event;
    tool._allITERecordInfo.insert(event.uuid(), info);

    tool.toolDoFinish(&event);

    EXPECT_TRUE(event.isAccepted());
    // Record should be erased after finish
    EXPECT_TRUE(tool._allITERecordInfo.isEmpty());
}

TEST(IDrawToolToolDoFinish, EDoQtCoversion_RejectsEvent)
{
    IDrawTool tool(rectangle);
    tool.setDrawBoard(getMainWindow()->drawBoard());

    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    CDrawToolEvent event(QPointF(100, 100), QPointF(100, 100), QPointF(100, 100), scene);

    IDrawTool::ITERecordInfo info;
    info.eventLife = IDrawTool::EDoQtCoversion;
    info._scene = scene;
    info._curEvent = event;
    info._startEvent = event;
    tool._allITERecordInfo.insert(event.uuid(), info);

    tool.toolDoFinish(&event);

    EXPECT_FALSE(event.isAccepted());
    // Record should be erased after finish
    EXPECT_TRUE(tool._allITERecordInfo.isEmpty());
}

TEST(IDrawToolToolDoFinish, ENormal_CompletesSuccessfully)
{
    IDrawTool tool(rectangle);
    tool.setDrawBoard(getMainWindow()->drawBoard());

    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    CDrawToolEvent event(QPointF(100, 100), QPointF(100, 100), QPointF(100, 100), scene);
    event._msBtns = Qt::LeftButton;

    // Use toolDoStart to create a properly initialized ITERecordInfo with ENormal
    tool.toolDoStart(&event);
    ASSERT_EQ(tool._allITERecordInfo.size(), 1);

    // Call toolDoFinish — normal path calls toolFinish which accepts event
    tool.toolDoFinish(&event);

    EXPECT_TRUE(event.isAccepted());
    // Record should be erased after finish
    EXPECT_TRUE(tool._allITERecordInfo.isEmpty());
}

// =========================================================================
// dueTouchDoubleClickedStart tests
//   dueTouchDoubleClickedStart is protected — accessible via
//   #define protected public.
// =========================================================================

TEST(IDrawToolDueTouchDoubleClickedStart, SimulatedEvent_ReturnsFalse)
{
    IDrawTool tool(rectangle);

    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    CDrawToolEvent event(QPointF(100, 100), QPointF(100, 100), QPointF(100, 100), scene);
    // Default: _orgEvent = nullptr → eventType() = EEventSimulated

    bool result = tool.dueTouchDoubleClickedStart(&event);

    EXPECT_FALSE(result);
    EXPECT_TRUE(tool._allITERecordInfo.isEmpty());
}

TEST(IDrawToolDueTouchDoubleClickedStart, TouchEventFirstCall_ReturnsFalse)
{
    IDrawTool tool(rectangle);
    tool.setDrawBoard(getMainWindow()->drawBoard());

    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    // Ensure timer is stopped from any previous test
    tool.getTimerForDoubleCliked()->stop();

    CDrawToolEvent event(QPointF(100, 100), QPointF(100, 100), QPointF(100, 100), scene);
    QEvent touchEvent(QEvent::TouchBegin);
    event._orgEvent = &touchEvent;

    bool result = tool.dueTouchDoubleClickedStart(&event);

    // First touch call starts timer, returns false
    EXPECT_FALSE(result);
    EXPECT_TRUE(tool.getTimerForDoubleCliked()->isActive());

    tool.getTimerForDoubleCliked()->stop();
}

TEST(IDrawToolDueTouchDoubleClickedStart, TouchEventDoubleClick_ReturnsTrue)
{
    IDrawTool tool(rectangle);
    tool.setDrawBoard(getMainWindow()->drawBoard());

    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    // Ensure timer is stopped from any previous test
    tool.getTimerForDoubleCliked()->stop();

    // First touch event — starts timer
    CDrawToolEvent event1(QPointF(100, 100), QPointF(100, 100), QPointF(100, 100), scene);
    QEvent touchEvent1(QEvent::TouchBegin);
    event1._orgEvent = &touchEvent1;
    bool result1 = tool.dueTouchDoubleClickedStart(&event1);
    EXPECT_FALSE(result1);

    // Second touch event at same position — within 20x20 rect → double-click
    CDrawToolEvent event2(QPointF(100, 100), QPointF(100, 100), QPointF(100, 100), scene);
    QEvent touchEvent2(QEvent::TouchBegin);
    event2._orgEvent = &touchEvent2;
    bool result2 = tool.dueTouchDoubleClickedStart(&event2);

    EXPECT_TRUE(result2);

    // Clean up
    tool.clearITE();
    tool.getTimerForDoubleCliked()->stop();
}

TEST(IDrawToolDueTouchDoubleClickedStart, MouseDblClick_ReturnsTrue)
{
    IDrawTool tool(rectangle);
    tool.setDrawBoard(getMainWindow()->drawBoard());

    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    CDrawToolEvent event(QPointF(100, 100), QPointF(100, 100), QPointF(100, 100), scene);
    QEvent dblClickEvent(QEvent::MouseButtonDblClick);
    event._orgEvent = &dblClickEvent;
    event._msBtns = Qt::LeftButton;

    bool result = tool.dueTouchDoubleClickedStart(&event);

    EXPECT_TRUE(result);

    // Clean up
    tool.clearITE();
    tool.getTimerForDoubleCliked()->stop();
}

#endif // TEST_IDRAWTOOL_ITEM
