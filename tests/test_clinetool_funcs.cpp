// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CLineTool (issue V-7767)
 *
 * Source: src/drawshape/drawTools/clinetool.cpp
 *
 * Target methods:
 *   - Constructor (tool mode = line)
 *   - attributions() (returns 5 default attribute items)
 *   - toolCreatItemUpdate() (line follows cursor)
 *   - toolCreatItemFinish() (not moved removes item / moved selects item)
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <algorithm>

#include <QPointF>
#include <QLineF>

#define protected public
#define private public
#include "drawshape/drawTools/clinetool.h"
#include "drawshape/drawTools/cdrawtoolfactory.h"
#include "drawshape/drawTools/idrawtool.h"
#include "drawshape/drawTools/idrawtoolevent.h"
#include "drawshape/drawItems/bzItems/cgraphicslineitem.h"
#include "drawshape/drawItems/bzItems/cgraphicsitem.h"
#include "drawshape/globaldefine.h"
#include "drawshape/cdrawscene.h"
#include "publicApi.h"
#undef protected
#undef private

// =========================================================================
// Helper: create a CLineTool and set its drawBoard
// =========================================================================
static CLineTool *createLineToolForTest()
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    CLineTool *tool = qobject_cast<CLineTool *>(CDrawToolFactory::Create(line));
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
// Constructor — tool mode is `line`
// =========================================================================

TEST(CLineToolConstructor, ToolModeIsLine)
{
    CLineTool *tool = new CLineTool();
    ASSERT_NE(tool, nullptr);
    EXPECT_EQ(tool->getDrawToolMode(), line);
    delete tool;
}

// =========================================================================
// attributions — returns 5 default attribute items
// =========================================================================

TEST(CLineToolAttributions, ReturnsFiveAttributeItems)
{
    CLineTool *tool = createLineToolForTest();
    ASSERT_NE(tool, nullptr);

    DrawAttribution::SAttrisList result = tool->attributions();
    EXPECT_EQ(result.size(), 5);

    delete tool;
}

TEST(CLineToolAttributions, ContainsExpectedAttributeTypes)
{
    CLineTool *tool = createLineToolForTest();
    ASSERT_NE(tool, nullptr);

    DrawAttribution::SAttrisList result = tool->attributions();
    ASSERT_EQ(result.size(), 5);

    // Verify the first two are EPenColor and EBorderWidth
    EXPECT_EQ(result[0].attri, DrawAttribution::EPenColor);
    EXPECT_EQ(result[1].attri, DrawAttribution::EBorderWidth);

    // Verify the last two are EStreakBeginStyle and EStreakEndStyle
    EXPECT_EQ(result[3].attri, DrawAttribution::EStreakBeginStyle);
    EXPECT_EQ(result[4].attri, DrawAttribution::EStreakEndStyle);

    delete tool;
}

// =========================================================================
// toolCreatItemUpdate — line follows cursor
// =========================================================================

TEST(CLineToolToolCreatItemUpdate, LineFollowsCursor)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    CLineTool *tool = createLineToolForTest();
    ASSERT_NE(tool, nullptr);

    // Create a line item and add it to the scene
    CGraphicsLineItem *lineItem = new CGraphicsLineItem(10.0, 10.0, 10.0, 10.0);
    scene->addCItem(lineItem);

    // Set up the event and record info
    CDrawToolEvent event(QPointF(100, 200), QPointF(100, 200), QPointF(100, 200), scene);
    IDrawTool::ITERecordInfo info;
    info._startPos = QPointF(10, 10);
    info.businessItem = lineItem;
    info._scene = scene;
    info._moved = true;

    tool->toolCreatItemUpdate(&event, &info);

    // The line should be updated to follow the cursor
    QLineF resultLine = lineItem->line();
    EXPECT_EQ(resultLine.p1(), QPointF(10, 10));
    // p2 should have changed to the event position
    EXPECT_NE(resultLine.p2(), QPointF(10, 10));

    delete tool;
    delete lineItem;
}

// =========================================================================
// toolCreatItemFinish — not moved removes item from scene
// =========================================================================

TEST(CLineToolToolCreatItemFinish, NotMovedRemovesItem)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    CLineTool *tool = createLineToolForTest();
    ASSERT_NE(tool, nullptr);

    // Create a line item and add it to the scene
    CGraphicsLineItem *lineItem = new CGraphicsLineItem(50.0, 50.0, 50.0, 50.0);
    scene->addCItem(lineItem);

    // The scene holds exactly this one business item before the finish call
    ASSERT_EQ(scene->getBzItems().count(), 1);
    ASSERT_EQ(scene->getBzItems().first(), lineItem);

    // Set up the event and record info with _moved = false (not moved)
    CDrawToolEvent event(QPointF(50, 50), QPointF(50, 50), QPointF(50, 50), scene);
    IDrawTool::ITERecordInfo info;
    info._startPos = QPointF(50, 50);
    info.businessItem = lineItem;
    info._scene = scene;
    info._moved = false;

    // Track the item by address only: the removal path deletes the item, so
    // the raw pointer must not be dereferenced after the finish call.
    const void *trackedItem = lineItem;

    tool->toolCreatItemFinish(&event, &info);

    // Item should have been removed from scene (businessItem set to null)
    EXPECT_EQ(info.businessItem, nullptr);

    // The item must be absent from the scene: no business item may remain at
    // the tracked address. Address comparison stays safe even when the
    // removal deleted the item (pointer value is never dereferenced).
    const QList<CGraphicsItem *> remaining = scene->getBzItems();
    EXPECT_TRUE(std::none_of(remaining.cbegin(), remaining.cend(),
                             [trackedItem](const QGraphicsItem *it) {
                                 return static_cast<const void *>(it) == trackedItem;
                             })) << "businessItem was nulled but the item is still in the scene";

    delete tool;
}

// =========================================================================
// toolCreatItemFinish — moved retains and selects the item
//
// Note: CGraphicsItem::isSelected() is overridden to check membership in the
// scene's select-group (ESelectGroup), not Qt's native selection flag.
// CLineTool::toolCreatItemFinish calls QGraphicsItem::setSelected(true) (the
// Qt-native method), which sets Qt's internal selection flag but does not add
// the item to the select-group. We therefore verify the selection through
// QGraphicsScene::selectedItems() (Qt-native, populated by setSelected) and
// confirm the item is retained (businessItem not nulled), contrasting with the
// not-moved branch which removes the item.
// =========================================================================

TEST(CLineToolToolCreatItemFinish, MovedSelectsItem)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    CLineTool *tool = createLineToolForTest();
    ASSERT_NE(tool, nullptr);

    // Create a line item and add it to the scene
    CGraphicsLineItem *lineItem = new CGraphicsLineItem(0.0, 0.0, 100.0, 100.0);
    scene->addCItem(lineItem);
    ASSERT_NE(lineItem->scene(), nullptr);

    // Set up the event and record info with _moved = true
    CDrawToolEvent event(QPointF(100, 100), QPointF(100, 100), QPointF(100, 100), scene);
    IDrawTool::ITERecordInfo info;
    info._startPos = QPointF(0, 0);
    info.businessItem = lineItem;
    info._scene = scene;
    info._moved = true;

    tool->toolCreatItemFinish(&event, &info);

    // Moved branch: item is retained (not removed) and businessItem is kept.
    EXPECT_EQ(info.businessItem, lineItem);
    EXPECT_NE(lineItem->scene(), nullptr);

    // setSelected(true) was invoked -> item appears in the scene's Qt-native
    // selected-items list.
    EXPECT_TRUE(scene->selectedItems().contains(lineItem));

    delete tool;
    delete lineItem;
}
