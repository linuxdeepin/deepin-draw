// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CDrawToolEvent methods (issue V-5575)
 *
 * Source: src/drawshape/drawTools/idrawtoolevent.cpp
 *
 * Target methods (not already covered by test_cdrawtoolevent_funcs.cpp):
 *   CDrawToolEvent::keyboardModifiers,
 *   isPosXAccepted/setPosXAccepted,
 *   isPosYAccepted/setPosYAccepted
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#define protected public
#define private public
#include "drawshape/drawTools/idrawtoolevent.h"
#include "drawshape/globaldefine.h"
#include "publicApi.h"
#undef protected
#undef private

#include <QApplication>

// =========================================================================
// CDrawToolEvent::keyboardModifiers
// =========================================================================

TEST(CDrawToolEventKeyboardModifiersTest, DefaultNoModifiers)
{
    CDrawToolEvent event;
    EXPECT_EQ(event.keyboardModifiers(), Qt::NoModifier);
}

TEST(CDrawToolEventKeyboardModifiersTest, ReturnsControlModifier)
{
    CDrawToolEvent event;
    event._kbMods = Qt::ControlModifier;
    EXPECT_EQ(event.keyboardModifiers(), Qt::ControlModifier);
}

TEST(CDrawToolEventKeyboardModifiersTest, ReturnsCombinedModifiers)
{
    CDrawToolEvent event;
    event._kbMods = Qt::ControlModifier | Qt::ShiftModifier;
    EXPECT_EQ(event.keyboardModifiers(),
              Qt::ControlModifier | Qt::ShiftModifier);
}

TEST(CDrawToolEventKeyboardModifiersTest, ReturnsAltModifier)
{
    CDrawToolEvent event;
    event._kbMods = Qt::AltModifier;
    EXPECT_TRUE(event.keyboardModifiers() & Qt::AltModifier);
}

// =========================================================================
// CDrawToolEvent::isPosXAccepted / setPosXAccepted
// =========================================================================

TEST(CDrawToolEventPosXAcceptedTest, SetTrueReturnsTrue)
{
    CDrawToolEvent event;
    event.setPosXAccepted(true);
    EXPECT_TRUE(event.isPosXAccepted());
}

TEST(CDrawToolEventPosXAcceptedTest, SetFalseReturnsFalse)
{
    CDrawToolEvent event;
    event.setPosXAccepted(true);
    event.setPosXAccepted(false);
    EXPECT_FALSE(event.isPosXAccepted());
}

TEST(CDrawToolEventPosXAcceptedTest, ToggleWorks)
{
    CDrawToolEvent event;
    event.setPosXAccepted(false);
    EXPECT_FALSE(event.isPosXAccepted());
    event.setPosXAccepted(true);
    EXPECT_TRUE(event.isPosXAccepted());
    event.setPosXAccepted(false);
    EXPECT_FALSE(event.isPosXAccepted());
}

// =========================================================================
// CDrawToolEvent::isPosYAccepted / setPosYAccepted
// =========================================================================

TEST(CDrawToolEventPosYAcceptedTest, SetTrueReturnsTrue)
{
    CDrawToolEvent event;
    event.setPosYAccepted(true);
    EXPECT_TRUE(event.isPosYAccepted());
}

TEST(CDrawToolEventPosYAcceptedTest, SetFalseReturnsFalse)
{
    CDrawToolEvent event;
    event.setPosYAccepted(true);
    event.setPosYAccepted(false);
    EXPECT_FALSE(event.isPosYAccepted());
}

TEST(CDrawToolEventPosYAcceptedTest, ToggleWorks)
{
    CDrawToolEvent event;
    event.setPosYAccepted(false);
    EXPECT_FALSE(event.isPosYAccepted());
    event.setPosYAccepted(true);
    EXPECT_TRUE(event.isPosYAccepted());
    event.setPosYAccepted(false);
    EXPECT_FALSE(event.isPosYAccepted());
}

TEST(CDrawToolEventPosAcceptedTest, IndependentXAndY)
{
    CDrawToolEvent event;
    event.setPosXAccepted(true);
    event.setPosYAccepted(false);
    EXPECT_TRUE(event.isPosXAccepted());
    EXPECT_FALSE(event.isPosYAccepted());

    event.setPosXAccepted(false);
    event.setPosYAccepted(true);
    EXPECT_FALSE(event.isPosXAccepted());
    EXPECT_TRUE(event.isPosYAccepted());
}
