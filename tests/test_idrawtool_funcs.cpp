// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for IDrawTool methods (issue V-5381)
 *
 * Source: src/drawshape/drawTools/idrawtool.cpp
 *
 * Target methods:
 *   IDrawTool: clearITE, currentPage, defaultAttriVar,
 *              drawBoard, isFirstEvent, setEnable,
 *              setTouchSensitiveRadius, status
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

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
// IDrawTool::status
// =========================================================================

TEST(IDrawToolStatus, DefaultStatus_IsEIdle)
{
    std::unique_ptr<IDrawTool> tool(CDrawToolFactory::Create(selection));
    ASSERT_NE(tool, nullptr);

    EXPECT_EQ(tool->status(), IDrawTool::EIdle);
}

TEST(IDrawToolStatus, AfterSetEnableTrue_StatusIsEIdle)
{
    std::unique_ptr<IDrawTool> tool(CDrawToolFactory::Create(rectangle));
    ASSERT_NE(tool, nullptr);

    tool->setEnable(true);
    EXPECT_EQ(tool->status(), IDrawTool::EIdle);
}

TEST(IDrawToolStatus, AfterSetEnableFalse_StatusIsEDisAbled)
{
    std::unique_ptr<IDrawTool> tool(CDrawToolFactory::Create(rectangle));
    ASSERT_NE(tool, nullptr);

    tool->setEnable(false);
    EXPECT_EQ(tool->status(), IDrawTool::EDisAbled);
}

// =========================================================================
// IDrawTool::setEnable
// =========================================================================

TEST(IDrawToolSetEnable, DisableThenReEnable_TogglesButton)
{
    std::unique_ptr<IDrawTool> tool(CDrawToolFactory::Create(rectangle));
    ASSERT_NE(tool, nullptr);

    QAbstractButton *btn = tool->toolButton();
    ASSERT_NE(btn, nullptr);

    tool->setEnable(false);
    EXPECT_FALSE(btn->isEnabled());

    tool->setEnable(true);
    EXPECT_TRUE(btn->isEnabled());
}

// =========================================================================
// IDrawTool::drawBoard / setDrawBoard
// =========================================================================

TEST(IDrawToolDrawBoard, DefaultDrawBoard_IsNull)
{
    std::unique_ptr<IDrawTool> tool(CDrawToolFactory::Create(selection));
    ASSERT_NE(tool, nullptr);

    EXPECT_EQ(tool->drawBoard(), nullptr);
}

TEST(IDrawToolDrawBoard, SetDrawBoard_ReturnsSamePointer)
{
    std::unique_ptr<IDrawTool> tool(CDrawToolFactory::Create(selection));
    ASSERT_NE(tool, nullptr);

    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    tool->setDrawBoard(board);
    EXPECT_EQ(tool->drawBoard(), board);
}

// =========================================================================
// IDrawTool::currentPage
// =========================================================================

TEST(IDrawToolCurrentPage, NoDrawBoard_ReturnsNull)
{
    std::unique_ptr<IDrawTool> tool(CDrawToolFactory::Create(selection));
    ASSERT_NE(tool, nullptr);

    EXPECT_EQ(tool->currentPage(), nullptr);
}

TEST(IDrawToolCurrentPage, WithDrawBoard_ReturnsCurrentPage)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    std::unique_ptr<IDrawTool> tool(CDrawToolFactory::Create(selection));
    ASSERT_NE(tool, nullptr);

    tool->setDrawBoard(board);
    Page *page = tool->currentPage();
    EXPECT_NE(page, nullptr);
}

// =========================================================================
// IDrawTool::setTouchSensitiveRadius
// =========================================================================

TEST(IDrawToolTouchSensitiveRadius, SetValue_UpdatesMember)
{
    std::unique_ptr<IDrawTool> tool(CDrawToolFactory::Create(selection));
    ASSERT_NE(tool, nullptr);

    EXPECT_EQ(tool->_touchSensitiveRadius, 10);

    tool->setTouchSensitiveRadius(25);
    EXPECT_EQ(tool->_touchSensitiveRadius, 25);

    tool->setTouchSensitiveRadius(0);
    EXPECT_EQ(tool->_touchSensitiveRadius, 0);
}

// =========================================================================
// IDrawTool::clearITE
// =========================================================================

TEST(IDrawToolClearITE, ClearsAllITERecordInfo)
{
    std::unique_ptr<IDrawTool> tool(CDrawToolFactory::Create(selection));
    ASSERT_NE(tool, nullptr);

    tool->_allITERecordInfo.insert(1, IDrawTool::ITERecordInfo());
    tool->_allITERecordInfo.insert(2, IDrawTool::ITERecordInfo());
    ASSERT_EQ(tool->_allITERecordInfo.size(), 2);

    tool->clearITE();
    EXPECT_TRUE(tool->_allITERecordInfo.isEmpty());
}

TEST(IDrawToolClearITE, EmptyMap_NoCrash)
{
    std::unique_ptr<IDrawTool> tool(CDrawToolFactory::Create(selection));
    ASSERT_NE(tool, nullptr);

    tool->clearITE();
    EXPECT_TRUE(tool->_allITERecordInfo.isEmpty());
}

// =========================================================================
// IDrawTool::isFirstEvent
// =========================================================================

TEST(IDrawToolIsFirstEvent, EmptyMap_ReturnsFalse)
{
    std::unique_ptr<IDrawTool> tool(CDrawToolFactory::Create(selection));
    ASSERT_NE(tool, nullptr);

    EXPECT_FALSE(tool->isFirstEvent());
}

TEST(IDrawToolIsFirstEvent, SingleEntry_ReturnsTrue)
{
    std::unique_ptr<IDrawTool> tool(CDrawToolFactory::Create(selection));
    ASSERT_NE(tool, nullptr);

    tool->_allITERecordInfo.insert(0, IDrawTool::ITERecordInfo());
    EXPECT_TRUE(tool->isFirstEvent());
}

TEST(IDrawToolIsFirstEvent, MultipleEntries_ReturnsFalse)
{
    std::unique_ptr<IDrawTool> tool(CDrawToolFactory::Create(selection));
    ASSERT_NE(tool, nullptr);

    tool->_allITERecordInfo.insert(0, IDrawTool::ITERecordInfo());
    tool->_allITERecordInfo.insert(1, IDrawTool::ITERecordInfo());
    EXPECT_FALSE(tool->isFirstEvent());
}

// =========================================================================
// IDrawTool::defaultAttriVar
// =========================================================================

TEST(IDrawToolDefaultAttriVar, WithDrawBoard_ReturnsValidAttri)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    std::unique_ptr<IDrawTool> tool(CDrawToolFactory::Create(rectangle));
    ASSERT_NE(tool, nullptr);

    tool->setDrawBoard(board);

    DrawAttribution::SAttri attri = tool->defaultAttriVar(EPenWidth);
    EXPECT_EQ(attri.attri, EPenWidth);
    EXPECT_FALSE(attri.var.isNull());
}
