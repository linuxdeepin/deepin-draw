// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for PageScene group-related methods (issue V-6027).
 *
 * Target methods:
 *   - isGroupable(pBzItems)
 *   - isUnGroupable(pBzItems)
 *   - copyCreatGroup(pGroup)
 *   - destoryGroup(pGroup, deleteIt, pushUndo)
 *   - destoryAllGroup(deleteIt, pushUndo)
 *   - bzGroups()
 *   - loadGroupTree(info)
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#define protected public
#define private public
#include "drawshape/cdrawscene.h"
#include "drawshape/drawItems/bzItems/cgraphicsitem.h"
#include "drawshape/drawItems/bzItems/cgraphicsrectitem.h"
#include "drawshape/drawItems/cgraphicsitemselectedmgr.h"
#include "drawshape/globaldefine.h"
#include "publicApi.h"
#undef protected
#undef private

#include <QTest>
#include <QTestEventList>

// Helper: create a group from selected items
static CGraphicsItemGroup *createGroupFromItems(PageScene *scene, PageView *view)
{
    view->slotOnSelectAll();
    DTestEventList e;
    e.addKeyPress(Qt::Key_G, Qt::ControlModifier, 100);
    e.addKeyRelease(Qt::Key_G, Qt::ControlModifier, 100);
    e.simulate(view->viewport());
    if (scene->m_pGroups.count() > 0) {
        return scene->m_pGroups.first();
    }
    return nullptr;
}

// =========================================================================
// Test fixture — provides a fresh view/scene for each test
// =========================================================================

class PageSceneGroupTest : public ::testing::Test {
protected:
    void SetUp() override
    {
        createNewViewByShortcutKey();
        m_view = getCurView();
        ASSERT_NE(m_view, nullptr);
        m_scene = m_view->drawScene();
        ASSERT_NE(m_scene, nullptr);
    }

    void createTwoRects()
    {
        drawApp->setCurrentTool(rectangle);
        createItemByMouse(m_view, false, QPoint(100, 100), QPoint(200, 200), false);
        drawApp->setCurrentTool(rectangle);
        createItemByMouse(m_view, false, QPoint(300, 300), QPoint(400, 400), false);
    }

    PageView *m_view = nullptr;
    PageScene *m_scene = nullptr;
};

// =========================================================================
// isGroupable
// =========================================================================

TEST_F(PageSceneGroupTest, IsGroupable_EmptyList_ReturnsFalse)
{
    QList<CGraphicsItem *> emptyList;
    m_scene->clearSelectGroup();
    EXPECT_FALSE(m_scene->isGroupable(emptyList));
}

TEST_F(PageSceneGroupTest, IsGroupable_SingleItem_ReturnsFalse)
{
    drawApp->setCurrentTool(rectangle);
    createItemByMouse(m_view, false, QPoint(100, 100), QPoint(200, 200), false);

    auto bzItems = m_scene->getBzItems();
    ASSERT_EQ(bzItems.count(), 1);

    EXPECT_FALSE(m_scene->isGroupable(bzItems));
}

TEST_F(PageSceneGroupTest, IsGroupable_MultipleItems_ReturnsTrue)
{
    createTwoRects();

    auto bzItems = m_scene->getBzItems();
    ASSERT_GE(bzItems.count(), 2);

    EXPECT_TRUE(m_scene->isGroupable(bzItems));
}

// =========================================================================
// isUnGroupable
// =========================================================================

TEST_F(PageSceneGroupTest, IsUnGroupable_NoGroups_ReturnsFalse)
{
    createTwoRects();

    auto bzItems = m_scene->getBzItems();
    ASSERT_GE(bzItems.count(), 2);

    QList<CGraphicsItem *> emptyList;
    EXPECT_FALSE(m_scene->isUnGroupable(emptyList));
}

TEST_F(PageSceneGroupTest, IsUnGroupable_WithGroup_ReturnsTrue)
{
    createTwoRects();

    CGraphicsItemGroup *pGroup = createGroupFromItems(m_scene, m_view);
    ASSERT_NE(pGroup, nullptr);

    QList<CGraphicsItem *> emptyList;
    EXPECT_TRUE(m_scene->isUnGroupable(emptyList));
}

// =========================================================================
// bzGroups
// =========================================================================

TEST_F(PageSceneGroupTest, BzGroups_NoGroups_ReturnsEmpty)
{
    m_scene->resetScene();
    EXPECT_EQ(m_scene->bzGroups().count(), 0);
}

TEST_F(PageSceneGroupTest, BzGroups_WithGroup_ReturnsNonEmpty)
{
    createTwoRects();

    CGraphicsItemGroup *pGroup = createGroupFromItems(m_scene, m_view);
    ASSERT_NE(pGroup, nullptr);

    EXPECT_GE(m_scene->bzGroups().count(), 1);
}

// =========================================================================
// destoryGroup
// =========================================================================

TEST_F(PageSceneGroupTest, DestoryGroup_Nullptr_ReturnsImmediately)
{
    m_scene->destoryGroup(nullptr, false, false);
    SUCCEED();
}

TEST_F(PageSceneGroupTest, DestoryGroup_RemovesFromScene)
{
    createTwoRects();

    CGraphicsItemGroup *pGroup = createGroupFromItems(m_scene, m_view);
    ASSERT_NE(pGroup, nullptr);
    int groupCountBefore = m_scene->bzGroups().count();
    ASSERT_GE(groupCountBefore, 1);

    m_scene->destoryGroup(pGroup, false, false);
    EXPECT_EQ(m_scene->bzGroups().count(), groupCountBefore - 1);
}

// =========================================================================
// destoryAllGroup
// =========================================================================

TEST_F(PageSceneGroupTest, DestoryAllGroup_ClearsAllGroups)
{
    createTwoRects();

    CGraphicsItemGroup *pGroup = createGroupFromItems(m_scene, m_view);
    ASSERT_NE(pGroup, nullptr);
    ASSERT_GE(m_scene->bzGroups().count(), 1);

    m_scene->destoryAllGroup(false, false);
    EXPECT_EQ(m_scene->bzGroups().count(), 0);
}

// =========================================================================
// copyCreatGroup
// =========================================================================

TEST_F(PageSceneGroupTest, CopyCreatGroup_Nullptr_ReturnsNullptr)
{
    EXPECT_EQ(PageScene::copyCreatGroup(nullptr), nullptr);
}

TEST_F(PageSceneGroupTest, CopyCreatGroup_CopiesGroup)
{
    createTwoRects();

    CGraphicsItemGroup *pGroup = createGroupFromItems(m_scene, m_view);
    ASSERT_NE(pGroup, nullptr);
    int groupCountBefore = m_scene->bzGroups().count();

    CGraphicsItemGroup *pNewGroup = PageScene::copyCreatGroup(pGroup);
    ASSERT_NE(pNewGroup, nullptr);
    EXPECT_EQ(m_scene->bzGroups().count(), groupCountBefore + 1);
}

// =========================================================================
// loadGroupTree
// =========================================================================

TEST_F(PageSceneGroupTest, LoadGroupTree_VirRootGroup_DestroysAllGroups)
{
    createTwoRects();

    CGraphicsItemGroup *pGroup = createGroupFromItems(m_scene, m_view);
    ASSERT_NE(pGroup, nullptr);
    ASSERT_GE(m_scene->bzGroups().count(), 1);

    // Create a tree with EVirRootGroup type to trigger destoryAllGroup
    PageScene::CGroupBzItemsTree tree;
    tree.setGroupType(CGraphicsItemGroup::EVirRootGroup);
    m_scene->loadGroupTree(tree);

    EXPECT_EQ(m_scene->bzGroups().count(), 0);
}
