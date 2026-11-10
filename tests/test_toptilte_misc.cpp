// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for TopTilte destructor and enterEvent (issue V-8176)
 *
 * Source: src/frame/toptoolbar.cpp / .h
 *
 * Target methods (0-coverage per existing .gcda):
 *   TopTilte::~TopTilte()
 *   TopTilte::enterEvent(QEvent*)        [Qt5]
 *   TopTilte::enterEvent(QEnterEvent*)   [Qt6]
 *
 * enterEvent is a protected override; we deliver a version-appropriate
 * Enter event (QEnterEvent under Qt6, plain QEvent under Qt5) through
 * QCoreApplication::sendEvent() / notify(), which invoke the virtual
 * event() chain and reach enterEvent without direct access to the
 * protected method.
 */

#include <gtest/gtest.h>

#include <memory>

#include <QCoreApplication>
#include <QEvent>

#if (QT_VERSION_MAJOR == 6)
#include <QEnterEvent>
#include <QPointingDevice>
#endif

// publicApi.h already includes toptoolbar.h (applying its own
// access-adjusting macros internally), so no extra includes are needed.
#include "publicApi.h"

// =========================================================================
// TopTilte::enterEvent (via sendEvent / notify)
// =========================================================================

// Build a stack-allocated Enter event matching the active Qt version.
// - sendEvent() does NOT take ownership of the event (unlike postEvent()),
//   so the event must never be heap-allocated here (would leak).
// - Qt5 widgets override enterEvent(QEvent*); Qt6 overrides
//   enterEvent(QEnterEvent*) and QWidget::event() static_casts Enter
//   events to QEnterEvent*, so the concrete type must match the version.
#if (QT_VERSION_MAJOR == 6)
#define MAKE_ENTER_EVENT(var) QEnterEvent var(QPointF(5, 5), QPointF(5, 5), QPointF(5, 5))
#else
#define MAKE_ENTER_EVENT(var) QEvent var(QEvent::Enter)
#endif

TEST(TopTilteMisc, EnterEventNoCrash)
{
    // MainWindow is created in QTestMain constructor; access TopTilte directly
    MainWindow *mw = drawApp->topMainWindow();
    ASSERT_NE(mw, nullptr);

    TopTilte *tt = mw->topTitle();
    ASSERT_NE(tt, nullptr);

    // Deliver an Enter event through the event system; this reaches the
    // protected enterEvent override via virtual dispatch in event().
    MAKE_ENTER_EVENT(enterEvent);
    bool accepted = QCoreApplication::sendEvent(tt, &enterEvent);
    // sendEvent returns true if the event was handled
    EXPECT_TRUE(accepted);
}

TEST(TopTilteMisc, EnterEventMultipleCallsNoCrash)
{
    MainWindow *mw = drawApp->topMainWindow();
    ASSERT_NE(mw, nullptr);

    TopTilte *tt = mw->topTitle();
    ASSERT_NE(tt, nullptr);

    for (int i = 0; i < 5; ++i) {
        MAKE_ENTER_EVENT(enterEvent);
        QCoreApplication::sendEvent(tt, &enterEvent);
    }
    SUCCEED();
}

TEST(TopTilteMisc, EnterEventViaNotifyNoCrash)
{
    MainWindow *mw = drawApp->topMainWindow();
    ASSERT_NE(mw, nullptr);

    TopTilte *tt = mw->topTitle();
    ASSERT_NE(tt, nullptr);

    // notify() dispatches through the event() virtual chain, same as
    // sendEvent(), but routes through the application object.
    MAKE_ENTER_EVENT(enterEvent);
    QCoreApplication::instance()->notify(tt, &enterEvent);
    SUCCEED();
}

// =========================================================================
// TopTilte::~TopTilte()
// =========================================================================

TEST(TopTilteMisc, DestructorNoCrash)
{
    TopTilte tt;
    // ~TopTilte() runs when tt goes out of scope
    SUCCEED();
}

TEST(TopTilteMisc, DestructorWithParentNoCrash)
{
    QWidget parent;
    TopTilte tt(&parent);
    // ~TopTilte() runs first (reverse declaration order), removing itself
    // from parent's children before the parent is destroyed
    SUCCEED();
}

TEST(TopTilteMisc, DestructorChildDeletedWithParent)
{
    // Heap allocation of the child is required: this test verifies Qt's
    // parent-child ownership, where deleting the parent deletes the child
    // TopTilte and thus invokes ~TopTilte(). The child stays owned by the
    // parent and must not be deleted again; the parent's lifetime is
    // managed by std::unique_ptr.
    std::unique_ptr<QWidget> parent = std::make_unique<QWidget>();
    TopTilte *tt = new TopTilte(parent.get());
    ASSERT_NE(tt, nullptr);
    parent.reset();  // ~QWidget() deletes the child TopTilte
    SUCCEED();
}
