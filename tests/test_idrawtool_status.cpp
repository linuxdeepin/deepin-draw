// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for IDrawTool methods (issue V-5645).
 *
 * Source: src/drawshape/drawTools/idrawtool.cpp
 *
 * Target methods:
 *   isWorking, interrupt, changeStatusFlagTo, getCurVaildActivedPointCount
 *   (getTimerForDoubleCliked already covered in testItems/test_idrawtool.cpp)
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QTimer>
#include <QSignalSpy>

#define protected public
#define private public
#include "drawshape/drawTools/idrawtool.h"
#include "drawshape/globaldefine.h"
#undef protected
#undef private

// =========================================================================
// IDrawTool::isWorking
// =========================================================================

TEST(IDrawToolIsWorking, EmptyRecords_ReturnsFalse)
{
    IDrawTool tool(rectangle);
    EXPECT_FALSE(tool.isWorking());
}

TEST(IDrawToolIsWorking, WithRecords_ReturnsTrue)
{
    IDrawTool tool(rectangle);
    tool._allITERecordInfo.insert(0, IDrawTool::ITERecordInfo());
    EXPECT_TRUE(tool.isWorking());
}

TEST(IDrawToolIsWorking, AfterClearITE_ReturnsFalse)
{
    IDrawTool tool(rectangle);
    tool._allITERecordInfo.insert(0, IDrawTool::ITERecordInfo());
    EXPECT_TRUE(tool.isWorking());

    tool.clearITE();
    EXPECT_FALSE(tool.isWorking());
}

// =========================================================================
// IDrawTool::interrupt
// =========================================================================

TEST(IDrawToolInterrupt, ClearsRecords)
{
    IDrawTool tool(rectangle);
    tool._allITERecordInfo.insert(0, IDrawTool::ITERecordInfo());
    tool._allITERecordInfo.insert(1, IDrawTool::ITERecordInfo());
    ASSERT_TRUE(tool.isWorking());

    tool.interrupt();

    EXPECT_FALSE(tool.isWorking());
}

TEST(IDrawToolInterrupt, WhenWorking_StatusChangesToReady)
{
    IDrawTool tool(rectangle);
    tool._status = IDrawTool::EWorking;

    tool.interrupt();

    EXPECT_EQ(tool._status, IDrawTool::EReady);
}

TEST(IDrawToolInterrupt, WhenNotWorking_StatusUnchanged)
{
    IDrawTool tool(rectangle);
    tool._status = IDrawTool::EIdle;

    tool.interrupt();

    EXPECT_EQ(tool._status, IDrawTool::EIdle);
}

// =========================================================================
// IDrawTool::changeStatusFlagTo
// =========================================================================

TEST(IDrawToolChangeStatusFlagTo, NewStatus_EmitsSignal)
{
    IDrawTool tool(rectangle);
    tool._status = IDrawTool::EIdle;

    QSignalSpy spy(&tool, &IDrawTool::statusChanged);
    tool.changeStatusFlagTo(IDrawTool::EWorking);

    EXPECT_EQ(tool._status, IDrawTool::EWorking);
    EXPECT_EQ(spy.count(), 1);
    auto args = spy.takeFirst();
    EXPECT_EQ(args.at(0).value<IDrawTool::EStatus>(), IDrawTool::EIdle);
    EXPECT_EQ(args.at(1).value<IDrawTool::EStatus>(), IDrawTool::EWorking);
}

TEST(IDrawToolChangeStatusFlagTo, SameStatus_NoSignal)
{
    IDrawTool tool(rectangle);
    tool._status = IDrawTool::EIdle;

    QSignalSpy spy(&tool, &IDrawTool::statusChanged);
    tool.changeStatusFlagTo(IDrawTool::EIdle);

    EXPECT_EQ(tool._status, IDrawTool::EIdle);
    EXPECT_EQ(spy.count(), 0);
}

TEST(IDrawToolChangeStatusFlagTo, MultipleChanges_EmitsEachTime)
{
    IDrawTool tool(rectangle);
    tool._status = IDrawTool::EIdle;

    QSignalSpy spy(&tool, &IDrawTool::statusChanged);
    tool.changeStatusFlagTo(IDrawTool::EReady);
    tool.changeStatusFlagTo(IDrawTool::EWorking);
    tool.changeStatusFlagTo(IDrawTool::EReady);

    EXPECT_EQ(tool._status, IDrawTool::EReady);
    EXPECT_EQ(spy.count(), 3);
}

// =========================================================================
// IDrawTool::getCurVaildActivedPointCount
// =========================================================================

TEST(IDrawToolGetCurVaildActivedPointCount, EmptyRecords_ReturnsZero)
{
    IDrawTool tool(rectangle);
    EXPECT_EQ(tool.getCurVaildActivedPointCount(), 0);
}

TEST(IDrawToolGetCurVaildActivedPointCount, AllActive_ReturnsAllCount)
{
    IDrawTool tool(rectangle);

    IDrawTool::ITERecordInfo info1;
    info1.eventLife = IDrawTool::ENormal;
    IDrawTool::ITERecordInfo info2;
    info2.eventLife = IDrawTool::ENormal;

    tool._allITERecordInfo.insert(0, info1);
    tool._allITERecordInfo.insert(1, info2);

    EXPECT_EQ(tool.getCurVaildActivedPointCount(), 2);
}

TEST(IDrawToolGetCurVaildActivedPointCount, MixedActiveAndInactive_ReturnsOnlyActive)
{
    IDrawTool tool(rectangle);

    IDrawTool::ITERecordInfo info1;
    info1.eventLife = IDrawTool::ENormal;
    IDrawTool::ITERecordInfo info2;
    info2.eventLife = IDrawTool::EDoNotthing;
    IDrawTool::ITERecordInfo info3;
    info3.eventLife = IDrawTool::EDoQtCoversion;

    tool._allITERecordInfo.insert(0, info1);
    tool._allITERecordInfo.insert(1, info2);
    tool._allITERecordInfo.insert(2, info3);

    // EDoNotthing is excluded, ENormal and EDoQtCoversion are counted
    EXPECT_EQ(tool.getCurVaildActivedPointCount(), 2);
}
