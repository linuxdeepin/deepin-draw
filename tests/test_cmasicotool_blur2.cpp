// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for IBlurTool::restoreZ (issue V-7429)
 *
 * Source: src/drawshape/drawTools/cmasicotool.cpp
 *
 * Target methods:
 *   IBlurTool::restoreZ()
 *
 * Guard: TEST_BLUR_ITEM (same as test_cmasicotool_blur.cpp)
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
#include "cgraphicsrectitem.h"
#include "publicApi.h"
#undef protected
#undef private

#if TEST_BLUR_ITEM

// =========================================================================
// Helper: create an IBlurTool with drawBoard set
// =========================================================================
static IBlurTool *createBlurTool2()
{
    IBlurTool *tool = static_cast<IBlurTool *>(CDrawToolFactory::Create(blur));
    tool->setDrawBoard(getMainWindow()->drawBoard());
    return tool;
}

// =========================================================================
// IBlurTool::restoreZ
// =========================================================================

TEST(IBlurToolRestoreZ2, EmptyTempZsNoCrash)
{
    createNewViewByShortcutKey();
    IBlurTool *tool = createBlurTool2();
    ASSERT_NE(tool, nullptr);

    tool->_tempZs.clear();
    tool->restoreZ();
    EXPECT_TRUE(tool->_tempZs.isEmpty());

    delete tool;
}

TEST(IBlurToolRestoreZ2, RestoresZValues)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);
    auto *page = board->currentPage();
    ASSERT_NE(page, nullptr);
    auto *scene = page->scene();
    ASSERT_NE(scene, nullptr);

    IBlurTool *tool = createBlurTool2();
    ASSERT_NE(tool, nullptr);

    // Create a rectangle item and add it to the scene
    CGraphicsRectItem *rectItem = new CGraphicsRectItem(QRectF(50, 50, 100, 100));
    scene->addCItem(rectItem);

    // Save original z value
    qreal originalZ = rectItem->drawZValue();

    // Save Z values (populates _tempZs)
    tool->saveZ(scene);
    EXPECT_FALSE(tool->_tempZs.isEmpty());

    // Modify the z value
    rectItem->setZValue(originalZ + 200.0);

    // Restore Z values
    tool->restoreZ();
    EXPECT_EQ(rectItem->drawZValue(), originalZ);
    EXPECT_TRUE(tool->_tempZs.isEmpty());

    delete tool;
}

TEST(IBlurToolRestoreZ2, ClearsTempZsAfterRestore)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);
    auto *page = board->currentPage();
    ASSERT_NE(page, nullptr);
    auto *scene = page->scene();
    ASSERT_NE(scene, nullptr);

    IBlurTool *tool = createBlurTool2();
    ASSERT_NE(tool, nullptr);

    CGraphicsRectItem *rectItem = new CGraphicsRectItem(QRectF(50, 50, 100, 100));
    scene->addCItem(rectItem);

    tool->saveZ(scene);
    EXPECT_FALSE(tool->_tempZs.isEmpty());

    tool->restoreZ();
    EXPECT_TRUE(tool->_tempZs.isEmpty());

    delete tool;
}

TEST(IBlurToolRestoreZ2, MultipleItemsAllRestored)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);
    auto *page = board->currentPage();
    ASSERT_NE(page, nullptr);
    auto *scene = page->scene();
    ASSERT_NE(scene, nullptr);

    IBlurTool *tool = createBlurTool2();
    ASSERT_NE(tool, nullptr);

    CGraphicsRectItem *rect1 = new CGraphicsRectItem(QRectF(10, 10, 50, 50));
    CGraphicsRectItem *rect2 = new CGraphicsRectItem(QRectF(70, 70, 50, 50));
    scene->addCItem(rect1);
    scene->addCItem(rect2);

    qreal z1 = rect1->drawZValue();
    qreal z2 = rect2->drawZValue();

    tool->saveZ(scene);
    EXPECT_GE(tool->_tempZs.size(), 2);

    rect1->setZValue(z1 + 50.0);
    rect2->setZValue(z2 + 50.0);

    tool->restoreZ();
    EXPECT_EQ(rect1->drawZValue(), z1);
    EXPECT_EQ(rect2->drawZValue(), z2);
    EXPECT_TRUE(tool->_tempZs.isEmpty());

    delete tool;
}

#endif // TEST_BLUR_ITEM
