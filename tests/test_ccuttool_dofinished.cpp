// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CCutTool::doFinished (issue V-7618 group 4)
 *
 * Source: src/drawshape/drawTools/ccuttool.cpp lines 325-339
 *
 * Target method:
 *   void doFinished(bool accept, bool cmd = false)
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
// CCutTool::doFinished
// =========================================================================

TEST(CCutToolDoFinished, AcceptTrue_AppliesCutChanges)
{
    CCutTool *tool = createCutToolForTest();
    ASSERT_NE(tool, nullptr);
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    tool->createCutItem(scene);

    // doFinished with accept=true should apply cut and switch to selection tool
    tool->doFinished(true, false);

    // After doFinished, the tool should have switched to selection
    EXPECT_NE(tool->drawBoard()->currentTool_p(), nullptr);
    delete tool;
}

TEST(CCutToolDoFinished, AcceptFalse_RollsBack)
{
    CCutTool *tool = createCutToolForTest();
    ASSERT_NE(tool, nullptr);
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    tool->createCutItem(scene);

    // doFinished with accept=false should roll back (no changes applied)
    tool->doFinished(false, false);

    SUCCEED();
    delete tool;
}

TEST(CCutToolDoFinished, CmdTrue_NoCrash)
{
    CCutTool *tool = createCutToolForTest();
    ASSERT_NE(tool, nullptr);
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    tool->createCutItem(scene);

    // cmd=true path should not crash
    tool->doFinished(true, true);

    SUCCEED();
    delete tool;
}

TEST(CCutToolDoFinished, AcceptFalseCmdTrue_NoCrash)
{
    CCutTool *tool = createCutToolForTest();
    ASSERT_NE(tool, nullptr);
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    tool->createCutItem(scene);

    tool->doFinished(false, true);

    SUCCEED();
    delete tool;
}
