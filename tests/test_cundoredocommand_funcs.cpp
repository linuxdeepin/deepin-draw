// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for undo/redo command methods (issue V-5381)
 *
 * Source: src/frame/cundoredocommand.cpp
 *
 * Target methods:
 *   CUndoRedoCommand: clearCommand, finishRecord,
 *                     recordRedoCommand, recordUndoCommand
 *   CUndoRedoCommandGroup: addCommand, count
 *   CItemUndoRedoCommand: item, getCmdByItemCmdInfo
 *   CSceneUndoRedoCommand: drawScene, scene, getCmdBySceneCmdInfo
 *   CSceneItemNumChangedCommand: items, parsingVars, real_redo, real_undo
 *   CSceneGroupChangedCommand: parsingVars
 *   CCmdBlock: constructors
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

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
// CUndoRedoCommand::recordUndoCommand
// =========================================================================

TEST(CUndoRedoCommandRecordUndo, EmptyDatas_ReturnsImmediately)
{
    CUndoRedoCommand::s_recordedCmdInfoList.clear();
    CUndoRedoCommand::s_forFindCoupleMap.clear();

    CUndoRedoCommand::recordUndoCommand(
        CUndoRedoCommand::EItemChangedCmd, 0, QList<QVariant>());

    EXPECT_TRUE(CUndoRedoCommand::s_recordedCmdInfoList.isEmpty());
}

TEST(CUndoRedoCommandRecordUndo, ValidItemCmd_AppendsToList)
{
    CUndoRedoCommand::s_recordedCmdInfoList.clear();
    CUndoRedoCommand::s_forFindCoupleMap.clear();

    QList<QVariant> datas;
    datas << static_cast<long long>(0x1234);

    CUndoRedoCommand::recordUndoCommand(
        CUndoRedoCommand::EItemChangedCmd,
        CItemUndoRedoCommand::EAllChanged, datas);

    ASSERT_EQ(CUndoRedoCommand::s_recordedCmdInfoList.size(), 1);
    auto &cp = CUndoRedoCommand::s_recordedCmdInfoList[0];
    EXPECT_EQ(cp.undoInfo.tp, CUndoRedoCommand::EItemChangedCmd);
    EXPECT_EQ(cp.undoInfo.expTp, CItemUndoRedoCommand::EAllChanged);
    EXPECT_FALSE(CUndoRedoCommand::s_forFindCoupleMap.isEmpty());
}

TEST(CUndoRedoCommandRecordUndo, InitTrue_ClearsPreviousRecords)
{
    CUndoRedoCommand::s_recordedCmdInfoList.clear();
    CUndoRedoCommand::s_forFindCoupleMap.clear();

    QList<QVariant> datas;
    datas << static_cast<long long>(0x1111);
    CUndoRedoCommand::recordUndoCommand(
        CUndoRedoCommand::EItemChangedCmd, 0, datas, false);
    ASSERT_EQ(CUndoRedoCommand::s_recordedCmdInfoList.size(), 1);

    QList<QVariant> datas2;
    datas2 << static_cast<long long>(0x2222);
    CUndoRedoCommand::recordUndoCommand(
        CUndoRedoCommand::EItemChangedCmd, 0, datas2, true);

    EXPECT_EQ(CUndoRedoCommand::s_recordedCmdInfoList.size(), 1);
    EXPECT_EQ(CUndoRedoCommand::s_recordedCmdInfoList[0].undoInfo.vars.first().toLongLong(),
              0x2222);
}

TEST(CUndoRedoCommandRecordUndo, GroupCmdType_NotRecorded)
{
    CUndoRedoCommand::s_recordedCmdInfoList.clear();
    QList<QVariant> datas;
    datas << static_cast<long long>(0x3333);

    CUndoRedoCommand::recordUndoCommand(
        CUndoRedoCommand::EDrawGroupCmd, 0, datas);

    EXPECT_TRUE(CUndoRedoCommand::s_recordedCmdInfoList.isEmpty());
}

// =========================================================================
// CUndoRedoCommand::recordRedoCommand
// =========================================================================

TEST(CUndoRedoCommandRecordRedo, EmptyDatas_ReturnsImmediately)
{
    CUndoRedoCommand::s_recordedCmdInfoList.clear();
    CUndoRedoCommand::s_forFindCoupleMap.clear();

    CUndoRedoCommand::recordRedoCommand(
        CUndoRedoCommand::EItemChangedCmd, 0, QList<QVariant>());

    EXPECT_TRUE(CUndoRedoCommand::s_recordedCmdInfoList.isEmpty());
}

TEST(CUndoRedoCommandRecordRedo, MatchingUndo_SetsRedoInfo)
{
    CUndoRedoCommand::s_recordedCmdInfoList.clear();
    CUndoRedoCommand::s_forFindCoupleMap.clear();

    long long fakeItem = 0x4444;
    QList<QVariant> undoDatas;
    undoDatas << fakeItem;
    CUndoRedoCommand::recordUndoCommand(
        CUndoRedoCommand::EItemChangedCmd,
        CItemUndoRedoCommand::EAllChanged, undoDatas);

    QList<QVariant> redoDatas;
    redoDatas << fakeItem;
    CUndoRedoCommand::recordRedoCommand(
        CUndoRedoCommand::EItemChangedCmd,
        CItemUndoRedoCommand::EAllChanged, redoDatas);

    ASSERT_EQ(CUndoRedoCommand::s_recordedCmdInfoList.size(), 1);
    auto &cp = CUndoRedoCommand::s_recordedCmdInfoList[0];
    EXPECT_TRUE(cp.isVaild());
    EXPECT_EQ(cp.redoInfo.tp, CUndoRedoCommand::EItemChangedCmd);
}

TEST(CUndoRedoCommandRecordRedo, GroupCmdType_NotProcessed)
{
    CUndoRedoCommand::s_recordedCmdInfoList.clear();
    QList<QVariant> datas;
    datas << static_cast<long long>(0x5555);

    CUndoRedoCommand::recordRedoCommand(
        CUndoRedoCommand::EDrawGroupCmd, 0, datas);

    EXPECT_TRUE(CUndoRedoCommand::s_recordedCmdInfoList.isEmpty());
}

// =========================================================================
// CUndoRedoCommand::clearCommand
// =========================================================================

TEST(CUndoRedoCommandClear, ClearsRecordedList)
{
    QList<QVariant> datas;
    datas << static_cast<long long>(0x6666);
    CUndoRedoCommand::recordUndoCommand(
        CUndoRedoCommand::EItemChangedCmd, 0, datas);
    ASSERT_FALSE(CUndoRedoCommand::s_recordedCmdInfoList.isEmpty());
    ASSERT_FALSE(CUndoRedoCommand::s_forFindCoupleMap.isEmpty());

    CUndoRedoCommand::clearCommand();

    EXPECT_TRUE(CUndoRedoCommand::s_recordedCmdInfoList.isEmpty());
    EXPECT_TRUE(CUndoRedoCommand::s_forFindCoupleMap.isEmpty());
}

TEST(CUndoRedoCommandClear, EmptyList_NoCrash)
{
    CUndoRedoCommand::s_recordedCmdInfoList.clear();
    CUndoRedoCommand::s_forFindCoupleMap.clear();

    CUndoRedoCommand::clearCommand();

    EXPECT_TRUE(CUndoRedoCommand::s_recordedCmdInfoList.isEmpty());
}

// =========================================================================
// CUndoRedoCommand::finishRecord
// =========================================================================

TEST(CUndoRedoCommandFinishRecord, EmptyRecord_NoCrash)
{
    CUndoRedoCommand::s_recordedCmdInfoList.clear();
    CUndoRedoCommand::s_forFindCoupleMap.clear();

    CUndoRedoCommand::finishRecord(false);

    EXPECT_TRUE(CUndoRedoCommand::s_recordedCmdInfoList.isEmpty());
}

// =========================================================================
// CUndoRedoCommandGroup::addCommand / count
// =========================================================================

TEST(CUndoRedoCommandGroupAddCount, EmptyGroup_CountZero)
{
    CUndoRedoCommandGroup group;
    EXPECT_EQ(group.count(), 0);
}

TEST(CUndoRedoCommandGroupAddCount, AddCommands_CountIncreases)
{
    CUndoRedoCommandGroup group;

    auto *cmd1 = new CItemMoveCommand();
    auto *cmd2 = new CItemMoveCommand();
    group.addCommand(cmd1);
    EXPECT_EQ(group.count(), 1);

    group.addCommand(cmd2);
    EXPECT_EQ(group.count(), 2);
}

TEST(CUndoRedoCommandGroupAddCount, AddNull_NoCrash)
{
    CUndoRedoCommandGroup group;
    group.addCommand(nullptr);
    EXPECT_EQ(group.count(), 1);
}

// =========================================================================
// CItemUndoRedoCommand::item
// =========================================================================

TEST(CItemUndoRedoCommandItem, DefaultNull_ReturnsNullptr)
{
    CItemUndoRedoCommand cmd;
    EXPECT_EQ(cmd._pItem, nullptr);
}

TEST(CItemUndoRedoCommandItem, AfterParsingVars_ReturnsItem)
{
    createNewViewByShortcutKey();
    PageScene *scene = getCurView()->drawScene();
    ASSERT_NE(scene, nullptr);

    auto *rectItem = new CGraphicsRectItem(QRectF(10, 10, 50, 50));
    scene->addCItem(rectItem, true, false);

    CItemUndoRedoCommand cmd;
    QList<QVariant> vars;
    vars << reinterpret_cast<long long>(static_cast<QGraphicsItem *>(rectItem));
    cmd.setVar(vars, UndoVar);

    EXPECT_EQ(cmd._pItem, static_cast<QGraphicsItem *>(rectItem));
}

// =========================================================================
// CItemUndoRedoCommand::getCmdByItemCmdInfo
// =========================================================================

TEST(CItemUndoRedoCommandGetCmd, EAllChanged_ReturnsBzItemAllCommand)
{
    CUndoRedoCommand::SCommandInfoCouple info;
    info.undoInfo.tp = CUndoRedoCommand::EItemChangedCmd;
    info.undoInfo.expTp = CItemUndoRedoCommand::EAllChanged;

    auto *cmd = CItemUndoRedoCommand::getCmdByItemCmdInfo(info);
    EXPECT_NE(cmd, nullptr);
    EXPECT_NE(dynamic_cast<CBzItemAllCommand *>(cmd), nullptr);
    delete cmd;
}

TEST(CItemUndoRedoCommandGetCmd, EPosChanged_ReturnsItemMoveCommand)
{
    CUndoRedoCommand::SCommandInfoCouple info;
    info.undoInfo.tp = CUndoRedoCommand::EItemChangedCmd;
    info.undoInfo.expTp = CItemUndoRedoCommand::EPosChanged;

    auto *cmd = CItemUndoRedoCommand::getCmdByItemCmdInfo(info);
    EXPECT_NE(cmd, nullptr);
    EXPECT_NE(dynamic_cast<CItemMoveCommand *>(cmd), nullptr);
    delete cmd;
}

TEST(CItemUndoRedoCommandGetCmd, ESizeChanged_ReturnsNullptr)
{
    CUndoRedoCommand::SCommandInfoCouple info;
    info.undoInfo.tp = CUndoRedoCommand::EItemChangedCmd;
    info.undoInfo.expTp = CItemUndoRedoCommand::ESizeChanged;

    auto *cmd = CItemUndoRedoCommand::getCmdByItemCmdInfo(info);
    EXPECT_EQ(cmd, nullptr);
}

// =========================================================================
// CSceneUndoRedoCommand::scene / drawScene
// =========================================================================

TEST(CSceneUndoRedoCommandScene, DefaultNull_ReturnsNullptr)
{
    CSceneUndoRedoCommand cmd;
    EXPECT_EQ(cmd._scene, nullptr);
    EXPECT_EQ(qobject_cast<PageScene *>(cmd._scene), nullptr);
}

TEST(CSceneUndoRedoCommandScene, AfterParsingVars_ReturnsScene)
{
    createNewViewByShortcutKey();
    PageScene *scene = getCurView()->drawScene();
    ASSERT_NE(scene, nullptr);

    CSceneUndoRedoCommand cmd;
    QList<QVariant> vars;
    vars << reinterpret_cast<long long>(static_cast<QGraphicsScene *>(scene));
    cmd.setVar(vars, UndoVar);

    EXPECT_EQ(cmd._scene, static_cast<QGraphicsScene *>(scene));
    EXPECT_EQ(qobject_cast<PageScene *>(cmd._scene), scene);
}

// =========================================================================
// CSceneUndoRedoCommand::getCmdBySceneCmdInfo
// =========================================================================

TEST(CSceneUndoRedoCommandGetCmd, ESizeChanged_ReturnsBoundingCmd)
{
    CUndoRedoCommand::SCommandInfoCouple info;
    info.undoInfo.tp = CUndoRedoCommand::ESceneChangedCmd;
    info.undoInfo.expTp = CSceneUndoRedoCommand::ESizeChanged;

    auto *cmd = CSceneUndoRedoCommand::getCmdBySceneCmdInfo(info);
    EXPECT_NE(cmd, nullptr);
    EXPECT_NE(dynamic_cast<CSceneBoundingChangedCommand *>(cmd), nullptr);
    delete cmd;
}

TEST(CSceneUndoRedoCommandGetCmd, EItemAdded_ReturnsItemNumCmd)
{
    CUndoRedoCommand::SCommandInfoCouple info;
    info.undoInfo.tp = CUndoRedoCommand::ESceneChangedCmd;
    info.undoInfo.expTp = CSceneUndoRedoCommand::EItemAdded;

    auto *cmd = CSceneUndoRedoCommand::getCmdBySceneCmdInfo(info);
    EXPECT_NE(cmd, nullptr);
    delete cmd;
}

TEST(CSceneUndoRedoCommandGetCmd, EGroupChanged_ReturnsGroupCmd)
{
    CUndoRedoCommand::SCommandInfoCouple info;
    info.undoInfo.tp = CUndoRedoCommand::ESceneChangedCmd;
    info.undoInfo.expTp = CSceneUndoRedoCommand::EGroupChanged;

    auto *cmd = CSceneUndoRedoCommand::getCmdBySceneCmdInfo(info);
    EXPECT_NE(cmd, nullptr);
    EXPECT_NE(dynamic_cast<CSceneGroupChangedCommand *>(cmd), nullptr);
    delete cmd;
}

// =========================================================================
// CSceneItemNumChangedCommand::items / parsingVars
// =========================================================================

TEST(CSceneItemNumCmdItems, DefaultEmpty)
{
    CSceneItemNumChangedCommand cmd(CSceneItemNumChangedCommand::Added);
    EXPECT_TRUE(cmd.items().isEmpty());
}

TEST(CSceneItemNumCmdParsingVars, ParsesSceneAndItems)
{
    createNewViewByShortcutKey();
    PageScene *scene = getCurView()->drawScene();
    ASSERT_NE(scene, nullptr);

    auto *rect1 = new CGraphicsRectItem(QRectF(10, 10, 50, 50));
    auto *rect2 = new CGraphicsRectItem(QRectF(80, 80, 50, 50));
    scene->addCItem(rect1, true, false);
    scene->addCItem(rect2, true, false);

    CSceneItemNumChangedCommand cmd(CSceneItemNumChangedCommand::Added);
    QList<QVariant> vars;
    vars << reinterpret_cast<long long>(static_cast<QGraphicsScene *>(scene));
    vars << reinterpret_cast<long long>(static_cast<QGraphicsItem *>(rect1));
    vars << reinterpret_cast<long long>(static_cast<QGraphicsItem *>(rect2));
    cmd.setVar(vars, UndoVar);

    EXPECT_EQ(cmd._scene, static_cast<QGraphicsScene *>(scene));
    ASSERT_EQ(cmd.items().size(), 2);
    EXPECT_EQ(cmd.items()[0], static_cast<CGraphicsItem *>(rect1));
    EXPECT_EQ(cmd.items()[1], static_cast<CGraphicsItem *>(rect2));
}

// =========================================================================
// CSceneItemNumChangedCommand::real_redo / real_undo
// =========================================================================

TEST(CSceneItemNumCmdRealRedoUndo, AddedType_RedoAddsUndoRemoves)
{
    createNewViewByShortcutKey();
    PageScene *scene = getCurView()->drawScene();
    ASSERT_NE(scene, nullptr);

    auto *rect = new CGraphicsRectItem(QRectF(10, 10, 50, 50));
    scene->addCItem(rect, true, false);
    ASSERT_EQ(scene->getBzItems().count(), 1);

    CSceneItemNumChangedCommand cmd(CSceneItemNumChangedCommand::Added);
    QList<QVariant> vars;
    vars << reinterpret_cast<long long>(static_cast<QGraphicsScene *>(scene));
    vars << reinterpret_cast<long long>(static_cast<QGraphicsItem *>(rect));
    cmd.setVar(vars, UndoVar);
    cmd.setVar(vars, RedoVar);

    // undo for Added → remove item
    cmd.real_undo();
    EXPECT_EQ(scene->getBzItems().count(), 0);

    // redo for Added → add item back
    cmd.real_redo();
    EXPECT_EQ(scene->getBzItems().count(), 1);
}

TEST(CSceneItemNumCmdRealRedoUndo, RemovedType_RedoRemovesUndoAdds)
{
    createNewViewByShortcutKey();
    PageScene *scene = getCurView()->drawScene();
    ASSERT_NE(scene, nullptr);

    auto *rect = new CGraphicsRectItem(QRectF(10, 10, 50, 50));
    scene->addCItem(rect, true, false);
    ASSERT_EQ(scene->getBzItems().count(), 1);

    CSceneItemNumChangedCommand cmd(CSceneItemNumChangedCommand::Removed);
    QList<QVariant> vars;
    vars << reinterpret_cast<long long>(static_cast<QGraphicsScene *>(scene));
    vars << reinterpret_cast<long long>(static_cast<QGraphicsItem *>(rect));
    cmd.setVar(vars, UndoVar);
    cmd.setVar(vars, RedoVar);

    // undo for Removed → add item back (already in scene, addCItem again)
    cmd.real_undo();
    EXPECT_EQ(scene->getBzItems().count(), 1);

    // redo for Removed → remove item
    cmd.real_redo();
    EXPECT_EQ(scene->getBzItems().count(), 0);
}

// =========================================================================
// CSceneGroupChangedCommand::parsingVars
// =========================================================================

TEST(CSceneGroupChangedCmdParsingVars, OnlyScene_NoItems)
{
    createNewViewByShortcutKey();
    PageScene *scene = getCurView()->drawScene();
    ASSERT_NE(scene, nullptr);

    CSceneGroupChangedCommand cmd;
    QList<QVariant> vars;
    vars << reinterpret_cast<long long>(static_cast<QGraphicsScene *>(scene));
    QVariant treeVar;
    treeVar.setValue(PageScene::CGroupBzItemsTree());
    vars << treeVar;
    cmd.setVar(vars, UndoVar);

    EXPECT_EQ(qobject_cast<PageScene *>(cmd._scene), scene);
    EXPECT_TRUE(cmd.realChangedItems().isEmpty());
}

TEST(CSceneGroupChangedCmdParsingVars, WithItems_ParsesItems)
{
    createNewViewByShortcutKey();
    PageScene *scene = getCurView()->drawScene();
    ASSERT_NE(scene, nullptr);

    auto *rect = new CGraphicsRectItem(QRectF(10, 10, 50, 50));
    scene->addCItem(rect, true, false);

    CSceneGroupChangedCommand cmd;
    QList<QVariant> vars;
    vars << reinterpret_cast<long long>(static_cast<QGraphicsScene *>(scene));
    QVariant treeVar;
    treeVar.setValue(PageScene::CGroupBzItemsTree());
    vars << treeVar;
    vars << reinterpret_cast<long long>(static_cast<QGraphicsItem *>(rect));
    cmd.setVar(vars, UndoVar);

    EXPECT_EQ(cmd.realChangedItems().size(), 1);
    EXPECT_EQ(cmd.realChangedItems().first(), static_cast<CGraphicsItem *>(rect));
}

TEST(CSceneGroupChangedCmdParsingVars, TooFewVars_NoCrash)
{
    CSceneGroupChangedCommand cmd;
    QList<QVariant> vars;
    vars << static_cast<long long>(0);
    cmd.setVar(vars, UndoVar);

    EXPECT_TRUE(cmd.realChangedItems().isEmpty());
}

// =========================================================================
// CCmdBlock constructors
// =========================================================================

TEST(CCmdBlockCtor, ItemConstructor_NullItem_NoCrash)
{
    {
        CCmdBlock block(static_cast<CGraphicsItem *>(nullptr), EChanged, false);
    }
    SUCCEED();
}

TEST(CCmdBlockCtor, ItemConstructor_EChangedUpdate_NoCrash)
{
    {
        CCmdBlock block(static_cast<CGraphicsItem *>(nullptr), EChangedUpdate, false);
    }
    SUCCEED();
}

TEST(CCmdBlockCtor, SceneConstructor_NullScene_NoCrash)
{
    {
        CCmdBlock block(static_cast<PageScene *>(nullptr),
                        CSceneUndoRedoCommand::ESizeChanged,
                        QList<QGraphicsItem *>(), false);
    }
    SUCCEED();
}

TEST(CCmdBlockCtor, SceneConstructorWithItem_NullScene_NoCrash)
{
    {
        CCmdBlock block(static_cast<PageScene *>(nullptr),
                        CSceneUndoRedoCommand::ESizeChanged,
                        static_cast<CGraphicsItem *>(nullptr), false);
    }
    SUCCEED();
}
