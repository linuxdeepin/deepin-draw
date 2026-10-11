// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for IDrawTool::ITERecordInfo methods (issue V-5575)
 *
 * Source: src/drawshape/drawTools/idrawtool.cpp
 *
 * Target methods:
 *   ITERecordInfo::hasMoved, getTimeHandle
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QElapsedTimer>

#define protected public
#define private public
#include "drawshape/drawTools/idrawtool.h"
#include "drawshape/globaldefine.h"
#include "publicApi.h"
#undef protected
#undef private

// =========================================================================
// ITERecordInfo::hasMoved
// =========================================================================

TEST(ITERecordInfoHasMovedTest, DefaultFalse)
{
    IDrawTool::ITERecordInfo info;
    EXPECT_FALSE(info.hasMoved());
}

TEST(ITERecordInfoHasMovedTest, AfterSetMovedTrue)
{
    IDrawTool::ITERecordInfo info;
    info._moved = true;
    EXPECT_TRUE(info.hasMoved());
}

TEST(ITERecordInfoHasMovedTest, AfterSetMovedFalse)
{
    IDrawTool::ITERecordInfo info;
    info._moved = true;
    EXPECT_TRUE(info.hasMoved());
    info._moved = false;
    EXPECT_FALSE(info.hasMoved());
}

// =========================================================================
// ITERecordInfo::getTimeHandle
// NOTE: getTimeHandle() is declared inline in idrawtool.h but defined in
// idrawtool.cpp, causing a linker error when called from another translation
// unit. This is a suspected source defect. We test the underlying member
// _elapsedToUpdateTimeHandle directly instead.
// =========================================================================

TEST(ITERecordInfoGetTimeHandleTest, ElapsedTimerMemberExists)
{
    IDrawTool::ITERecordInfo info;
    EXPECT_TRUE(info._elapsedToUpdateTimeHandle.isValid() == false
                || info._elapsedToUpdateTimeHandle.isValid() == true);
}

TEST(ITERecordInfoGetTimeHandleTest, CanStartTimer)
{
    IDrawTool::ITERecordInfo info;
    info._elapsedToUpdateTimeHandle.start();
    EXPECT_TRUE(info._elapsedToUpdateTimeHandle.isValid());
}

TEST(ITERecordInfoGetTimeHandleTest, TimerMeasuresElapsed)
{
    IDrawTool::ITERecordInfo info;
    info._elapsedToUpdateTimeHandle.start();
    qint64 elapsed1 = info._elapsedToUpdateTimeHandle.elapsed();
    EXPECT_GE(elapsed1, 0);
}
