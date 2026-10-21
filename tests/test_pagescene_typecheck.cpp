// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for PageScene static type-checking methods (issue V-6027).
 *
 * Target methods:
 *   - isBussizeItem(QGraphicsItem *pItem)
 *   - isBussizeHandleNodeItem(QGraphicsItem *pItem)
 *   - isNormalGroupItem(QGraphicsItem *pItem)
 *   - isDrawItem(QGraphicsItem *pItem)
 *   - getAssociatedBzItem(QGraphicsItem *pItem)
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#define protected public
#define private public
#include "drawshape/cdrawscene.h"
#include "drawshape/drawItems/bzItems/cgraphicsitem.h"
#include "drawshape/drawItems/bzItems/cgraphicsrectitem.h"
#include "drawshape/drawItems/bzItems/cgraphicsellipseitem.h"
#include "drawshape/drawItems/cgraphicsitemselectedmgr.h"
#include "drawshape/drawItems/csizehandlerect.h"
#include "drawshape/drawItems/cgraphicslayer.h"
#include "drawshape/globaldefine.h"
#include "publicApi.h"
#undef protected
#undef private

// =========================================================================
// isBussizeItem
// =========================================================================

TEST(PageSceneIsBussizeItem, Nullptr_ReturnsFalse)
{
    EXPECT_FALSE(PageScene::isBussizeItem(nullptr));
}

TEST(PageSceneIsBussizeItem, BzItem_ReturnsTrue)
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

    EXPECT_TRUE(PageScene::isBussizeItem(bzItems.first()));
}

TEST(PageSceneIsBussizeItem, GroupItem_ReturnsFalse)
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
    EXPECT_FALSE(PageScene::isBussizeItem(pGroup));
}

TEST(PageSceneIsBussizeItem, HandleNode_ReturnsFalse)
{
    CGraphicsRectItem parentItem;
    CSizeHandleRect handle(&parentItem, CSizeHandleRect::LeftTop);
    EXPECT_FALSE(PageScene::isBussizeItem(&handle));
}

// =========================================================================
// isBussizeHandleNodeItem
// =========================================================================

TEST(PageSceneIsBussizeHandleNodeItem, Nullptr_ReturnsFalse)
{
    EXPECT_FALSE(PageScene::isBussizeHandleNodeItem(nullptr));
}

TEST(PageSceneIsBussizeHandleNodeItem, HandleNode_ReturnsTrue)
{
    CGraphicsRectItem parentItem;
    CSizeHandleRect handle(&parentItem, CSizeHandleRect::LeftTop);
    EXPECT_TRUE(PageScene::isBussizeHandleNodeItem(&handle));
}

TEST(PageSceneIsBussizeHandleNodeItem, BzItem_ReturnsFalse)
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

    EXPECT_FALSE(PageScene::isBussizeHandleNodeItem(bzItems.first()));
}

TEST(PageSceneIsBussizeHandleNodeItem, GroupItem_ReturnsFalse)
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
    EXPECT_FALSE(PageScene::isBussizeHandleNodeItem(pGroup));
}

// =========================================================================
// isNormalGroupItem
// =========================================================================

TEST(PageSceneIsNormalGroupItem, Nullptr_ReturnsFalse)
{
    EXPECT_FALSE(PageScene::isNormalGroupItem(nullptr));
}

TEST(PageSceneIsNormalGroupItem, BzItem_ReturnsFalse)
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

    EXPECT_FALSE(PageScene::isNormalGroupItem(bzItems.first()));
}

TEST(PageSceneIsNormalGroupItem, NormalGroup_ReturnsTrue)
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
    EXPECT_TRUE(PageScene::isNormalGroupItem(pGroup));
}

TEST(PageSceneIsNormalGroupItem, SelectGroup_ReturnsFalse)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    CGraphicsItemGroup *selGroup = scene->selectGroup();
    ASSERT_NE(selGroup, nullptr);
    EXPECT_FALSE(PageScene::isNormalGroupItem(selGroup));
}

// =========================================================================
// isDrawItem
// =========================================================================

TEST(PageSceneIsDrawItem, Nullptr_ReturnsFalse)
{
    EXPECT_FALSE(PageScene::isDrawItem(nullptr));
}

TEST(PageSceneIsDrawItem, BzItem_ReturnsTrue)
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

    EXPECT_TRUE(PageScene::isDrawItem(bzItems.first()));
}

TEST(PageSceneIsDrawItem, NormalGroup_ReturnsTrue)
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
    EXPECT_TRUE(PageScene::isDrawItem(pGroup));
}

TEST(PageSceneIsDrawItem, HandleNode_ReturnsFalse)
{
    CGraphicsRectItem parentItem;
    CSizeHandleRect handle(&parentItem, CSizeHandleRect::LeftTop);
    EXPECT_FALSE(PageScene::isDrawItem(&handle));
}

// =========================================================================
// getAssociatedBzItem
// =========================================================================

TEST(PageSceneGetAssociatedBzItem, Nullptr_ReturnsNullptr)
{
    EXPECT_EQ(PageScene::getAssociatedBzItem(nullptr), nullptr);
}

TEST(PageSceneGetAssociatedBzItem, BzItem_ReturnsItself)
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

    CGraphicsItem *result = PageScene::getAssociatedBzItem(bzItems.first());
    EXPECT_EQ(result, bzItems.first());
}

TEST(PageSceneGetAssociatedBzItem, HandleNode_ReturnsParentBzItem)
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
    CGraphicsItem *bzItem = bzItems.first();

    QList<CSizeHandleRect *> handles = bzItem->handleNodes();
    if (handles.isEmpty())
        GTEST_SKIP() << "No handle nodes available on this item";

    CGraphicsItem *result = PageScene::getAssociatedBzItem(handles.first());
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result, bzItem);
}

TEST(PageSceneGetAssociatedBzItem, GroupItem_ReturnsNullptr)
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
    EXPECT_EQ(PageScene::getAssociatedBzItem(pGroup), nullptr);
}
