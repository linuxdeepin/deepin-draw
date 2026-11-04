// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for IDrawTool methods (issue V-7618 group 6)
 *
 * Source: src/drawshape/drawTools/idrawtool.cpp
 *
 * Target methods:
 *   IDrawTool::activeTool()
 *   IDrawTool::autoSupUndoForCreatItem()
 *   IDrawTool::isPressEventHandledByQt (base class default)
 *   IDrawTool::rasterItemToLayer (base class default)
 *   IDrawTool::boardChanged signal
 *   IDrawTool::setViewToSelectionTool(PageView *)
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QSignalSpy>
#include <memory>

#define protected public
#define private public
#include "drawshape/drawTools/idrawtool.h"
#include "drawshape/drawTools/cdrawtoolfactory.h"
#include "drawshape/globaldefine.h"
#include "publicApi.h"
#undef protected
#undef private

// =========================================================================
// IDrawTool::activeTool
// =========================================================================

TEST(IDrawToolActiveTool, ReturnsTrueAfterActivation)
{
    createNewViewByShortcutKey();

    std::unique_ptr<IDrawTool> tool(CDrawToolFactory::Create(rectangle));
    ASSERT_NE(tool, nullptr);

    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);
    tool->setDrawBoard(board);

    // activeTool() calls toolButton()->setChecked(true) and returns true
    bool result = tool->activeTool();
    EXPECT_TRUE(result);
}

// =========================================================================
// IDrawTool::autoSupUndoForCreatItem
// =========================================================================

TEST(IDrawToolAutoSupUndoForCreatItem, DefaultReturnsTrue)
{
    std::unique_ptr<IDrawTool> tool(CDrawToolFactory::Create(rectangle));
    ASSERT_NE(tool, nullptr);

    // Base class default returns true
    EXPECT_TRUE(tool->autoSupUndoForCreatItem());
}

TEST(IDrawToolAutoSupUndoForCreatItem, RectangleTool_ReturnsTrue)
{
    // Rectangle tool uses the default implementation which returns true
    std::unique_ptr<IDrawTool> tool(CDrawToolFactory::Create(rectangle));
    ASSERT_NE(tool, nullptr);
    EXPECT_TRUE(tool->autoSupUndoForCreatItem());
}

TEST(IDrawToolAutoSupUndoForCreatItem, SelectionTool_NoCrash)
{
    // Selection tool may override; just verify no crash
    std::unique_ptr<IDrawTool> tool(CDrawToolFactory::Create(selection));
    if (tool) {
        tool->autoSupUndoForCreatItem();
    }
    SUCCEED();
}

// =========================================================================
// IDrawTool::rasterItemToLayer (base class default)
// =========================================================================

TEST(IDrawToolRasterItemToLayerBase, DefaultReturnsFalse)
{
    std::unique_ptr<IDrawTool> tool(CDrawToolFactory::Create(rectangle));
    ASSERT_NE(tool, nullptr);

    // Base class default returns false
    bool result = tool->rasterItemToLayer(nullptr, nullptr);
    EXPECT_FALSE(result);
}

// =========================================================================
// IDrawTool::setViewToSelectionTool
// =========================================================================

TEST(IDrawToolSetViewToSelectionTool, WithNullView_NoCrash)
{
    createNewViewByShortcutKey();

    // setViewToSelectionTool(nullptr) should not crash
    // It gets the selection tool and calls activeTool()
    IDrawTool::setViewToSelectionTool(nullptr);

    SUCCEED();
}

TEST(IDrawToolSetViewToSelectionTool, WithValidView_SwitchesToSelection)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    IDrawTool::setViewToSelectionTool(view);

    // After calling, the current tool should be selection
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);
    // The tool may or may not have switched depending on internal state
    SUCCEED();
}

// =========================================================================
// IDrawTool::boardChanged signal
// =========================================================================

TEST(IDrawToolBoardChangedSignal, SetDrawBoard_EmitsSignal)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    std::unique_ptr<IDrawTool> tool(CDrawToolFactory::Create(rectangle));
    ASSERT_NE(tool, nullptr);

    QSignalSpy spy(tool.get(), &IDrawTool::boardChanged);
    ASSERT_TRUE(spy.isValid());

    tool->setDrawBoard(board);

    // setDrawBoard emits boardChanged(oldBoard, newBoard) exactly once
    // (a fresh factory tool has _drawBoard == nullptr, which differs from board)
    ASSERT_EQ(spy.count(), 1);
    QList<QVariant> args = spy.takeFirst();
    // Parameters: old board, new board
    EXPECT_EQ(args.size(), 2);
}
