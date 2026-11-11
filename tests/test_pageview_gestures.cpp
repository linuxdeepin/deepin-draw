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
#include <memory>

#define protected public
#define private public
#include "publicApi.h"
#include "drawshape/globaldefine.h"
#include "drawshape/drawItems/bzItems/cgraphicstextitem.h"
#include "widgets/ctextedit.h"
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
    ensurePageExists();
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
    ensurePageExists();
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
    ensurePageExists();
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

TEST(PageViewSlotSetTextAlignment, NoSelection_SelectionUnchanged)
{
    createNewViewByShortcutKey();
    ensurePageExists();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    // No text item selected — the slot must be a no-op that keeps the
    // selection state unchanged.
    ASSERT_TRUE(view->drawScene()->selectedItems().isEmpty());
    view->slotSetTextAlignment(Qt::AlignLeft);
    view->slotSetTextAlignment(Qt::AlignHCenter);
    view->slotSetTextAlignment(Qt::AlignRight);
    EXPECT_TRUE(view->drawScene()->selectedItems().isEmpty());
}

TEST(PageViewSlotSetTextAlignment, WithTextItem_AppliesAlignment)
{
    createNewViewByShortcutKey();
    ensurePageExists();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    // Create a text item, select it and put it into edit state — the slot
    // only applies the alignment when the selected text item is editing.
    drawApp->setCurrentTool(text);
    createItemByMouse(view, false, QPoint(100, 100), QPoint(300, 200), false);
    view->slotOnSelectAll();

    PageScene *scene = view->drawScene();
    ASSERT_FALSE(scene->selectedItems().isEmpty());
    ASSERT_EQ(scene->selectedItems().first()->type(), TextType);
    auto *textItem = static_cast<CGraphicsTextItem *>(scene->selectedItems().first());
    ASSERT_NE(textItem, nullptr);
    textItem->setTextState(CGraphicsTextItem::EInEdit);

    view->slotSetTextAlignment(Qt::AlignHCenter);
    EXPECT_EQ(static_cast<int>(textItem->m_pTextEdit->alignment()),
              static_cast<int>(Qt::AlignHCenter));

    view->slotSetTextAlignment(Qt::AlignRight);
    EXPECT_EQ(static_cast<int>(textItem->m_pTextEdit->alignment()),
              static_cast<int>(Qt::AlignRight));

    // Clean up the created item so later tests start from a pristine scene.
    scene->clearSelectGroup();
    delete textItem;
}

// ===========================================================================
// dragMoveEvent — drag move delegation
// ===========================================================================

TEST(PageViewDragMoveEvent, NoMimeData_IgnoredByScrollAreaBase)
{
    createNewViewByShortcutKey();
    ensurePageExists();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    // Delegates to QAbstractScrollArea::dragMoveEvent — a pure pass-through
    // with no drop handling at view level: calling it must leave the view
    // state untouched.
    QDragMoveEvent event(QPoint(50, 50), Qt::CopyAction, nullptr,
                         Qt::NoButton, Qt::NoModifier);
    const int itemCountBefore = view->drawScene()->items().count();
    ASSERT_TRUE(view->drawScene()->selectedItems().isEmpty());
    view->dragMoveEvent(&event);
    EXPECT_TRUE(view->drawScene()->selectedItems().isEmpty());
    EXPECT_EQ(view->drawScene()->items().count(), itemCountBefore);
}

TEST(PageViewDragMoveEvent, WithFileUrl_IgnoredByScrollAreaBase)
{
    createNewViewByShortcutKey();
    ensurePageExists();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    std::unique_ptr<QMimeData> mimeData(new QMimeData);
    mimeData->setUrls(QList<QUrl>() << QUrl("file:///tmp/test.png"));
    QDragMoveEvent event(QPoint(50, 50), Qt::CopyAction, mimeData.get(),
                         Qt::LeftButton, Qt::NoModifier);
    // Pure pass-through to QAbstractScrollArea::dragMoveEvent: the view
    // state and the mime payload must stay untouched.
    const int itemCountBefore = view->drawScene()->items().count();
    ASSERT_TRUE(view->drawScene()->selectedItems().isEmpty());
    view->dragMoveEvent(&event);
    EXPECT_TRUE(view->drawScene()->selectedItems().isEmpty());
    EXPECT_EQ(view->drawScene()->items().count(), itemCountBefore);
    EXPECT_TRUE(mimeData->hasUrls());
    EXPECT_EQ(event.dropAction(), Qt::CopyAction);
}

// ===========================================================================
// dropEvent — drop file handling
// ===========================================================================

TEST(PageViewDropEvent, WithImageUrls_IgnoredByScrollAreaBase)
{
    createNewViewByShortcutKey();
    ensurePageExists();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    // Delegates to QAbstractScrollArea::dropEvent — a pure pass-through:
    // file import does not go through the view's drop handler, so the view
    // state and the mime payload must stay untouched.
    std::unique_ptr<QMimeData> mimeData(new QMimeData);
    mimeData->setUrls(QList<QUrl>() << QUrl("file:///tmp/test.png"));
    QDropEvent event(QPoint(50, 50), Qt::CopyAction, mimeData.get(),
                     Qt::LeftButton, Qt::NoModifier);
    const int itemCountBefore = view->drawScene()->items().count();
    ASSERT_TRUE(view->drawScene()->selectedItems().isEmpty());
    view->dropEvent(&event);
    EXPECT_TRUE(view->drawScene()->selectedItems().isEmpty());
    EXPECT_EQ(view->drawScene()->items().count(), itemCountBefore);
    EXPECT_TRUE(mimeData->hasUrls());
    EXPECT_EQ(event.dropAction(), Qt::CopyAction);
}

TEST(PageViewDropEvent, EmptyMimeData_IgnoredByScrollAreaBase)
{
    createNewViewByShortcutKey();
    ensurePageExists();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    std::unique_ptr<QMimeData> mimeData(new QMimeData);
    QDropEvent event(QPoint(50, 50), Qt::CopyAction, mimeData.get(),
                     Qt::LeftButton, Qt::NoModifier);
    // Pure pass-through: the view state must stay untouched.
    const int itemCountBefore = view->drawScene()->items().count();
    ASSERT_TRUE(view->drawScene()->selectedItems().isEmpty());
    view->dropEvent(&event);
    EXPECT_TRUE(view->drawScene()->selectedItems().isEmpty());
    EXPECT_EQ(view->drawScene()->items().count(), itemCountBefore);
}

TEST(PageViewDropEvent, NoUrls_IgnoredByScrollAreaBase)
{
    createNewViewByShortcutKey();
    ensurePageExists();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    std::unique_ptr<QMimeData> mimeData(new QMimeData);
    mimeData->setText("plain text");
    QDropEvent event(QPoint(50, 50), Qt::CopyAction, mimeData.get(),
                     Qt::LeftButton, Qt::NoModifier);
    // Pure pass-through: the view state and the mime payload must stay
    // untouched (no url-based import for a text-only drop).
    const int itemCountBefore = view->drawScene()->items().count();
    ASSERT_TRUE(view->drawScene()->selectedItems().isEmpty());
    view->dropEvent(&event);
    EXPECT_TRUE(view->drawScene()->selectedItems().isEmpty());
    EXPECT_EQ(view->drawScene()->items().count(), itemCountBefore);
    EXPECT_TRUE(mimeData->hasText());
}

// ===========================================================================
// pinchTriggered — pinch gesture zoom
// ===========================================================================

TEST(PageViewPinchTriggered, ScaleUp_NoCrash)
{
    createNewViewByShortcutKey();
    ensurePageExists();
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
    ensurePageExists();
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
    ensurePageExists();
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
