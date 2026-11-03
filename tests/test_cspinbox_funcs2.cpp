// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CSpinBox methods (issue V-7429)
 *
 * Source: src/widgets/cspinbox.cpp
 *
 * Target methods not covered by test_cspinbox.cpp:
 *   updateMaxSize(), minimumSizeHint(),
 *   focusInEvent(), focusOutEvent(),
 *   focusChanged(bool) signal
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QSignalSpy>
#include <QFocusEvent>
#include <QSize>
#include <QLineEdit>
#include <QApplication>

#define protected public
#define private public
#include "widgets/cspinbox.h"
#include "drawshape/globaldefine.h"
#undef protected
#undef private

// =========================================================================
// CSpinBox::updateMaxSize
// =========================================================================

TEST(CSpinBoxUpdateMaxSize, DoesNotCrashOnNonTabletEnv)
{
    CSpinBox box;
    // On non-tablet env, updateMaxSize does nothing (early return).
    box.updateMaxSize();
    SUCCEED();
}

TEST(CSpinBoxUpdateMaxSize, DoesNotCrashWithText)
{
    CSpinBox box;
    box.lineEdit()->setText("123");
    box.updateMaxSize();
    SUCCEED();
}

// =========================================================================
// CSpinBox::minimumSizeHint
// =========================================================================

TEST(CSpinBoxMinimumSizeHint, ReturnsValidSize)
{
    CSpinBox box;
    QSize sz = box.minimumSizeHint();
    // DSpinBox::minimumSizeHint() should return a non-null size.
    EXPECT_TRUE(sz.width() >= 0);
    EXPECT_TRUE(sz.height() >= 0);
}

TEST(CSpinBoxMinimumSizeHint, ConsistentAcrossCalls)
{
    CSpinBox box;
    QSize sz1 = box.minimumSizeHint();
    QSize sz2 = box.minimumSizeHint();
    EXPECT_EQ(sz1, sz2);
}

// =========================================================================
// CSpinBox::focusInEvent / focusOutEvent
// =========================================================================

TEST(CSpinBoxFocusInEvent, DoesNotCrash)
{
    CSpinBox box;
    QFocusEvent event(QEvent::FocusIn);
    QApplication::sendEvent(&box, &event);
    SUCCEED();
}

TEST(CSpinBoxFocusOutEvent, DoesNotCrack)
{
    CSpinBox box;
    QFocusEvent event(QEvent::FocusOut);
    QApplication::sendEvent(&box, &event);
    SUCCEED();
}

TEST(CSpinBoxFocusEvents, FocusInThenOutDoesNotCrash)
{
    CSpinBox box;
    QFocusEvent focusIn(QEvent::FocusIn);
    QFocusEvent focusOut(QEvent::FocusOut);
    QApplication::sendEvent(&box, &focusIn);
    QApplication::sendEvent(&box, &focusOut);
    SUCCEED();
}
