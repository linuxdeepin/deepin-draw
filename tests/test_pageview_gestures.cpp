// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for PageView gesture / drag-drop / view-reset slots
 * (src/frame/cgraphicsview.cpp).
 *
 * Covers (issue V-8025 §1):
 *   - slotViewOrignal()  — reset zoom to original scale 1.0
 *   - slotSetTextAlignment() — text alignment slot (no-crash + with text item)
 *   - dropEvent()        — drag-and-drop file handling
 *   - dragMoveEvent()    — drag move event delegation
 *   - pinchTriggered()   — pinch gesture zoom scaling
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QUrl>
#include <QPinchGesture>

#define protected public
#define private public
#include "publicApi.h"
#undef protected
#undef private

// QPinchGesture has a protected constructor; subclass to expose it for tests.
class TestPinchGesture : public QPinchGesture
{
    Q_OBJECT
public:
    explicit TestPinchGesture(QObject *parent = nullptr)
        : QPinchGesture(parent) {}
};

// ===========================================================================
// slotViewOrignal — resets zoom to 1.0
// ===========================================================================

TEST(PageViewSlotViewOrignal, ResetsScaleToOneAfterZoom)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    // Zoom in to 2.0 so the view is not already at 1.0.
    view->scale(2.0);
    EXPECT_NEAR(view->getScale(), 2.0, 1e-3);

    // slotViewOrignal should reset zoom back to 1.0.
    view->slotViewOrignal();

    EXPECT_NEAR(view->getScale(), 1.0, 1e-3);
}

TEST(PageViewSlotViewOrignal, AlreadyAtOne_NoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    // Already at 1.0 — calling again should be safe.
    EXPECT_NEAR(view->getScale(), 1.0, 1e-3);
    view->slotViewOrignal();
    EXPECT_NEAR(view->getScale(), 1.0, 1e-3);
}

TEST(PageViewSlotViewOrignal, ResetsAfterZoomOut)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    // Zoom out below 1.0.
    view->scale(0.5);
    EXPECT_NEAR(view->getScale(), 0.5, 1e-3);

    view->slotViewOrignal();
    EXPECT_NEAR(view->getScale(), 1.0, 1e-3);
}

// ===========================================================================
// slotSetTextAlignment — text alignment slot
// ===========================================================================

TEST(PageViewSlotSetTextAlignment, NoSelection_NoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    // No text item selected — slot should handle gracefully.
    view->slotSetTextAlignment(Qt::AlignLeft);
    view->slotSetTextAlignment(Qt::AlignCenter);
    view->slotSetTextAlignment(Qt::AlignRight);
    SUCCEED();
}

TEST(PageViewSlotSetTextAlignment, WithTextItem_NoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    // Create a text item and select it, then call the alignment slot.
    drawApp->setCurrentTool(text);
    createItemByMouse(view, false, QPoint(100, 100), QPoint(300, 200), false);

    view->slotOnSelectAll();

    view->slotSetTextAlignment(Qt::AlignLeft);
    view->slotSetTextAlignment(Qt::AlignCenter);
    view->slotSetTextAlignment(Qt::AlignRight);

    SUCCEED();
}

// ===========================================================================
// dragMoveEvent — drag move delegation
// ===========================================================================

TEST(PageViewDragMoveEvent, NoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    QDragMoveEvent event(QPoint(50, 50), Qt::CopyAction, nullptr,
                         Qt::NoButton, Qt::NoModifier);
    view->dragMoveEvent(&event);
    SUCCEED();
}

TEST(PageViewDragMoveEvent, WithMimeData_NoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    QMimeData *mimeData = new QMimeData;
    mimeData->setUrls(QList<QUrl>() << QUrl("file:///tmp/test.png"));
    QDragMoveEvent event(QPoint(50, 50), Qt::CopyAction, mimeData,
                         Qt::LeftButton, Qt::NoModifier);
    view->dragMoveEvent(&event);
    delete mimeData;
    SUCCEED();
}

// ===========================================================================
// dropEvent — drop file handling
// ===========================================================================

TEST(PageViewDropEvent, WithImageUrls_NoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    QMimeData *mimeData = new QMimeData;
    mimeData->setUrls(QList<QUrl>() << QUrl("file:///tmp/test.png"));
    QDropEvent event(QPoint(50, 50), Qt::CopyAction, mimeData,
                     Qt::LeftButton, Qt::NoModifier);
    view->dropEvent(&event);
    delete mimeData;
    SUCCEED();
}

TEST(PageViewDropEvent, EmptyMimeData_NoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    QMimeData *mimeData = new QMimeData;
    QDropEvent event(QPoint(50, 50), Qt::CopyAction, mimeData,
                     Qt::LeftButton, Qt::NoModifier);
    view->dropEvent(&event);
    delete mimeData;
    SUCCEED();
}

TEST(PageViewDropEvent, NoUrls_NoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    QMimeData *mimeData = new QMimeData;
    mimeData->setText("plain text");
    QDropEvent event(QPoint(50, 50), Qt::CopyAction, mimeData,
                     Qt::LeftButton, Qt::NoModifier);
    view->dropEvent(&event);
    delete mimeData;
    SUCCEED();
}

// ===========================================================================
// pinchTriggered — pinch gesture zoom
// ===========================================================================

TEST(PageViewPinchTriggered, ScaleUp_NoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    TestPinchGesture gesture;
    gesture.setScaleFactor(1.5);
    gesture.setTotalScaleFactor(1.5);
    gesture.setCenterPoint(QPointF(100, 100));
    gesture.setChangeFlags(QPinchGesture::ScaleFactorChanged);

    view->pinchTriggered(&gesture);

    // scaleWithCenter multiplies the current scale (1.0) by the factor.
    EXPECT_NEAR(view->getScale(), 1.5, 1e-3);
}

TEST(PageViewPinchTriggered, ScaleDown_NoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    TestPinchGesture gesture;
    gesture.setScaleFactor(0.5);
    gesture.setTotalScaleFactor(0.5);
    gesture.setCenterPoint(QPointF(100, 100));
    gesture.setChangeFlags(QPinchGesture::ScaleFactorChanged);

    view->pinchTriggered(&gesture);

    // 0.5 * 1.0 is inside the [0.1, 20] clamp, so the scale is halved.
    EXPECT_NEAR(view->getScale(), 0.5, 1e-3);
}

TEST(PageViewPinchTriggered, RotationOnly_NoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    TestPinchGesture gesture;
    gesture.setRotationAngle(45.0);
    gesture.setTotalRotationAngle(45.0);
    gesture.setCenterPoint(QPointF(100, 100));
    gesture.setChangeFlags(QPinchGesture::RotationAngleChanged);

    view->pinchTriggered(&gesture);

    // A rotation-only pinch must not change the zoom.
    EXPECT_NEAR(view->getScale(), 1.0, 1e-3);
}

#include "test_pageview_gestures.moc"
