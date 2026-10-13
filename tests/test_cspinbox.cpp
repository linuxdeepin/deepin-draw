// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CSpinBox methods (issue V-5645).
 *
 * Source: src/widgets/cspinbox.cpp
 *
 * Target methods:
 *   isTimerRunning, timerStart, timerEnd, getTimer, setSpinRange,
 *   setSpecialText, setSpinPhaseValue, isChangedByWheelEnd,
 *   setValueChangedKeepFocus
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QTimer>
#include <QSignalSpy>
#include <limits>

#define protected public
#define private public
#include "widgets/cspinbox.h"
#include "drawshape/globaldefine.h"
#undef protected
#undef private

// =========================================================================
// CSpinBox::isTimerRunning
// =========================================================================

TEST(CSpinBoxIsTimerRunning, NoTimer_ReturnsFalse)
{
    CSpinBox box;
    EXPECT_FALSE(box.isTimerRunning());
}

TEST(CSpinBoxIsTimerRunning, TimerStarted_ReturnsTrue)
{
    CSpinBox box;
    box.timerStart();
    EXPECT_TRUE(box.isTimerRunning());
}

TEST(CSpinBoxIsTimerRunning, TimerEnded_ReturnsFalse)
{
    CSpinBox box;
    box.timerStart();
    box.timerEnd();
    // timerEnd deletes _wheelTimer via deleteLater
    // Process events so deleteLater takes effect
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    EXPECT_FALSE(box.isTimerRunning());
}

// =========================================================================
// CSpinBox::getTimer
// =========================================================================

TEST(CSpinBoxGetTimer, FirstCall_CreatesTimer)
{
    CSpinBox box;
    ASSERT_EQ(box._wheelTimer, nullptr);

    QTimer *t = box.getTimer();

    ASSERT_NE(t, nullptr);
    EXPECT_TRUE(t->isSingleShot());
    EXPECT_EQ(box._wheelTimer, t);
}

TEST(CSpinBoxGetTimer, SecondCall_ReturnsSameTimer)
{
    CSpinBox box;

    QTimer *t1 = box.getTimer();
    QTimer *t2 = box.getTimer();

    EXPECT_EQ(t1, t2);
}

// =========================================================================
// CSpinBox::timerStart
// =========================================================================

TEST(CSpinBoxTimerStart, StartsTimer)
{
    CSpinBox box;
    ASSERT_FALSE(box.isTimerRunning());

    box.timerStart();

    EXPECT_TRUE(box.isTimerRunning());
}

TEST(CSpinBoxTimerStart, WhenNotRunning_SetsBeginPhase)
{
    CSpinBox box;
    box.setSpinRange(0, 100);
    box.setValue(50);

    QSignalSpy spy(&box, &CSpinBox::valueChanged);

    box.timerStart();

    // timerStart calls setSpinPhaseValue with EChangedBegin when not running
    ASSERT_GE(spy.count(), 1);
    auto args = spy.takeFirst();
    EXPECT_EQ(args.at(1).value<EChangedPhase>(), EChangedBegin);
}

TEST(CSpinBoxTimerStart, WhenAlreadyRunning_NoExtraBeginPhase)
{
    CSpinBox box;
    box.setSpinRange(0, 100);
    box.setValue(50);

    box.timerStart();

    QSignalSpy spy(&box, &CSpinBox::valueChanged);
    box.timerStart();

    // Should not emit EChangedBegin again since timer is already running
    bool foundBegin = false;
    for (const auto &args : spy) {
        if (args.at(1).value<EChangedPhase>() == EChangedBegin) {
            foundBegin = true;
            break;
        }
    }
    EXPECT_FALSE(foundBegin);
}

// =========================================================================
// CSpinBox::timerEnd
// =========================================================================

TEST(CSpinBoxTimerEnd, SetsWheelEndFlag)
{
    CSpinBox box;
    box.setSpinRange(0, 100);
    box.setValue(50);
    box.timerStart();

    box.timerEnd();

    // _wheelEnd is set to true then false within timerEnd
    // After timerEnd, _wheelEnd should be false
    EXPECT_FALSE(box._wheelEnd);
}

TEST(CSpinBoxTimerEnd, EmitsFinishedPhase)
{
    CSpinBox box;
    box.setSpinRange(0, 100);
    box.setValue(50);
    box.timerStart();

    QSignalSpy spy(&box, &CSpinBox::valueChanged);
    box.timerEnd();

    bool foundFinished = false;
    for (const auto &args : spy) {
        if (args.at(1).value<EChangedPhase>() == EChangedFinished) {
            foundFinished = true;
            break;
        }
    }
    EXPECT_TRUE(foundFinished);
}

// =========================================================================
// CSpinBox::setSpinRange
// =========================================================================

TEST(CSpinBoxSetSpinRange, SetsMinAndMax)
{
    CSpinBox box;

    box.setSpinRange(10, 200);

    EXPECT_EQ(box.m_min, 10);
    EXPECT_EQ(box.m_max, 200);
}

TEST(CSpinBoxSetSpinRange, OverwritePreviousRange)
{
    CSpinBox box;
    box.setSpinRange(0, 100);

    box.setSpinRange(-50, 500);

    EXPECT_EQ(box.m_min, -50);
    EXPECT_EQ(box.m_max, 500);
}

// =========================================================================
// CSpinBox::setSpecialText
// =========================================================================

TEST(CSpinBoxSetSpecialText, SetsSpecialValueText)
{
    CSpinBox box;

    box.setSpecialText("Custom");

    EXPECT_EQ(box.specialValueText(), QString("Custom"));
}

TEST(CSpinBoxSetSpecialText, DefaultText)
{
    CSpinBox box;

    box.setSpecialText();

    EXPECT_EQ(box.specialValueText(), QString::fromUtf8("— —"));
}

// =========================================================================
// CSpinBox::setSpinPhaseValue
// =========================================================================

TEST(CSpinBoxSetSpinPhaseValue, ValidValue_EmitsSignal)
{
    CSpinBox box;
    box.setSpinRange(0, 100);

    QSignalSpy spy(&box, &CSpinBox::valueChanged);

    box.setSpinPhaseValue(50, EChangedUpdate);

    ASSERT_EQ(spy.count(), 1);
    auto args = spy.takeFirst();
    EXPECT_EQ(args.at(0).toInt(), 50);
    EXPECT_EQ(args.at(1).value<EChangedPhase>(), EChangedUpdate);
}

TEST(CSpinBoxSetSpinPhaseValue, ValueBelowMin_ClampedToMin)
{
    CSpinBox box;
    box.setSpinRange(10, 100);

    QSignalSpy spy(&box, &CSpinBox::valueChanged);

    box.setSpinPhaseValue(-5, EChanged);

    ASSERT_EQ(spy.count(), 1);
    auto args = spy.takeFirst();
    EXPECT_EQ(args.at(0).toInt(), 10);
}

TEST(CSpinBoxSetSpinPhaseValue, ValueAboveMax_ClampedToMax)
{
    CSpinBox box;
    box.setSpinRange(0, 100);

    QSignalSpy spy(&box, &CSpinBox::valueChanged);

    box.setSpinPhaseValue(200, EChanged);

    ASSERT_EQ(spy.count(), 1);
    auto args = spy.takeFirst();
    EXPECT_EQ(args.at(0).toInt(), 100);
}

TEST(CSpinBoxSetSpinPhaseValue, SameValueAndPhase_NoSignal)
{
    CSpinBox box;
    box.setSpinRange(0, 100);
    box.setSpinPhaseValue(50, EChanged);

    QSignalSpy spy(&box, &CSpinBox::valueChanged);
    box.setSpinPhaseValue(50, EChanged);

    EXPECT_EQ(spy.count(), 0);
}

// =========================================================================
// CSpinBox::isChangedByWheelEnd
// =========================================================================

TEST(CSpinBoxIsChangedByWheelEnd, DefaultFalse)
{
    CSpinBox box;
    EXPECT_FALSE(box.isChangedByWheelEnd());
}

TEST(CSpinBoxIsChangedByWheelEnd, TrueDuringTimerEnd)
{
    CSpinBox box;
    box.setSpinRange(0, 100);
    box.setValue(50);
    box.timerStart();

    // During timerEnd, _wheelEnd is set to true then false
    box._wheelEnd = true;
    EXPECT_TRUE(box.isChangedByWheelEnd());

    box._wheelEnd = false;
    EXPECT_FALSE(box.isChangedByWheelEnd());
}

// =========================================================================
// CSpinBox::setValueChangedKeepFocus
// =========================================================================

TEST(CSpinBoxSetValueChangedKeepFocus, SetTrue)
{
    CSpinBox box;
    box.setValueChangedKeepFocus(true);
    EXPECT_TRUE(box._keepFocus);
}

TEST(CSpinBoxSetValueChangedKeepFocus, SetFalse)
{
    CSpinBox box;
    box._keepFocus = true;
    box.setValueChangedKeepFocus(false);
    EXPECT_FALSE(box._keepFocus);
}
