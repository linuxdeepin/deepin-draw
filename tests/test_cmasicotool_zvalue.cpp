// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for IBlurTool Z-value save/restore (issue V-9255 area 4)
 *
 * Source: src/drawshape/drawTools/cmasicotool.cpp
 *
 * Target methods:
 *   IBlurTool::saveZ           — scene root items → saveItemZValue
 *   IBlurTool::saveItemZValue  — isBzItem direct insert, isBzGroup recursive
 *   IBlurTool::restoreZ        — restore z values, clear _tempZs, JDynamicLayer branch
 *   IBlurTool::getBlurEnableItems — isBzGroup children + isBlurEnable, single item
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <memory>

#define protected public
#define private public
#include "drawshape/drawTools/idrawtool.h"
#include "drawshape/drawTools/cdrawtoolfactory.h"
#include "drawshape/drawTools/cmasicotool.h"
#include "drawshape/globaldefine.h"
#include "cattributemanagerwgt.h"
#include "publicApi.h"
#undef protected
#undef private

#include "test_helpers.h"

// =========================================================================
// IBlurTool::saveItemZValue — isBzItem direct insert
// =========================================================================

TEST(IBlurToolZValue, SaveItemZValue_BzItem_InsertsIntoTempZs)
{
    ensurePageExists();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    std::unique_ptr<IDrawTool> tool(CDrawToolFactory::Create(blur));
    ASSERT_NE(tool, nullptr);
    tool->setDrawBoard(board);
    auto *blurTool = static_cast<IBlurTool *>(tool.get());

    auto *scene = board->currentPage()->scene();
    ASSERT_NE(scene, nullptr);

    CGraphicsRectItem *rectItem = new CGraphicsRectItem(QRectF(0, 0, 100, 100));
    scene->addItem(rectItem);
    rectItem->setZValue(5.0);

    int sizeBefore = blurTool->_tempZs.size();
    blurTool->saveItemZValue(rectItem);

    EXPECT_EQ(blurTool->_tempZs.size(), sizeBefore + 1);
    EXPECT_TRUE(blurTool->_tempZs.contains(rectItem));

    scene->removeItem(rectItem);
    delete rectItem;
}

TEST(IBlurToolZValue, SaveItemZValue_BzItem_SavesCorrectZValue)
{
    ensurePageExists();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    std::unique_ptr<IDrawTool> tool(CDrawToolFactory::Create(blur));
    ASSERT_NE(tool, nullptr);
    tool->setDrawBoard(board);
    auto *blurTool = static_cast<IBlurTool *>(tool.get());

    auto *scene = board->currentPage()->scene();
    ASSERT_NE(scene, nullptr);

    CGraphicsRectItem *rectItem = new CGraphicsRectItem(QRectF(0, 0, 50, 50));
    scene->addItem(rectItem);
    rectItem->setZValue(42.0);

    blurTool->saveItemZValue(rectItem);
    EXPECT_EQ(blurTool->_tempZs.value(rectItem), 42.0);

    scene->removeItem(rectItem);
    delete rectItem;
}

// =========================================================================
// IBlurTool::saveZ — populates _tempZs from scene root items
// =========================================================================

TEST(IBlurToolZValue, SaveZ_PopulatesTempZs)
{
    ensurePageExists();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    std::unique_ptr<IDrawTool> tool(CDrawToolFactory::Create(blur));
    ASSERT_NE(tool, nullptr);
    tool->setDrawBoard(board);
    auto *blurTool = static_cast<IBlurTool *>(tool.get());

    auto *scene = board->currentPage()->scene();
    ASSERT_NE(scene, nullptr);

    CGraphicsRectItem *item1 = new CGraphicsRectItem(QRectF(0, 0, 50, 50));
    CGraphicsRectItem *item2 = new CGraphicsRectItem(QRectF(100, 100, 50, 50));
    scene->addItem(item1);
    scene->addItem(item2);
    item1->setZValue(3.0);
    item2->setZValue(7.0);

    blurTool->_tempZs.clear();
    blurTool->saveZ(scene);

    EXPECT_FALSE(blurTool->_tempZs.isEmpty());
    EXPECT_GE(blurTool->_tempZs.size(), 2);

    scene->removeItem(item1);
    scene->removeItem(item2);
    delete item1;
    delete item2;
}

TEST(IBlurToolZValue, SaveZ_ClearsTempZsBeforePopulating)
{
    ensurePageExists();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    std::unique_ptr<IDrawTool> tool(CDrawToolFactory::Create(blur));
    ASSERT_NE(tool, nullptr);
    tool->setDrawBoard(board);
    auto *blurTool = static_cast<IBlurTool *>(tool.get());

    auto *scene = board->currentPage()->scene();
    ASSERT_NE(scene, nullptr);

    // Pre-populate _tempZs with stale data
    blurTool->_tempZs.insert(nullptr, 99.0);
    EXPECT_FALSE(blurTool->_tempZs.isEmpty());

    blurTool->saveZ(scene);

    // saveZ clears _tempZs first, so the stale entry should be gone
    EXPECT_FALSE(blurTool->_tempZs.contains(nullptr));
}

// =========================================================================
// IBlurTool::restoreZ — restores z values and clears _tempZs
// =========================================================================

TEST(IBlurToolZValue, RestoreZ_RestoresZValues)
{
    ensurePageExists();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    std::unique_ptr<IDrawTool> tool(CDrawToolFactory::Create(blur));
    ASSERT_NE(tool, nullptr);
    tool->setDrawBoard(board);
    auto *blurTool = static_cast<IBlurTool *>(tool.get());

    auto *scene = board->currentPage()->scene();
    ASSERT_NE(scene, nullptr);

    CGraphicsRectItem *rectItem = new CGraphicsRectItem(QRectF(0, 0, 100, 100));
    scene->addItem(rectItem);
    rectItem->setZValue(10.0);

    // Save the z value
    blurTool->saveItemZValue(rectItem);
    EXPECT_EQ(blurTool->_tempZs.value(rectItem), 10.0);

    // Change the z value
    rectItem->setZValue(99.0);
    EXPECT_EQ(rectItem->drawZValue(), 99.0);

    // Restore — should set it back to 10.0
    blurTool->restoreZ();
    EXPECT_EQ(rectItem->drawZValue(), 10.0);

    // _tempZs should be cleared after restore
    EXPECT_TRUE(blurTool->_tempZs.isEmpty());

    scene->removeItem(rectItem);
    delete rectItem;
}

TEST(IBlurToolZValue, RestoreZ_ClearsTempZs)
{
    ensurePageExists();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    std::unique_ptr<IDrawTool> tool(CDrawToolFactory::Create(blur));
    ASSERT_NE(tool, nullptr);
    tool->setDrawBoard(board);
    auto *blurTool = static_cast<IBlurTool *>(tool.get());

    auto *scene = board->currentPage()->scene();
    ASSERT_NE(scene, nullptr);

    CGraphicsRectItem *rectItem = new CGraphicsRectItem(QRectF(0, 0, 50, 50));
    scene->addItem(rectItem);
    rectItem->setZValue(5.0);

    blurTool->saveItemZValue(rectItem);
    EXPECT_FALSE(blurTool->_tempZs.isEmpty());

    blurTool->restoreZ();
    EXPECT_TRUE(blurTool->_tempZs.isEmpty());

    scene->removeItem(rectItem);
    delete rectItem;
}

TEST(IBlurToolZValue, RestoreZ_EmptyTempZs_NoCrash)
{
    std::unique_ptr<IDrawTool> tool(CDrawToolFactory::Create(blur));
    ASSERT_NE(tool, nullptr);
    auto *blurTool = static_cast<IBlurTool *>(tool.get());

    blurTool->_tempZs.clear();
    blurTool->restoreZ();
    EXPECT_TRUE(blurTool->_tempZs.isEmpty());
}

// =========================================================================
// IBlurTool::getBlurEnableItems — single item (not group)
// =========================================================================

TEST(IBlurToolZValue, GetBlurEnabledItems_NullItem_ReturnsEmpty)
{
    std::unique_ptr<IDrawTool> tool(CDrawToolFactory::Create(blur));
    ASSERT_NE(tool, nullptr);
    auto *blurTool = static_cast<IBlurTool *>(tool.get());

    QList<CGraphicsItem *> result = blurTool->getBlurEnableItems(nullptr);
    EXPECT_TRUE(result.isEmpty());
}

TEST(IBlurToolZValue, GetBlurEnabledItems_NonBlurSingleItem_ReturnsEmpty)
{
    ensurePageExists();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    std::unique_ptr<IDrawTool> tool(CDrawToolFactory::Create(blur));
    ASSERT_NE(tool, nullptr);
    tool->setDrawBoard(board);
    auto *blurTool = static_cast<IBlurTool *>(tool.get());

    auto *scene = board->currentPage()->scene();
    ASSERT_NE(scene, nullptr);

    CGraphicsRectItem *rectItem = new CGraphicsRectItem(QRectF(0, 0, 100, 100));
    scene->addItem(rectItem);

    // Rect items are not blur-enabled by default
    QList<CGraphicsItem *> result = blurTool->getBlurEnableItems(rectItem);
    EXPECT_TRUE(result.isEmpty());

    scene->removeItem(rectItem);
    delete rectItem;
}
