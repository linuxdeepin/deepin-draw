// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#define protected public
#define private public
#include "cgraphicsview.h"
#include <qaction.h>

#include "ccentralwidget.h"
#include "clefttoolbar.h"
#include "toptoolbar.h"
#include "drawshape/cdrawscene.h"
#include "drawshape/cdrawparamsigleton.h"
#include "drawshape/drawItems/cgraphicsitemselectedmgr.h"
#include "drawshape/globaldefine.h"
#include "drawshape/sitemdata.h"
#include "application.h"

#include "crecttool.h"
#include "ccuttool.h"
#include "cellipsetool.h"
#include "cmasicotool.h"
#include "cpentool.h"
#include "cpolygonalstartool.h"
#include "cpolygontool.h"
#include "ctexttool.h"
#include "ctriangletool.h"

#include <DFloatingButton>
#include <DComboBox>
#include <dzoommenucombobox.h>
#include "cspinbox.h"

#include "cpictureitem.h"
#include "cgraphicsrectitem.h"
#include "cgraphicsellipseitem.h"
#include "cgraphicstriangleitem.h"
#include "cgraphicspolygonalstaritem.h"
#include "cgraphicspolygonitem.h"
#include "cgraphicslineitem.h"
#include "cgraphicspenitem.h"
#include "cgraphicstextitem.h"
#include "cgraphicscutitem.h"

#include <QDebug>
#include <DLineEdit>

#include "publicApi.h"

#undef protected
#undef private

// =========================================================================
// firstItem tests
// =========================================================================

TEST(PageSceneFirstItem, EmptyScene_ReturnsNullptr)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    // Empty scene, empty itemsCus, incW=0 → scene->items(pos) returns empty → nullptr
    QGraphicsItem *result = scene->firstItem(QPointF(100, 100),
                                             QList<QGraphicsItem *>(),
                                             false, false, false, false, false, 0);
    EXPECT_EQ(result, nullptr);
}

TEST(PageSceneFirstItem, WithBzItemsPenalgorFalseIsBzItemTrue_ReturnsFirstBzItem)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(100, 100), QPoint(200, 200), false);

    auto bzItems = scene->getBzItems();
    ASSERT_GT(bzItems.count(), 0);

    QList<QGraphicsItem *> items;
    for (auto item : bzItems) {
        items.append(item);
    }

    // penalgor=false, isBzItem=true → returns first bz item from sorted list
    QGraphicsItem *result = scene->firstItem(QPointF(150, 150), items,
                                             false, false, true, false, false, 0);
    EXPECT_NE(result, nullptr);
}

TEST(PageSceneFirstItem, WithBzItemsPenalgorFalseIsBzItemFalse_ReturnsFirstItem)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(100, 100), QPoint(200, 200), false);

    auto bzItems = scene->getBzItems();
    ASSERT_GT(bzItems.count(), 0);

    QList<QGraphicsItem *> items;
    for (auto item : bzItems) {
        items.append(item);
    }

    // penalgor=false, isBzItem=false → returns items.first() (highest Z)
    QGraphicsItem *result = scene->firstItem(QPointF(150, 150), items,
                                             false, false, false, false, false, 0);
    EXPECT_NE(result, nullptr);
}

TEST(PageSceneFirstItem, WithBzItemsPenalgorTrue_ReturnsNonPenetrableItem)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(100, 100), QPoint(200, 200), false);

    auto bzItems = scene->getBzItems();
    ASSERT_GT(bzItems.count(), 0);

    QList<QGraphicsItem *> items;
    for (auto item : bzItems) {
        items.append(item);
    }

    // penalgor=true → penetrability algorithm, rectangle is not penetrable at center
    QGraphicsItem *result = scene->firstItem(QPointF(150, 150), items,
                                             false, true, false, false, false, 0);
    EXPECT_NE(result, nullptr);
}

// =========================================================================
// getGroupTree tests
// =========================================================================

TEST(PageSceneGetGroupTree, NullptrOnEmptyScene_ReturnsEmptyTree)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    auto tree = scene->getGroupTree(nullptr);
    EXPECT_EQ(tree.childGroups.count(), 0);
    EXPECT_EQ(tree.bzItems.count(), 0);
    EXPECT_EQ(tree.groupType(), CGraphicsItemGroup::EVirRootGroup);
    PageScene::releaseBzItemsTreeInfo(tree);
}

TEST(PageSceneGetGroupTree, NullptrWithItemsButNoGroups_ReturnsEmptyChildGroups)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(100, 100), QPoint(200, 200), false);

    ASSERT_GE(scene->getBzItems().count(), 1);

    // No groups created, so getGroupTree(nullptr) should return empty childGroups
    auto tree = scene->getGroupTree(nullptr);
    EXPECT_EQ(tree.childGroups.count(), 0);
    PageScene::releaseBzItemsTreeInfo(tree);
}

TEST(PageSceneGetGroupTree, NullptrWithGroups_ReturnsTreeWithChildGroups)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    // Create two rectangle items
    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(100, 100), QPoint(200, 200), false);
    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(300, 300), QPoint(400, 400), false);

    ASSERT_GE(scene->getBzItems().count(), 2);

    // Select all and group
    view->slotOnSelectAll();
    DTestEventList e;
    e.addKeyPress(Qt::Key_G, Qt::ControlModifier, 100);
    e.addKeyRelease(Qt::Key_G, Qt::ControlModifier, 100);
    e.simulate(view->viewport());

    ASSERT_GE(scene->m_pGroups.count(), 1);

    auto tree = scene->getGroupTree(nullptr);
    EXPECT_GE(tree.childGroups.count(), 1);
    PageScene::releaseBzItemsTreeInfo(tree);
}

TEST(PageSceneGetGroupTree, WithValidGroup_ReturnsGroupTree)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(100, 100), QPoint(200, 200), false);
    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(300, 300), QPoint(400, 400), false);

    ASSERT_GE(scene->getBzItems().count(), 2);

    view->slotOnSelectAll();
    DTestEventList e;
    e.addKeyPress(Qt::Key_G, Qt::ControlModifier, 100);
    e.addKeyRelease(Qt::Key_G, Qt::ControlModifier, 100);
    e.simulate(view->viewport());

    ASSERT_GE(scene->m_pGroups.count(), 1);

    CGraphicsItemGroup *pGroup = scene->m_pGroups.first();
    ASSERT_NE(pGroup, nullptr);

    auto tree = scene->getGroupTree(pGroup);
    EXPECT_NE(tree.pGroup, nullptr);
    EXPECT_GE(tree.bzItems.count(), 1);
    PageScene::releaseBzItemsTreeInfo(tree);
}

// =========================================================================
// getGroupTreeInfo tests
// =========================================================================

TEST(PageSceneGetGroupTreeInfo, NullptrOnEmptyScene_ReturnsEmptyTreeInfo)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    auto treeInfo = scene->getGroupTreeInfo(nullptr, EDuplicate, false);
    EXPECT_EQ(treeInfo.childGroups.count(), 0);
    EXPECT_EQ(treeInfo.bzItems.count(), 0);
    EXPECT_EQ(treeInfo.groupType(), CGraphicsItemGroup::EVirRootGroup);
    PageScene::releaseBzItemsTreeInfo(treeInfo);
}

TEST(PageSceneGetGroupTreeInfo, NullptrWithGroupsEDuplicate_ReturnsTreeInfo)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(100, 100), QPoint(200, 200), false);
    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(300, 300), QPoint(400, 400), false);

    ASSERT_GE(scene->getBzItems().count(), 2);

    view->slotOnSelectAll();
    DTestEventList e;
    e.addKeyPress(Qt::Key_G, Qt::ControlModifier, 100);
    e.addKeyRelease(Qt::Key_G, Qt::ControlModifier, 100);
    e.simulate(view->viewport());

    ASSERT_GE(scene->m_pGroups.count(), 1);

    auto treeInfo = scene->getGroupTreeInfo(nullptr, EDuplicate, false);
    EXPECT_GE(treeInfo.childGroups.count(), 1);
    PageScene::releaseBzItemsTreeInfo(treeInfo);
}

TEST(PageSceneGetGroupTreeInfo, NullptrWithESaveToDDf_ReturnsTreeInfoWithBzItems)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(100, 100), QPoint(200, 200), false);

    ASSERT_GE(scene->getBzItems().count(), 1);

    // With ESaveToDDf, top-level bz items (not in any group) are collected into bzItems
    auto treeInfo = scene->getGroupTreeInfo(nullptr, ESaveToDDf, false);
    EXPECT_EQ(treeInfo.groupType(), CGraphicsItemGroup::EVirRootGroup);
    // bzItems should contain top-level ungrouped items
    EXPECT_GT(treeInfo.bzItems.count(), 0);
    PageScene::releaseBzItemsTreeInfo(treeInfo);
}

TEST(PageSceneGetGroupTreeInfo, WithValidGroup_ReturnsGroupTreeInfo)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(100, 100), QPoint(200, 200), false);
    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(300, 300), QPoint(400, 400), false);
    view->slotOnSelectAll();
    DTestEventList e;
    e.addKeyPress(Qt::Key_G, Qt::ControlModifier, 100);
    e.addKeyRelease(Qt::Key_G, Qt::ControlModifier, 100);
    e.simulate(view->viewport());

    ASSERT_GE(scene->m_pGroups.count(), 1);

    CGraphicsItemGroup *pGroup = scene->m_pGroups.first();
    ASSERT_NE(pGroup, nullptr);

    auto treeInfo = scene->getGroupTreeInfo(pGroup, EDuplicate, false);
    EXPECT_GE(treeInfo.bzItems.count(), 1);
    PageScene::releaseBzItemsTreeInfo(treeInfo);
}

// =========================================================================
// isCurrentZMovable tests
// =========================================================================

TEST(PageSceneIsCurrentZMovable, NoSelection_ReturnsFalse)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    scene->clearSelectGroup();
    EXPECT_FALSE(scene->isCurrentZMovable(EDownLayer, 1, nullptr));
}

TEST(PageSceneIsCurrentZMovable, LessThanTwoBzItems_ReturnsFalse)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(100, 100), QPoint(200, 200), false);

    ASSERT_EQ(scene->getBzItems().count(), 1);

    scene->clearSelectGroup();
    auto bzItems = scene->getBzItems();
    ASSERT_GT(bzItems.count(), 0);
    scene->selectItem(bzItems.first());

    EXPECT_FALSE(scene->isCurrentZMovable(EDownLayer, 1, nullptr));
}

TEST(PageSceneIsCurrentZMovable, WithSelectionEDownLayer_ReturnsExpected)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(100, 100), QPoint(200, 200), false);
    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(300, 300), QPoint(400, 400), false);
    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(500, 500), QPoint(600, 600), false);

    ASSERT_GE(scene->getBzItems().count(), 3);

    auto bzItems = scene->getBzItems();
    // bzItems is sorted descending by Z; first=highest Z, last=lowest Z
    // Select the middle item (neither highest nor lowest Z)
    CGraphicsItem *middleItem = bzItems[1];
    ASSERT_NE(middleItem, nullptr);

    scene->clearSelectGroup();
    scene->selectItem(middleItem);

    // Middle item can move down (its Z > lowest Z)
    EXPECT_TRUE(scene->isCurrentZMovable(EDownLayer, 1, nullptr));
}

TEST(PageSceneIsCurrentZMovable, WithSelectionEUpLayer_ReturnsExpected)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(100, 100), QPoint(200, 200), false);
    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(300, 300), QPoint(400, 400), false);
    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(500, 500), QPoint(600, 600), false);

    ASSERT_GE(scene->getBzItems().count(), 3);

    auto bzItems = scene->getBzItems();
    // Select the middle item (neither highest nor lowest Z)
    CGraphicsItem *middleItem = bzItems[1];
    ASSERT_NE(middleItem, nullptr);

    scene->clearSelectGroup();
    scene->selectItem(middleItem);

    // Middle item can move up (its Z < highest Z)
    EXPECT_TRUE(scene->isCurrentZMovable(EUpLayer, 1, nullptr));
}

// =========================================================================
// moveBzItemsLayer tests
// Branches: items.isEmpty() early return, pushToStack true/false,
//           switch(tp): EDownLayer / EUpLayer / EToGroup
// =========================================================================

TEST(PageSceneMoveBzItemsLayer, EmptyItems_ReturnsImmediately)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    QList<CGraphicsItem *> emptyItems;
    scene->moveBzItemsLayer(emptyItems, EDownLayer, 1, nullptr, false);
    SUCCEED();
}

TEST(PageSceneMoveBzItemsLayer, EDownLayer_DispatchesToMoveItemsZDown)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(100, 100), QPoint(200, 200), false);
    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(300, 300), QPoint(400, 400), false);
    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(500, 500), QPoint(600, 600), false);

    ASSERT_GE(scene->getBzItems().count(), 3);

    auto bzItems = scene->getBzItems();
    CGraphicsItem *topItem = bzItems[0];
    qreal originalZ = topItem->zValue();

    QList<CGraphicsItem *> items;
    items.append(topItem);

    scene->moveBzItemsLayer(items, EDownLayer, 1, nullptr, false);

    EXPECT_LT(topItem->zValue(), originalZ);
}

TEST(PageSceneMoveBzItemsLayer, EUpLayer_DispatchesToMoveItemsZUp)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(100, 100), QPoint(200, 200), false);
    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(300, 300), QPoint(400, 400), false);
    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(500, 500), QPoint(600, 600), false);

    ASSERT_GE(scene->getBzItems().count(), 3);

    auto bzItems = scene->getBzItems();
    CGraphicsItem *bottomItem = bzItems[2];
    qreal originalZ = bottomItem->zValue();

    QList<CGraphicsItem *> items;
    items.append(bottomItem);

    scene->moveBzItemsLayer(items, EUpLayer, 1, nullptr, false);

    EXPECT_GT(bottomItem->zValue(), originalZ);
}

TEST(PageSceneMoveBzItemsLayer, EToGroup_DispatchesToSortZBaseOneBzItem)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(100, 100), QPoint(200, 200), false);
    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(300, 300), QPoint(400, 400), false);
    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(500, 500), QPoint(600, 600), false);

    ASSERT_GE(scene->getBzItems().count(), 3);

    auto bzItems = scene->getBzItems();
    CGraphicsItem *topItem = bzItems[0];
    CGraphicsItem *middleItem = bzItems[1];
    CGraphicsItem *bottomItem = bzItems[2];

    QList<CGraphicsItem *> items;
    items.append(topItem);
    items.append(bottomItem);

    scene->moveBzItemsLayer(items, EToGroup, 1, topItem, false);

    EXPECT_GE(topItem->zValue(), middleItem->zValue());
    EXPECT_GE(bottomItem->zValue(), middleItem->zValue());
}

TEST(PageSceneMoveBzItemsLayer, PushToStack_RecordsUndoRedo)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(100, 100), QPoint(200, 200), false);
    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(300, 300), QPoint(400, 400), false);
    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(500, 500), QPoint(600, 600), false);

    ASSERT_GE(scene->getBzItems().count(), 3);

    auto bzItems = scene->getBzItems();
    CGraphicsItem *middleItem = bzItems[1];
    qreal originalZ = middleItem->zValue();

    QList<CGraphicsItem *> items;
    items.append(middleItem);

    scene->moveBzItemsLayer(items, EDownLayer, 1, nullptr, true);

    EXPECT_NE(middleItem->zValue(), originalZ);
}

// =========================================================================
// moveItemsZDown tests
// Branches: getSameGroup nullptr early return, ENormalGroup vs EVirRootGroup,
//           step == -1, toLimitFirst, boundary check
// =========================================================================

TEST(PageSceneMoveItemsZDown, SingleStep_MovesItemDown)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(100, 100), QPoint(200, 200), false);
    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(300, 300), QPoint(400, 400), false);
    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(500, 500), QPoint(600, 600), false);

    ASSERT_GE(scene->getBzItems().count(), 3);

    auto bzItems = scene->getBzItems();
    CGraphicsItem *topItem = bzItems[0];
    qreal originalZ = topItem->zValue();

    QList<CGraphicsItem *> items;
    items.append(topItem);

    scene->moveItemsZDown(items, 1);

    EXPECT_LT(topItem->zValue(), originalZ);
}

TEST(PageSceneMoveItemsZDown, StepMinusOne_MovesToBottom)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(100, 100), QPoint(200, 200), false);
    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(300, 300), QPoint(400, 400), false);
    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(500, 500), QPoint(600, 600), false);

    ASSERT_GE(scene->getBzItems().count(), 3);

    auto bzItems = scene->getBzItems();
    CGraphicsItem *topItem = bzItems[0];
    CGraphicsItem *middleItem = bzItems[1];
    qreal originalZ = topItem->zValue();

    QList<CGraphicsItem *> items;
    items.append(topItem);

    scene->moveItemsZDown(items, -1);

    EXPECT_LT(topItem->zValue(), middleItem->zValue());
    EXPECT_LT(topItem->zValue(), originalZ);
}

// =========================================================================
// moveItemsZUp tests
// Branches: getSameGroup nullptr early return, ENormalGroup vs EVirRootGroup,
//           step == -1, toLimitFirst, boundary check
// =========================================================================

TEST(PageSceneMoveItemsZUp, SingleStep_MovesItemUp)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(100, 100), QPoint(200, 200), false);
    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(300, 300), QPoint(400, 400), false);
    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(500, 500), QPoint(600, 600), false);

    ASSERT_GE(scene->getBzItems().count(), 3);

    auto bzItems = scene->getBzItems();
    CGraphicsItem *bottomItem = bzItems[2];
    qreal originalZ = bottomItem->zValue();

    QList<CGraphicsItem *> items;
    items.append(bottomItem);

    scene->moveItemsZUp(items, 1);

    EXPECT_GT(bottomItem->zValue(), originalZ);
}

TEST(PageSceneMoveItemsZUp, StepMinusOne_MovesToTop)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(100, 100), QPoint(200, 200), false);
    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(300, 300), QPoint(400, 400), false);
    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(500, 500), QPoint(600, 600), false);

    ASSERT_GE(scene->getBzItems().count(), 3);

    auto bzItems = scene->getBzItems();
    CGraphicsItem *bottomItem = bzItems[2];
    CGraphicsItem *middleItem = bzItems[1];
    qreal originalZ = bottomItem->zValue();

    QList<CGraphicsItem *> items;
    items.append(bottomItem);

    scene->moveItemsZUp(items, -1);

    EXPECT_GT(bottomItem->zValue(), middleItem->zValue());
    EXPECT_GT(bottomItem->zValue(), originalZ);
}

// =========================================================================
// sortZBaseOneBzItem tests
// Branches: assert(items.contains(pBaseItem)), grouping logic,
//           baszZIndex assignment, groupListIndexList, markZDirty
// =========================================================================

TEST(PageSceneSortZBaseOneBzItem, WithBaseItem_ReordersZ)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(100, 100), QPoint(200, 200), false);
    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(300, 300), QPoint(400, 400), false);
    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(500, 500), QPoint(600, 600), false);

    ASSERT_GE(scene->getBzItems().count(), 3);

    auto bzItems = scene->getBzItems();
    CGraphicsItem *topItem = bzItems[0];
    CGraphicsItem *middleItem = bzItems[1];
    CGraphicsItem *bottomItem = bzItems[2];

    qreal middleOriginalZ = middleItem->zValue();

    QList<CGraphicsItem *> items;
    items.append(topItem);
    items.append(bottomItem);

    scene->sortZBaseOneBzItem(items, topItem);

    EXPECT_GE(topItem->zValue(), middleItem->zValue());
    EXPECT_GE(bottomItem->zValue(), middleItem->zValue());
    EXPECT_LT(middleItem->zValue(), middleOriginalZ);
}

TEST(PageSceneSortZBaseOneBzItem, AllItemsInGroup_PreservesZOrder)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(100, 100), QPoint(200, 200), false);
    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(300, 300), QPoint(400, 400), false);
    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(500, 500), QPoint(600, 600), false);

    ASSERT_GE(scene->getBzItems().count(), 3);

    auto bzItems = scene->getBzItems();
    CGraphicsItem *topItem = bzItems[0];
    CGraphicsItem *middleItem = bzItems[1];
    CGraphicsItem *bottomItem = bzItems[2];

    qreal topZ = topItem->zValue();
    qreal middleZ = middleItem->zValue();
    qreal bottomZ = bottomItem->zValue();

    QList<CGraphicsItem *> items;
    items.append(topItem);
    items.append(middleItem);
    items.append(bottomItem);

    scene->sortZBaseOneBzItem(items, topItem);

    EXPECT_EQ(topItem->zValue(), topZ);
    EXPECT_EQ(middleItem->zValue(), middleZ);
    EXPECT_EQ(bottomItem->zValue(), bottomZ);
}
