// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CGraphicsView/PageView signals (issue V-7618 group 12)
 *
 * Source: src/frame/cgraphicsview.h/.cpp
 *
 * Target signals:
 *   signalSetScale(const qreal scale)
 *   signalTransmitContinueDoOtherThing()
 *   singalTransmitEndLoadDDF()
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QSignalSpy>

#define protected public
#define private public
#include "cgraphicsview.h"
#include "publicApi.h"
#undef protected
#undef private

// =========================================================================
// PageView signals
// =========================================================================

TEST(PageViewSignals, signalSetScale_IsValid)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    QSignalSpy spy(view, &PageView::signalSetScale);
    ASSERT_TRUE(spy.isValid());
    SUCCEED();
}

TEST(PageViewSignals, signalTransmitContinueDoOtherThing_IsValid)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    QSignalSpy spy(view, &PageView::signalTransmitContinueDoOtherThing);
    ASSERT_TRUE(spy.isValid());
    SUCCEED();
}

TEST(PageViewSignals, singalTransmitEndLoadDDF_IsValid)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    QSignalSpy spy(view, &PageView::singalTransmitEndLoadDDF);
    ASSERT_TRUE(spy.isValid());
    SUCCEED();
}

TEST(PageViewSignals, ScaleChange_EmitsSignalSetScale)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    QSignalSpy spy(view, &PageView::signalSetScale);
    ASSERT_TRUE(spy.isValid());

    // Set scale should trigger the signal
    view->scale(2.0);

    // PageView::scale emits signalSetScale(m_scale) exactly once
    ASSERT_EQ(spy.count(), 1);
    QList<QVariant> args = spy.takeFirst();
    EXPECT_EQ(args.size(), 1);
    EXPECT_NEAR(args.at(0).toReal(), 2.0, 0.01);
}

TEST(PageViewSignals, EmitSignalTransmitContinueDoOtherThing_NoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    QSignalSpy spy(view, &PageView::signalTransmitContinueDoOtherThing);
    ASSERT_TRUE(spy.isValid());

    emit view->signalTransmitContinueDoOtherThing();

    EXPECT_EQ(spy.count(), 1);
}

TEST(PageViewSignals, EmitSingalTransmitEndLoadDDF_NoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    QSignalSpy spy(view, &PageView::singalTransmitEndLoadDDF);
    ASSERT_TRUE(spy.isValid());

    emit view->singalTransmitEndLoadDDF();

    EXPECT_EQ(spy.count(), 1);
}
