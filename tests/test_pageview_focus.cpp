// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for PageView focus, proxy widgets & viewport event
 * (src/frame/cgraphicsview.cpp).
 *
 * Covers (issue V-5981 §六):
 *   - captureFocus(force) — view gains focus
 *   - activeProxWidget / activeProxItem / activeProxDrawItem — null when
 *     no text editing active
 *   - viewportEvent — hover/tooltip event forwarding, no crash
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QEvent>
#include <QHoverEvent>
#include <QToolTip>

#define protected public
#define private public
#include "publicApi.h"
#undef protected
#undef private

// ---------------------------------------------------------------------------
// captureFocus
// ---------------------------------------------------------------------------

TEST(PageViewFocusTest, CaptureFocus_Default_ViewHasFocus)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    view->captureFocus();
    qApp->processEvents();
    EXPECT_TRUE(view->hasFocus());
}

TEST(PageViewFocusTest, CaptureFocus_ForceTrue_NoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    view->captureFocus(true);
    qApp->processEvents();
    SUCCEED();
}

TEST(PageViewFocusTest, CaptureFocus_ForceFalse_NoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    view->captureFocus(false);
    qApp->processEvents();
    SUCCEED();
}

// ---------------------------------------------------------------------------
// activeProxWidget / activeProxItem / activeProxDrawItem
// ---------------------------------------------------------------------------

TEST(PageViewProxyTest, ActiveProxWidget_NullWhenNotEditing)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    // No text editing active → should return nullptr.
    EXPECT_EQ(view->activeProxWidget(), nullptr);
}

TEST(PageViewProxyTest, ActiveProxItem_NullWhenNotEditing)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    EXPECT_EQ(view->activeProxItem(), nullptr);
}

TEST(PageViewProxyTest, ActiveProxDrawItem_NullWhenNotEditing)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    EXPECT_EQ(view->activeProxDrawItem(), nullptr);
}

// ---------------------------------------------------------------------------
// viewportEvent
// ---------------------------------------------------------------------------

TEST(PageViewViewportEventTest, HoverEvent_NoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    QHoverEvent hoverEnter(QEvent::HoverEnter, QPointF(10, 10), QPointF(0, 0));
    bool result = view->viewportEvent(&hoverEnter);
    (void)result; // just verify no crash
    SUCCEED();
}

TEST(PageViewViewportEventTest, ToolTipEvent_NoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    QHelpEvent tooltip(QEvent::ToolTip, QPoint(50, 50),
                       view->viewport()->mapToGlobal(QPoint(50, 50)));
    bool result = view->viewportEvent(&tooltip);
    (void)result;
    SUCCEED();
}
