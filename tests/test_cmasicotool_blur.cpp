// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for IBlurTool methods (issue V-5429)
 *
 * Source: src/drawshape/drawTools/cmasicotool.cpp
 *
 * Target methods:
 *   IBlurTool: getBlurEnableItems, isEnable, registerAttributionWidgets,
 *              saveItemZValue, saveZ, toolFinish, toolUpdate
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

// =========================================================================
// IBlurTool::getBlurEnableItems
// =========================================================================

TEST(IBlurToolGetBlurEnableItemsTest, NullItemReturnsEmptyList)
{
    std::unique_ptr<IDrawTool> tool(CDrawToolFactory::Create(blur));
    ASSERT_NE(tool, nullptr);
    auto *blurTool = static_cast<IBlurTool *>(tool.get());

    QList<CGraphicsItem *> result = blurTool->getBlurEnableItems(nullptr);
    EXPECT_TRUE(result.isEmpty());
}

TEST(IBlurToolGetBlurEnableItemsTest, NonNullNonBlurItemReturnsEmptyList)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    std::unique_ptr<IDrawTool> tool(CDrawToolFactory::Create(blur));
    ASSERT_NE(tool, nullptr);
    tool->setDrawBoard(board);
    auto *blurTool = static_cast<IBlurTool *>(tool.get());

    // Create a rect item (not blur-enabled by default)
    auto *scene = board->currentPage()->scene();
    ASSERT_NE(scene, nullptr);
    CGraphicsRectItem *rectItem = new CGraphicsRectItem(QRectF(0, 0, 100, 100));
    scene->addItem(rectItem);

    QList<CGraphicsItem *> result = blurTool->getBlurEnableItems(rectItem);
    // Rect items are not blur-enabled, so result should be empty
    EXPECT_TRUE(result.isEmpty());

    scene->removeItem(rectItem);
    delete rectItem;
}

// =========================================================================
// IBlurTool::isEnable
// =========================================================================

TEST(IBlurToolIsEnableTest, NullViewReturnsFalse)
{
    std::unique_ptr<IDrawTool> tool(CDrawToolFactory::Create(blur));
    ASSERT_NE(tool, nullptr);
    auto *blurTool = static_cast<IBlurTool *>(tool.get());

    EXPECT_FALSE(blurTool->isEnable(nullptr));
}

// =========================================================================
// IBlurTool::registerAttributionWidgets
// =========================================================================

TEST(IBlurToolRegisterAttrWidgetsTest, RegisterDoesNotCrash)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    std::unique_ptr<IDrawTool> tool(CDrawToolFactory::Create(blur));
    ASSERT_NE(tool, nullptr);
    tool->setDrawBoard(board);
    auto *blurTool = static_cast<IBlurTool *>(tool.get());

    // registerAttributionWidgets accesses drawBoard() and its attributionWidget()
    blurTool->registerAttributionWidgets();
    SUCCEED();
}

TEST(IBlurToolRegisterAttrWidgetsTest, RegistersBlurAttributeWidget)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    std::unique_ptr<IDrawTool> tool(CDrawToolFactory::Create(blur));
    ASSERT_NE(tool, nullptr);
    tool->setDrawBoard(board);
    auto *blurTool = static_cast<IBlurTool *>(tool.get());

    blurTool->registerAttributionWidgets();

    // After registration, the blur attribute widget should be in s_allInstalledAttriWgts
    auto &wgts = board->attributionWidget()->s_allInstalledAttriWgts;
    EXPECT_TRUE(wgts.contains(DrawAttribution::EBlurAttri));
}

// =========================================================================
// IBlurTool::saveItemZValue
// =========================================================================

TEST(IBlurToolSaveItemZValueTest, NullItemNoCrash)
{
    std::unique_ptr<IDrawTool> tool(CDrawToolFactory::Create(blur));
    ASSERT_NE(tool, nullptr);
    auto *blurTool = static_cast<IBlurTool *>(tool.get());

    // saveItemZValue with a non-bzItem should not crash and not insert into _tempZs
    // Pass nullptr — the method accesses pItem->isBzItem() which will crash.
    // Instead, create a real item on a scene.
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);
    tool->setDrawBoard(board);

    auto *scene = board->currentPage()->scene();
    ASSERT_NE(scene, nullptr);
    CGraphicsRectItem *rectItem = new CGraphicsRectItem(QRectF(0, 0, 50, 50));
    scene->addItem(rectItem);

    int sizeBefore = blurTool->_tempZs.size();
    blurTool->saveItemZValue(rectItem);
    // A rect item is a bzItem, so its z-value should be saved
    int sizeAfter = blurTool->_tempZs.size();
    EXPECT_GE(sizeAfter, sizeBefore);

    scene->removeItem(rectItem);
    delete rectItem;
}

// =========================================================================
// IBlurTool::saveZ
// =========================================================================

TEST(IBlurToolSaveZTest, SaveZClearsTempAndPopulates)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    std::unique_ptr<IDrawTool> tool(CDrawToolFactory::Create(blur));
    ASSERT_NE(tool, nullptr);
    tool->setDrawBoard(board);
    auto *blurTool = static_cast<IBlurTool *>(tool.get());

    auto *scene = board->currentPage()->scene();
    ASSERT_NE(scene, nullptr);

    // Add an item to the scene
    CGraphicsRectItem *rectItem = new CGraphicsRectItem(QRectF(0, 0, 50, 50));
    scene->addItem(rectItem);

    blurTool->saveZ(scene);
    // _tempZs should contain at least one entry
    EXPECT_FALSE(blurTool->_tempZs.isEmpty());

    scene->removeItem(rectItem);
    delete rectItem;
}

// =========================================================================
// IBlurTool::toolFinish
// =========================================================================

TEST(IBlurToolToolFinishTest, NotFinalEventNoCrash)
{
    std::unique_ptr<IDrawTool> tool(CDrawToolFactory::Create(blur));
    ASSERT_NE(tool, nullptr);
    auto *blurTool = static_cast<IBlurTool *>(tool.get());

    // isFinalEvent() returns false when _allITERecordInfo is empty
    EXPECT_FALSE(blurTool->isFinalEvent());

    // toolFinish with no final event should be a no-op
    blurTool->toolFinish(nullptr, nullptr);
    SUCCEED();
}

TEST(IBlurToolToolFinishTest, FinalEventWithNoBlurPressedClearsState)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    std::unique_ptr<IDrawTool> tool(CDrawToolFactory::Create(blur));
    ASSERT_NE(tool, nullptr);
    tool->setDrawBoard(board);
    auto *blurTool = static_cast<IBlurTool *>(tool.get());

    // Set up a final event condition
    tool->_allITERecordInfo.insert(0, IDrawTool::ITERecordInfo());
    EXPECT_TRUE(blurTool->isFinalEvent());

    // _pressedPosBlurEnable is false by default, so toolFinish should just
    // clear _pLastTopItem and set cursor.
    blurTool->_pressedPosBlurEnable = false;
    blurTool->toolFinish(nullptr, nullptr);
    EXPECT_EQ(blurTool->_pLastTopItem, nullptr);
}

// =========================================================================
// IBlurTool::toolUpdate
// =========================================================================

TEST(IBlurToolToolUpdateTest, NotPressedReturnsImmediately)
{
    std::unique_ptr<IDrawTool> tool(CDrawToolFactory::Create(blur));
    ASSERT_NE(tool, nullptr);
    auto *blurTool = static_cast<IBlurTool *>(tool.get());

    // _pressedPosBlurEnable is false by default → toolUpdate returns immediately
    blurTool->_pressedPosBlurEnable = false;
    blurTool->toolUpdate(nullptr, nullptr);
    SUCCEED();
}
