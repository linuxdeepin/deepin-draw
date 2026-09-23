// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QWheelEvent>

#define protected public
#define private public
#include "publicApi.h"
#include "cgraphicsview.h"

/*
 * PageView::wheelEvent (1478 lines, complexity 118)
 *
 * A very large zoom/scroll handler. We perform a smoke test: construct a
 * basic vertical-scroll QWheelEvent and deliver it to the running view.
 * No crash = pass. This exercises the entry path and at least the initial
 * branch dispatch without attempting to cover all 118 complexity paths.
 */

TEST(PageViewWheelEventTest, VerticalScrollNoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    QWheelEvent event(QPointF(100, 100), QPointF(100, 100),
                      QPoint(0, 0), QPoint(0, 120),
                      Qt::NoButton, Qt::NoModifier, Qt::NoScrollPhase, false);
    view->wheelEvent(&event);
    SUCCEED();
}

TEST(PageViewWheelEventTest, CtrlModifierZoomNoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    // Ctrl+wheel is the typical zoom gesture.
    QWheelEvent event(QPointF(50, 50), QPointF(50, 50),
                      QPoint(0, 0), QPoint(0, 120),
                      Qt::NoButton, Qt::ControlModifier, Qt::NoScrollPhase, false);
    view->wheelEvent(&event);
    SUCCEED();
}
