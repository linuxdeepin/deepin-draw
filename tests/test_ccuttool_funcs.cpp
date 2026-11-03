// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CCutTool methods (issue V-7429)
 *
 * Source: src/drawshape/drawTools/ccuttool.cpp
 *
 * Target methods:
 *   CCutTool::createCutItem(PageScene *scene)
 *   CCutTool::deleteCutItem(PageScene *scene)
 *   CCutTool::changeCutType(int type, PageScene *scene)
 *   CCutTool::changeCutSize(const PageScene *scene, const QSize &size)
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#define protected public
#define private public
#include "drawshape/drawTools/ccuttool.h"
#include "drawshape/drawTools/cdrawtoolfactory.h"
#include "drawshape/drawTools/idrawtool.h"
#include "drawshape/globaldefine.h"
#include "drawshape/cdrawscene.h"
#include "cgraphicscutitem.h"
#include "publicApi.h"
#undef protected
#undef private

// =========================================================================
// Helper: create a CCutTool and set its drawBoard
// =========================================================================
static CCutTool *createCutToolForTest()
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    CCutTool *tool = qobject_cast<CCutTool *>(CDrawToolFactory::Create(cut));
    if (tool)
        tool->setDrawBoard(board);
    return tool;
}

static PageScene *getTestScene()
{
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    auto *page = board->currentPage();
    return page ? page->scene() : nullptr;
}

// =========================================================================
// CCutTool::createCutItem
// =========================================================================

TEST(CCutToolCreateCutItem, CreatesCutItemInScene)
{
    CCutTool *tool = createCutToolForTest();
    ASSERT_NE(tool, nullptr);
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    tool->createCutItem(scene);
    EXPECT_TRUE(tool->m_cutItems.contains(scene));
    EXPECT_NE(tool->m_cutItems[scene], nullptr);

    // Cleanup
    tool->deleteCutItem(scene);
    delete tool;
}

TEST(CCutToolCreateCutItem, DoesNotDuplicateForSameScene)
{
    CCutTool *tool = createCutToolForTest();
    ASSERT_NE(tool, nullptr);
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    tool->createCutItem(scene);
    auto firstItem = tool->m_cutItems[scene];

    // Calling again should not create a duplicate (contains check)
    tool->createCutItem(scene);
    EXPECT_EQ(tool->m_cutItems.count(scene), 1);

    tool->deleteCutItem(scene);
    delete tool;
}

TEST(CCutToolCreateCutItem, SetsFreeMode)
{
    CCutTool *tool = createCutToolForTest();
    ASSERT_NE(tool, nullptr);
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    tool->createCutItem(scene);
    ASSERT_NE(tool->m_pCutItem, nullptr);
    EXPECT_TRUE(tool->m_pCutItem->isFreeMode());

    tool->deleteCutItem(scene);
    delete tool;
}

// =========================================================================
// CCutTool::deleteCutItem
// =========================================================================

TEST(CCutToolDeleteCutItem, RemovesCutItemFromScene)
{
    CCutTool *tool = createCutToolForTest();
    ASSERT_NE(tool, nullptr);
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    tool->createCutItem(scene);
    EXPECT_TRUE(tool->m_cutItems.contains(scene));

    tool->deleteCutItem(scene);
    EXPECT_FALSE(tool->m_cutItems.contains(scene));
    delete tool;
}

TEST(CCutToolDeleteCutItem, NoCrashWhenNoCutItem)
{
    CCutTool *tool = createCutToolForTest();
    ASSERT_NE(tool, nullptr);
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    // No cut item exists; should not crash
    tool->deleteCutItem(scene);
    EXPECT_FALSE(tool->m_cutItems.contains(scene));
    delete tool;
}

TEST(CCutToolDeleteCutItem, ClearsCurrentCutItemPointer)
{
    CCutTool *tool = createCutToolForTest();
    ASSERT_NE(tool, nullptr);
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    tool->createCutItem(scene);
    EXPECT_NE(tool->m_pCutItem, nullptr);

    tool->deleteCutItem(scene);
    EXPECT_EQ(tool->m_pCutItem, nullptr);
    delete tool;
}

// =========================================================================
// CCutTool::changeCutType
// =========================================================================

TEST(CCutToolChangeCutType, FreeModeReturnsNonZeroSize)
{
    CCutTool *tool = createCutToolForTest();
    ASSERT_NE(tool, nullptr);
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    tool->createCutItem(scene);

    QSizeF result = tool->changeCutType(cut_free, scene);
    EXPECT_NE(result, QSizeF(0, 0));
    EXPECT_TRUE(tool->m_bModify);

    tool->deleteCutItem(scene);
    delete tool;
}

TEST(CCutToolChangeCutType, RatioTypeReturnsNonZeroSize)
{
    CCutTool *tool = createCutToolForTest();
    ASSERT_NE(tool, nullptr);
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    tool->createCutItem(scene);

    QSizeF result = tool->changeCutType(cut_1_1, scene);
    EXPECT_NE(result, QSizeF(0, 0));

    tool->deleteCutItem(scene);
    delete tool;
}

TEST(CCutToolChangeCutType, NoCutItemReturnsZeroSize)
{
    CCutTool *tool = createCutToolForTest();
    ASSERT_NE(tool, nullptr);
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    // No cut item created; getCutItem returns nullptr
    QSizeF result = tool->changeCutType(cut_free, scene);
    EXPECT_EQ(result, QSizeF(0, 0));
    delete tool;
}

// =========================================================================
// CCutTool::changeCutSize
// =========================================================================

TEST(CCutToolChangeCutSize, UpdatesSize)
{
    CCutTool *tool = createCutToolForTest();
    ASSERT_NE(tool, nullptr);
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    tool->createCutItem(scene);
    ASSERT_NE(tool->m_pCutItem, nullptr);

    QSize newSize(200, 150);
    tool->changeCutSize(scene, newSize);

    EXPECT_EQ(tool->m_pCutItem->rect().size().toSize(), newSize);
    EXPECT_TRUE(tool->m_bModify);

    tool->deleteCutItem(scene);
    delete tool;
}

TEST(CCutToolChangeCutSize, NoCrashWithoutCutItem)
{
    CCutTool *tool = createCutToolForTest();
    ASSERT_NE(tool, nullptr);
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    // No cut item exists; should not crash
    tool->changeCutSize(scene, QSize(100, 100));
    SUCCEED();
    delete tool;
}
