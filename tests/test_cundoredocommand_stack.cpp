// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CUndoRedoCommand stack methods (issue V-5575)
 *
 * Source: src/frame/cundoredocommand.cpp
 *
 * Target methods (not already covered by test_cundoredocommand_funcs.cpp):
 *   CUndoRedoCommand::pushStack, isRecordEmpty,
 *   setBlockRedoWhenPushedToStack,
 *   setUndoRedoStack/getUndoRedoStack, getCmdByCmdInfo
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QUndoStack>

#define protected public
#define private public
#include "frame/cundoredocommand.h"
#include "drawshape/globaldefine.h"
#include "drawshape/sitemdata.h"
#include "cgraphicsrectitem.h"
#include "publicApi.h"
#undef protected
#undef private

// =========================================================================
// CUndoRedoCommand::isRecordEmpty
// =========================================================================

TEST(CUndoRedoCommandIsRecordEmptyTest, EmptyWhenListCleared)
{
    CUndoRedoCommand::s_recordedCmdInfoList.clear();
    EXPECT_TRUE(CUndoRedoCommand::isRecordEmpty());
}

TEST(CUndoRedoCommandIsRecordEmptyTest, NotEmptyWhenListHasEntries)
{
    CUndoRedoCommand::s_recordedCmdInfoList.clear();

    QList<QVariant> datas;
    datas << static_cast<long long>(0x1234);
    CUndoRedoCommand::recordUndoCommand(
        CUndoRedoCommand::EItemChangedCmd,
        CItemUndoRedoCommand::EAllChanged, datas);

    EXPECT_FALSE(CUndoRedoCommand::isRecordEmpty());

    // Cleanup
    CUndoRedoCommand::s_recordedCmdInfoList.clear();
}

// =========================================================================
// CUndoRedoCommand::setBlockRedoWhenPushedToStack
// =========================================================================

TEST(CUndoRedoCommandSetBlockRedoTest, SetTrue)
{
    CUndoRedoCommand::setBlockRedoWhenPushedToStack(true);
    EXPECT_TRUE(CUndoRedoCommand::s_blockRedoWhenPushToStack);
}

TEST(CUndoRedoCommandSetBlockRedoTest, SetFalse)
{
    CUndoRedoCommand::setBlockRedoWhenPushedToStack(false);
    EXPECT_FALSE(CUndoRedoCommand::s_blockRedoWhenPushToStack);
}

TEST(CUndoRedoCommandSetBlockRedoTest, Toggle)
{
    CUndoRedoCommand::setBlockRedoWhenPushedToStack(false);
    EXPECT_FALSE(CUndoRedoCommand::s_blockRedoWhenPushToStack);
    CUndoRedoCommand::setBlockRedoWhenPushedToStack(true);
    EXPECT_TRUE(CUndoRedoCommand::s_blockRedoWhenPushToStack);
    CUndoRedoCommand::setBlockRedoWhenPushedToStack(false);
    EXPECT_FALSE(CUndoRedoCommand::s_blockRedoWhenPushToStack);
}

// =========================================================================
// CUndoRedoCommand::setUndoRedoStack / getUndoRedoStack
// =========================================================================

TEST(CUndoRedoCommandStackTest, SetAndGetUndoRedoStack)
{
    QUndoStack *stack = new QUndoStack();
    void *dummyPtr = reinterpret_cast<void *>(0xABCD1234);

    CUndoRedoCommand::setUndoRedoStack(stack, dummyPtr);
    EXPECT_EQ(CUndoRedoCommand::getUndoRedoStack(dummyPtr), stack);

    delete stack;
    // Clear the dangling entry so later tests never see a stale pointer.
    CUndoRedoCommand::setUndoRedoStack(nullptr, dummyPtr);
}

TEST(CUndoRedoCommandStackTest, GetStackForUnknownPtrReturnsNull)
{
    void *unknownPtr = reinterpret_cast<void *>(0xDEADBEEF);
    // An unregistered pointer should return a default-constructed
    // (null) QUndoStack* from the map.
    EXPECT_EQ(CUndoRedoCommand::getUndoRedoStack(unknownPtr), nullptr);
}

// =========================================================================
// CUndoRedoCommand::pushStack (needs a view)
// =========================================================================

TEST(CUndoRedoCommandPushStackTest, PushesCommandWithoutCrash)
{
    createNewViewByShortcutKey();
    auto *view = getCurView();
    ASSERT_NE(view, nullptr);

    QUndoStack *stack = CUndoRedoCommand::getUndoRedoStack(view);
    ASSERT_NE(stack, nullptr);
    const int countBefore = stack->count();

    auto *cmd = new CSceneUndoRedoCommand();
    CUndoRedoCommand::pushStack(cmd);
    // The command must actually have been added to the view's undo stack.
    ASSERT_EQ(stack->count(), countBefore + 1);
    EXPECT_EQ(stack->command(stack->count() - 1), cmd);
}

// =========================================================================
// CUndoRedoCommand::getCmdByCmdInfo
// =========================================================================

TEST(CUndoRedoCommandGetCmdByCmdInfoTest, SceneChangedCmdReturnsNonNull)
{
    CUndoRedoCommand::SCommandInfoCouple info;
    info.undoInfo.tp = CUndoRedoCommand::ESceneChangedCmd;
    info.undoInfo.expTp = CSceneUndoRedoCommand::ESizeChanged;
    info.redoInfo = info.undoInfo;

    auto *cmd = CUndoRedoCommand::getCmdByCmdInfo(info);
    ASSERT_NE(cmd, nullptr);
    // The scene-changed info must produce a CSceneUndoRedoCommand.
    EXPECT_NE(dynamic_cast<CSceneUndoRedoCommand *>(cmd), nullptr);
    delete cmd;
}

TEST(CUndoRedoCommandGetCmdByCmdInfoTest, ItemChangedCmdReturnsNonNull)
{
    CUndoRedoCommand::SCommandInfoCouple info;
    info.undoInfo.tp = CUndoRedoCommand::EItemChangedCmd;
    info.undoInfo.expTp = CItemUndoRedoCommand::EAllChanged;
    info.redoInfo = info.undoInfo;

    auto *cmd = CUndoRedoCommand::getCmdByCmdInfo(info);
    ASSERT_NE(cmd, nullptr);
    // The item-changed info must produce a CItemUndoRedoCommand.
    EXPECT_NE(dynamic_cast<CItemUndoRedoCommand *>(cmd), nullptr);
    delete cmd;
}

TEST(CUndoRedoCommandGetCmdByCmdInfoTest, UnknownCmdReturnsNull)
{
    CUndoRedoCommand::SCommandInfoCouple info;
    info.undoInfo.tp = CUndoRedoCommand::EDrawCmdUnknowed;
    info.redoInfo = info.undoInfo;

    auto *cmd = CUndoRedoCommand::getCmdByCmdInfo(info);
    EXPECT_EQ(cmd, nullptr);
}

TEST(CUndoRedoCommandGetCmdByCmdInfoTest, GroupCmdReturnsNull)
{
    CUndoRedoCommand::SCommandInfoCouple info;
    info.undoInfo.tp = CUndoRedoCommand::EDrawGroupCmd;
    info.redoInfo = info.undoInfo;

    auto *cmd = CUndoRedoCommand::getCmdByCmdInfo(info);
    EXPECT_EQ(cmd, nullptr);
}
