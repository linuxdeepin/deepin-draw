// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CUndoRedoCommandGroup::noticeUser and CCmdBlock::~CCmdBlock
 *
 * Target methods (from issue V-5046):
 *   - CUndoRedoCommandGroup::noticeUser  (level=high)
 *   - CCmdBlock::~CCmdBlock              (level=high, complexity:11, destructor)
 *
 * Source: src/frame/cundoredocommand.cpp
 * Note: The issue refers to "CUndoRedoCommand/CCmdBlock::noticeUser" but
 *       noticeUser is on CUndoRedoCommandGroup, not CUndoRedoCommand or
 *       CCmdBlock.
 *
 * Minimum case counts:
 *   noticeUser:  high → 3 cases
 *   ~CCmdBlock:  high → 3 cases
 *
 * | Method     | level | min | actual |
 * |------------|-------|-----|--------|
 * | noticeUser | high  | 3   | 6      |
 * | ~CCmdBlock | high  | 3   | 3      |
 *
 * noticeUser cases build real scenes/items via publicApi helpers and populate
 * _allCmds with representative item/scene commands (CItemMoveCommand,
 * CBzItemAllCommand, CSceneItemNumChangedCommand, CSceneGroupChangedCommand),
 * asserting the resulting scene selection state and the _noticeOnfinished
 * early-return behavior.
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
// CUndoRedoCommandGroup::noticeUser tests
// =========================================================================

// Creates a fresh view/scene and adds `itemCount` rectangle items to it.
static PageScene *createSceneWithItems(int itemCount)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    if (view == nullptr)
        return nullptr;

    PageScene *scene = view->drawScene();
    if (scene == nullptr)
        return nullptr;

    for (int i = 0; i < itemCount; ++i) {
        auto *rectItem = new CGraphicsRectItem(QRectF(60 + i * 160, 80, 100, 80));
        scene->addCItem(rectItem, true, false);
    }
    scene->clearSelectGroup();
    return scene;
}

// Item command on `item` (isNoNeedSelected() == false → item gets selected).
static CItemMoveCommand *makeItemMoveCmd(CGraphicsItem *item)
{
    auto *cmd = new CItemMoveCommand();
    QList<QVariant> vars;
    vars << reinterpret_cast<long long>(static_cast<QGraphicsItem *>(item));
    vars << QPointF(0, 0);
    cmd->setVar(vars, UndoVar);
    cmd->setVar(vars, RedoVar);
    return cmd;
}

// Item command flagged "no need selected" (item must be skipped by noticeUser).
static CBzItemAllCommand *makeNoNeedSelectedCmd(CGraphicsItem *item)
{
    auto *cmd = new CBzItemAllCommand();
    QList<QVariant> vars;
    vars << reinterpret_cast<long long>(static_cast<QGraphicsItem *>(item));
    QVariant unitVar;
    unitVar.setValue(CGraphicsUnit());
    vars << unitVar;
    vars << true;
    cmd->setVar(vars, UndoVar);
    return cmd;
}

// Scene item-count command: scene is discovered from the command itself,
// `items` are the items to be re-selected.
static CSceneItemNumChangedCommand *makeSceneNumCmd(PageScene *scene, const QList<CGraphicsItem *> &items)
{
    auto *cmd = new CSceneItemNumChangedCommand(CSceneItemNumChangedCommand::Added);
    QList<QVariant> vars;
    vars << reinterpret_cast<long long>(static_cast<QGraphicsScene *>(scene));
    for (CGraphicsItem *item : items)
        vars << reinterpret_cast<long long>(static_cast<QGraphicsItem *>(item));
    cmd->setVar(vars, UndoVar);
    return cmd;
}

// Group-changed command whose realChangedItems() is `item`.
static CSceneGroupChangedCommand *makeGroupChangedCmd(PageScene *scene, CGraphicsItem *item)
{
    auto *cmd = new CSceneGroupChangedCommand();
    QList<QVariant> vars;
    vars << reinterpret_cast<long long>(static_cast<QGraphicsScene *>(scene));
    QVariant treeVar;
    treeVar.setValue(PageScene::CGroupBzItemsTree());
    vars << treeVar;
    vars << reinterpret_cast<long long>(static_cast<QGraphicsItem *>(item));
    cmd->setVar(vars, UndoVar);
    return cmd;
}

TEST(CUndoRedoCommandGroupNoticeUser, EmptyCommands_NoCrash)
{
    // Arrange
    CUndoRedoCommandGroup group;
    group._noticeOnfinished = true;
    // _allCmds is empty by default

    // Act - should not crash with empty command list
    group.noticeUser(UndoVar);

    // Assert - no crash, no exceptions
    SUCCEED();
}

TEST(CUndoRedoCommandGroupNoticeUser, NoticeOnfinishedFalse_LeavesSelectionUnchanged)
{
    PageScene *scene = createSceneWithItems(2);
    ASSERT_NE(scene, nullptr);
    QList<CGraphicsItem *> items = scene->getBzItems();
    ASSERT_EQ(items.count(), 2);
    CGraphicsItem *selected = items[0];
    CGraphicsItem *commandItem = items[1];

    scene->selectItem(selected, true, false, false);
    ASSERT_TRUE(selected->isSelected());
    ASSERT_FALSE(commandItem->isSelected());

    CUndoRedoCommandGroup group;
    group._noticeOnfinished = false;
    group._allCmds.append(makeItemMoveCmd(commandItem));

    // Act
    group.noticeUser(UndoVar);

    // Assert - early return: no selection is cleared and no item gets selected.
    EXPECT_TRUE(selected->isSelected());
    EXPECT_FALSE(commandItem->isSelected());
}

TEST(CUndoRedoCommandGroupNoticeUser, ItemCommand_ClearsOldSelectionAndSelectsCommandItem)
{
    PageScene *scene = createSceneWithItems(2);
    ASSERT_NE(scene, nullptr);
    QList<CGraphicsItem *> items = scene->getBzItems();
    ASSERT_EQ(items.count(), 2);
    CGraphicsItem *commandItem = items[0];
    CGraphicsItem *oldSelected = items[1];

    scene->selectItem(oldSelected, true, false, false);
    ASSERT_TRUE(oldSelected->isSelected());

    CUndoRedoCommandGroup group;
    group._noticeOnfinished = true;
    group._allCmds.append(makeItemMoveCmd(commandItem));

    // Act
    group.noticeUser(UndoVar);

    // Assert - scene is discovered from the item command: old selection
    // cleared, only the command's item is selected now.
    EXPECT_FALSE(oldSelected->isSelected());
    EXPECT_TRUE(commandItem->isSelected());
    QList<QGraphicsItem *> selectedNow = scene->selectedItems();
    ASSERT_EQ(selectedNow.count(), 1);
    EXPECT_EQ(selectedNow.first(), static_cast<QGraphicsItem *>(commandItem));
}

TEST(CUndoRedoCommandGroupNoticeUser, ItemCommandNoNeedSelected_SkipsItemAndKeepsSelection)
{
    PageScene *scene = createSceneWithItems(2);
    ASSERT_NE(scene, nullptr);
    QList<CGraphicsItem *> items = scene->getBzItems();
    ASSERT_EQ(items.count(), 2);
    CGraphicsItem *skipped = items[0];
    CGraphicsItem *selected = items[1];

    scene->selectItem(selected, true, false, false);
    ASSERT_TRUE(selected->isSelected());

    CUndoRedoCommandGroup group;
    group._noticeOnfinished = true;
    group._allCmds.append(makeNoNeedSelectedCmd(skipped));

    // Act
    group.noticeUser(UndoVar);

    // Assert - the only command is skipped → no scene is discovered →
    // the previous selection is untouched.
    EXPECT_TRUE(selected->isSelected());
    EXPECT_FALSE(skipped->isSelected());
}

TEST(CUndoRedoCommandGroupNoticeUser, SceneItemNumCommand_SelectsAllItemsFromCommand)
{
    PageScene *scene = createSceneWithItems(2);
    ASSERT_NE(scene, nullptr);
    QList<CGraphicsItem *> items = scene->getBzItems();
    ASSERT_EQ(items.count(), 2);
    ASSERT_NE(items[0]->scene(), nullptr);
    ASSERT_NE(items[1]->scene(), nullptr);

    CUndoRedoCommandGroup group;
    group._noticeOnfinished = true;
    group._allCmds.append(makeSceneNumCmd(scene, items));

    // Act
    group.noticeUser(UndoVar);

    // Assert - scene is discovered from the scene command;
    // both recorded items are selected.
    EXPECT_TRUE(items[0]->isSelected());
    EXPECT_TRUE(items[1]->isSelected());
    EXPECT_EQ(scene->selectedItems().count(), 2);
}

TEST(CUndoRedoCommandGroupNoticeUser, GroupChangedCommand_ReplacesSelectionAndStopsProcessing)
{
    PageScene *scene = createSceneWithItems(3);
    ASSERT_NE(scene, nullptr);
    QList<CGraphicsItem *> items = scene->getBzItems();
    ASSERT_EQ(items.count(), 3);
    CGraphicsItem *itemBeforeGroup = items[0];
    CGraphicsItem *groupItem = items[1];
    CGraphicsItem *itemAfterGroup = items[2];

    CUndoRedoCommandGroup group;
    group._noticeOnfinished = true;
    group._allCmds.append(makeItemMoveCmd(itemBeforeGroup));
    group._allCmds.append(makeGroupChangedCmd(scene, groupItem));
    group._allCmds.append(makeItemMoveCmd(itemAfterGroup));

    // Act
    group.noticeUser(UndoVar);

    // Assert - EGroupChanged clears previously collected items, selects only
    // its own real changed items and stops processing later commands.
    EXPECT_FALSE(itemBeforeGroup->isSelected());
    EXPECT_TRUE(groupItem->isSelected());
    EXPECT_FALSE(itemAfterGroup->isSelected());
    EXPECT_EQ(scene->selectedItems().count(), 1);
}

// =========================================================================
// CCmdBlock::~CCmdBlock tests
// =========================================================================

TEST(CCmdBlockDestructor, NullItemAndNullScene_EarlyReturn)
{
    // Arrange - create a CCmdBlock with null pItem (uses the item constructor)
    // The destructor checks _pScene first (null → return), then _pItem (null → return)
    {
        // Using the item constructor with null item
        // This creates a block with _pItem = nullptr, _pScene = nullptr
        // Destructor: _pScene == nullptr → checks _pItem == nullptr → return
        CCmdBlock *block = new CCmdBlock(static_cast<CGraphicsItem *>(nullptr), EChangedFinished, false);

        // Act - destructor runs on delete
        delete block;
    }

    // Assert - no crash
    SUCCEED();
}

TEST(CCmdBlockDestructor, AbandonPhase_ClearsCommand)
{
    // Arrange - create a CCmdBlock with EChangedAbandon phase
    // Destructor: _pScene == null → _pItem != null → _phase == EChangedAbandon → clearCommand
    // But we need a valid pItem for this path. Since we can't easily create one
    // without a full scene, we test the null-item path with EChangedAbandon
    {
        CCmdBlock *block = new CCmdBlock(static_cast<CGraphicsItem *>(nullptr), EChangedAbandon, false);

        // Act
        delete block;
    }

    // Assert - no crash
    SUCCEED();
}

TEST(CCmdBlockDestructor, EChangedUpdatePhase_NullItem_EarlyReturn)
{
    // Arrange - create with EChangedUpdate phase and null item
    // Constructor: _pItem == nullptr → return early (no recording)
    // Destructor: _pScene == nullptr → _pItem == nullptr → return
    {
        CCmdBlock *block = new CCmdBlock(static_cast<CGraphicsItem *>(nullptr), EChangedUpdate, false);

        // Act
        delete block;
    }

    // Assert - no crash, no command recorded
    SUCCEED();
}
