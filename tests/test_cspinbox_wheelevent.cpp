// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CSpinBox::wheelEvent (issue V-8787)
 *
 * Source: src/widgets/cspinbox.cpp
 *
 * Target method:
 *   CSpinBox::wheelEvent(QWheelEvent *event)
 *
 * wheelEvent has two branches:
 *   - hasFocus() == true  → timerStart() + DSpinBox::wheelEvent(event)
 *   - hasFocus() == false → early return (no timer, no value change)
 *
 * Existing test_cspinbox.cpp / test_cspinbox_funcs2.cpp cover timer
 * internals, focus events, and size hints but never send a QWheelEvent.
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QWheelEvent>
#include <QApplication>
#include <QSignalSpy>
#include <QPointF>

#define protected public
#define private public
#include "widgets/cspinbox.h"
#include "drawshape/globaldefine.h"
#undef protected
#undef private

// Helper: activate the widget so setFocus works under offscreen platform.
static void activateAndFocus(CSpinBox &box)
{
    box.setFocusPolicy(Qt::StrongFocus);
    box.show();
    QApplication::setActiveWindow(&box);
    box.activateWindow();
    box.setFocus(Qt::OtherFocusReason);
    QApplication::processEvents();
}

// =========================================================================
// wheelEvent — no focus branch (early return, no timer started)
// =========================================================================

TEST(CSpinBoxWheelEvent, NoFocus_DoesNotStartTimer)
{
    CSpinBox box;
    box.setSpinRange(0, 100);
    box.setValue(50);

    // Ensure the widget is realised but NOT focused.
    box.show();
    box.clearFocus();
    QApplication::processEvents();

    // Construct a vertical wheel event.
    QWheelEvent event(QPointF(5, 5), QPointF(5, 5),
                      QPoint(0, 120), QPoint(0, 120),
                      Qt::NoButton, Qt::NoModifier, Qt::NoScrollPhase, false);

    QApplication::sendEvent(&box, &event);

    // Without focus, timerStart() is never called → timer should NOT be running.
    EXPECT_FALSE(box.isTimerRunning());
}

TEST(CSpinBoxWheelEvent, NoFocus_ValueUnchanged)
{
    CSpinBox box;
    box.setSpinRange(0, 100);
    box.setValue(50);
    box.show();
    box.clearFocus();
    QApplication::processEvents();

    int valueBefore = box.value();

    QWheelEvent event(QPointF(5, 5), QPointF(5, 5),
                      QPoint(0, 120), QPoint(0, 120),
                      Qt::NoButton, Qt::NoModifier, Qt::NoScrollPhase, false);
    QApplication::sendEvent(&box, &event);

    EXPECT_EQ(box.value(), valueBefore);
}

// =========================================================================
// wheelEvent — focused branch (timer started, value changes)
// =========================================================================

TEST(CSpinBoxWheelEvent, WithFocus_StartsTimer)
{
    CSpinBox box;
    box.setSpinRange(0, 100);
    box.setValue(50);

    activateAndFocus(box);
    if (!box.hasFocus()) {
        GTEST_SKIP() << "Cannot acquire focus under offscreen platform";
    }

    QWheelEvent event(QPointF(5, 5), QPointF(5, 5),
                      QPoint(0, 120), QPoint(0, 120),
                      Qt::NoButton, Qt::NoModifier, Qt::NoScrollPhase, false);
    QApplication::sendEvent(&box, &event);

    // With focus, timerStart() is called → timer should be running.
    EXPECT_TRUE(box.isTimerRunning());
}

TEST(CSpinBoxWheelEvent, WithFocus_EmitsValueChangedSignal)
{
    CSpinBox box;
    box.setSpinRange(0, 100);
    box.setValue(50);

    activateAndFocus(box);
    if (!box.hasFocus()) {
        GTEST_SKIP() << "Cannot acquire focus under offscreen platform";
    }

    QSignalSpy spy(&box, &CSpinBox::valueChanged);

    QWheelEvent event(QPointF(5, 5), QPointF(5, 5),
                      QPoint(0, 120), QPoint(0, 120),
                      Qt::NoButton, Qt::NoModifier, Qt::NoScrollPhase, false);
    QApplication::sendEvent(&box, &event);

    // DSpinBox::wheelEvent should change the value (up by singleStep=1)
    // and timerStart emits EChangedBegin.
    EXPECT_GE(spy.count(), 1);
}

TEST(CSpinBoxWheelEvent, WithFocus_NegativeDelta_DecreasesValue)
{
    CSpinBox box;
    box.setSpinRange(0, 100);
    box.setValue(50);

    activateAndFocus(box);
    if (!box.hasFocus()) {
        GTEST_SKIP() << "Cannot acquire focus under offscreen platform";
    }

    int valueBefore = box.value();

    QWheelEvent event(QPointF(5, 5), QPointF(5, 5),
                      QPoint(0, -120), QPoint(0, -120),
                      Qt::NoButton, Qt::NoModifier, Qt::NoScrollPhase, false);
    QApplication::sendEvent(&box, &event);

    // Negative wheel delta should decrease the value.
    EXPECT_LT(box.value(), valueBefore);
}
