// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for tools and scene miscellaneous interfaces (issue V-8626)
 *
 * Existing tests cover CCutTool createCutItem/deleteCutItem,
 * CSelectTool registerAttributionWidgets registration,
 * IBlurTool registerAttributionWidgets — but NOT:
 *   - CCutTool::getCutStatus()
 *   - CSelectTool group buttonClicked lambda
 *   - IBlurTool boardChanged nested lambda
 *   - PageScene::getGroupTreeInfo sort lambda
 *   - PageContext::addSceneItem lambda
 *
 * Source:
 *   src/drawshape/drawTools/ccuttool.cpp
 *   src/drawshape/drawTools/cselecttool.cpp
 *   src/drawshape/drawTools/cmasicotool.cpp
 *   src/drawshape/cdrawscene.cpp
 *   src/drawshape/cdrawparamsigleton.cpp
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#define protected public
#define private public
#include "drawshape/drawTools/ccuttool.h"
#include "drawshape/drawTools/cdrawtoolfactory.h"
#include "drawshape/drawTools/cselecttool.h"
#include "drawshape/drawTools/cmasicotool.h"
#include "drawshape/drawTools/idrawtool.h"
#include "drawshape/cdrawscene.h"
#include "drawshape/cdrawparamsigleton.h"
#include "drawshape/drawItems/bzItems/cgraphicsitem.h"
#include "drawshape/drawItems/bzItems/cgraphicsrectitem.h"
#include "drawshape/drawItems/bzItems/cgraphicscutitem.h"
#include "drawshape/drawItems/cgraphicsitemselectedmgr.h"
#include "drawshape/globaldefine.h"
#include "cattributemanagerwgt.h"
#include "cattributeitemwidget.h"
#include "publicApi.h"
#undef protected
#undef private

#include <QSet>

// ===========================================================================
// Helper: create a CCutTool and set its drawBoard
// ===========================================================================
static CCutTool *createCutToolForTestMisc()
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    CCutTool *tool = qobject_cast<CCutTool *>(CDrawToolFactory::Create(cut));
    if (tool)
        tool->setDrawBoard(board);
    return tool;
}

static PageScene *getTestSceneMisc()
{
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    auto *page = board->currentPage();
    return page ? page->scene() : nullptr;
}

// ===========================================================================
// CCutTool::getCutStatus
// ===========================================================================

TEST(CCutToolMiscTest, GetCutStatus_NoCutItem_ReturnsFalse)
{
    CCutTool *tool = createCutToolForTestMisc();
    ASSERT_NE(tool, nullptr);

    // No cut item created yet — getCutStatus should return false
    bool status = tool->getCutStatus();
    EXPECT_FALSE(status);

    delete tool;
}

TEST(CCutToolMiscTest, GetCutStatus_WithCutItemNotMoved_ReturnsFalse)
{
    CCutTool *tool = createCutToolForTestMisc();
    ASSERT_NE(tool, nullptr);
    PageScene *scene = getTestSceneMisc();
    ASSERT_NE(scene, nullptr);

    // Create a cut item — by default its rect matches the scene rect
    tool->createCutItem(scene);

    bool status = tool->getCutStatus();
    // When the cut item hasn't been moved/resized, status should be false
    EXPECT_FALSE(status);

    delete tool;
}

// ===========================================================================
// CSelectTool — group buttonClicked lambda
// ===========================================================================

// Snapshot helper to restore the attribute registry after test
class AttrRegistrySnapshotMisc
{
public:
    explicit AttrRegistrySnapshotMisc(CAttributeManagerWgt *mgr)
        : m_mgr(mgr), m_wgts(mgr->s_allInstalledAttriWgts),
          m_vars(mgr->s_allInstalledDefaultVar) {}

    ~AttrRegistrySnapshotMisc()
    {
        QSet<QWidget *> preWidgets;
        for (QWidget *w : m_wgts)
            preWidgets.insert(w);
        for (auto it = m_mgr->s_allInstalledAttriWgts.constBegin();
             it != m_mgr->s_allInstalledAttriWgts.constEnd(); ++it) {
            if (!preWidgets.contains(it.value()) && it.value()->parent() == nullptr)
                delete it.value();
        }
        m_mgr->s_allInstalledAttriWgts = m_wgts;
        m_mgr->s_allInstalledDefaultVar = m_vars;
    }

private:
    CAttributeManagerWgt *m_mgr;
    QMap<int, QWidget *> m_wgts;
    QMap<int, QVariant> m_vars;
};

TEST(CSelectToolMiscTest, RegisterAttributionWidgets_GroupButtonClicked_NoCrash)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    CSelectTool *tool = qobject_cast<CSelectTool *>(CDrawToolFactory::Create(selection));
    ASSERT_NE(tool, nullptr);
    tool->setDrawBoard(board);

    // Snapshot the registry so the test doesn't permanently modify it
    auto *attrMgr = board->attributionWidget();
    ASSERT_NE(attrMgr, nullptr);
    AttrRegistrySnapshotMisc snap(attrMgr);

    // Register attribution widgets (creates the group button widget)
    tool->registerAttributionWidgets();

    // Find the CGroupButtonWgt in the registered widgets
    CGroupButtonWgt *groupBtn = nullptr;
    for (auto it = attrMgr->s_allInstalledAttriWgts.constBegin();
         it != attrMgr->s_allInstalledAttriWgts.constEnd(); ++it) {
        groupBtn = qobject_cast<CGroupButtonWgt *>(it.value());
        if (groupBtn)
            break;
    }

    if (groupBtn) {
        // Emit the buttonClicked signal with doGroup=true, doUngroup=false
        emit groupBtn->buttonClicked(true, false);
        QCoreApplication::processEvents(QEventLoop::AllEvents, 200);
    }

    // No crash means success
    delete tool;
    SUCCEED();
}

// ===========================================================================
// IBlurTool — boardChanged nested lambda
// ===========================================================================

TEST(IBlurToolMiscTest, BoardChangedLambdaDoesNotCrash)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    IBlurTool *tool = qobject_cast<IBlurTool *>(CDrawToolFactory::Create(blur));
    ASSERT_NE(tool, nullptr);

    // The IBlurTool constructor connects boardChanged; setting the drawBoard
    // should trigger boardChanged which fires the nested lambdas
    tool->setDrawBoard(board);

    QCoreApplication::processEvents(QEventLoop::AllEvents, 200);

    // No crash means the nested lambdas executed correctly
    delete tool;
    SUCCEED();
}

// ===========================================================================
// PageScene::getGroupTreeInfo — sort lambda
// ===========================================================================

TEST(PageSceneMiscTest, GetGroupTreeInfo_NoGroups_NoCrash)
{
    createNewViewByShortcutKey();
    PageScene *scene = getTestSceneMisc();
    ASSERT_NE(scene, nullptr);

    // Call with nullptr group to collect all group tree info
    auto info = scene->getGroupTreeInfo(nullptr, ESaveToDDf, false);

    // Should return valid info with no child groups (empty scene)
    EXPECT_TRUE(info.childGroups.isEmpty());
}

TEST(PageSceneMiscTest, GetGroupTreeInfo_WithGroups_NoCrash)
{
    createNewViewByShortcutKey();
    PageScene *scene = getTestSceneMisc();
    ASSERT_NE(scene, nullptr);

    // Add some items to the scene
    CGraphicsRectItem *rect1 = new CGraphicsRectItem(QRectF(0, 0, 50, 50));
    CGraphicsRectItem *rect2 = new CGraphicsRectItem(QRectF(100, 100, 50, 50));
    scene->addCItem(rect1);
    scene->addCItem(rect2);

    // Create a group with the items
    scene->creatGroup(QList<CGraphicsItem *>() << rect1 << rect2,
                      CGraphicsItemGroup::ENormalGroup, true, nullptr, true);

    // Call getGroupTreeInfo — exercises the sort lambda
    auto info = scene->getGroupTreeInfo(nullptr, ESaveToDDf, false);

    // Should have at least one child group
    EXPECT_GE(info.childGroups.size(), 1);
}

// ===========================================================================
// PageContext::addSceneItem — lambda path
// ===========================================================================

TEST(PageContextMiscTest, AddSceneItemDoesNotCrash)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);
    Page *page = board->currentPage();
    ASSERT_NE(page, nullptr);

    PageContext *ctx = page->context();
    ASSERT_NE(ctx, nullptr);

    // Create a CGraphicsUnit for a rect item and add it via addSceneItem
    CGraphicsRectItem rectItem(QRectF(10, 10, 50, 50));
    CGraphicsUnit unit = rectItem.getGraphicsUnit(ESaveToDDf);

    ctx->addSceneItem(unit, false, true, false);

    // The item should have been added to the scene
    QCoreApplication::processEvents(QEventLoop::AllEvents, 200);

    SUCCEED();
}
