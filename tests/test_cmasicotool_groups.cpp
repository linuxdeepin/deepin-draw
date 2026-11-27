// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for IBlurTool group and blur-flow branches (issue V-9581 §5).
 *
 * Source: src/drawshape/drawTools/cmasicotool.cpp
 *
 * Target lines:
 *   88-94: getBlurEnableItems — group branch: when pItem is a BzGroup,
 *          iterate children and collect blur-enabled items
 *   132-137: saveItemZValue — group recursion: when pItem is a BzGroup,
 *            recurse into children and insert both child and group z-values
 *   244-247: toolStart — no blur-enabled items → _pressedPosBlurEnable=false
 *   332-336: toolFinish — no blur operation (_opeTpUpdate != EDoBLur) →
 *            CUndoRedoCommand::clearCommand()
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QImage>
#include <QPixmap>
#include <QTouchEvent>
#include <memory>

#define protected public
#define private public
#include "drawshape/drawTools/idrawtool.h"
#include "drawshape/drawTools/idrawtoolevent.h"
#include "drawshape/drawTools/cdrawtoolfactory.h"
#include "drawshape/drawTools/cmasicotool.h"
#include "drawshape/globaldefine.h"
#include "drawshape/drawItems/bzItems/cpictureitem.h"
#include "drawshape/drawItems/bzItems/cgraphicsrectitem.h"
#include "drawshape/drawItems/cgraphicsitemselectedmgr.h"
#include "drawshape/drawItems/cgraphicslayer.h"
#include "drawshape/cdrawscene.h"
#include "cundoredocommand.h"
#include "publicApi.h"
#undef protected
#undef private

// Helper: create a touch CDrawToolEvent at the given scene position.
static CDrawToolEvent makeTouchEvent(QEvent::Type type, const QPointF &scenePos,
                                     PageScene *scene, QTouchEvent *outEvent)
{
    QTouchEvent::TouchPoint tp(1, QEventPoint::State::Pressed, scenePos, scenePos);
    return CDrawToolEvent::fromTouchPoint(tp, scene, outEvent);
}

// ===========================================================================
// getBlurEnableItems — group branch (lines 88-94)
// ===========================================================================

TEST(IBlurToolGroupTest, GetBlurEnableItems_WithGroupOfPictures_ReturnsAll)
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

    // Create two picture items (blur-enabled by default).
    QPixmap pix(50, 50);
    pix.fill(Qt::red);
    auto *pic1 = new CPictureItem(pix);
    pic1->setRect(QRectF(0, 0, 50, 50));
    auto *pic2 = new CPictureItem(pix);
    pic2->setRect(QRectF(100, 100, 50, 50));
    scene->addItem(pic1);
    scene->addItem(pic2);

    ASSERT_TRUE(pic1->isBlurEnable());
    ASSERT_TRUE(pic2->isBlurEnable());

    // Group the two picture items.
    auto *group = new CGraphicsItemGroup(CGraphicsItemGroup::ENormalGroup);
    group->add(pic1, false, false);
    group->add(pic2, false, false);

    // Pass the group — should return both blur-enabled children (lines 88-94).
    QList<CGraphicsItem *> result = blurTool->getBlurEnableItems(group);
    EXPECT_EQ(result.count(), 2);

    // Clean up.
    group->clear();
    scene->removeItem(pic1);
    scene->removeItem(pic2);
    delete pic1;
    delete pic2;
    delete group;
}

TEST(IBlurToolGroupTest, GetBlurEnableItems_WithGroupNoBlurItems_ReturnsEmpty)
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

    // Create two rect items (NOT blur-enabled).
    auto *rect1 = new CGraphicsRectItem(QRectF(0, 0, 50, 50));
    auto *rect2 = new CGraphicsRectItem(QRectF(100, 100, 50, 50));
    scene->addItem(rect1);
    scene->addItem(rect2);

    auto *group = new CGraphicsItemGroup(CGraphicsItemGroup::ENormalGroup);
    group->add(rect1, false, false);
    group->add(rect2, false, false);

    QList<CGraphicsItem *> result = blurTool->getBlurEnableItems(group);
    EXPECT_TRUE(result.isEmpty());

    group->clear();
    scene->removeItem(rect1);
    scene->removeItem(rect2);
    delete rect1;
    delete rect2;
    delete group;
}

// ===========================================================================
// saveItemZValue — group recursion (lines 132-137)
// ===========================================================================

TEST(IBlurToolGroupTest, SaveItemZValue_WithGroup_RecursesChildren)
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

    QPixmap pix(50, 50);
    pix.fill(Qt::blue);
    auto *pic1 = new CPictureItem(pix);
    pic1->setRect(QRectF(0, 0, 50, 50));
    auto *pic2 = new CPictureItem(pix);
    pic2->setRect(QRectF(100, 100, 50, 50));
    scene->addItem(pic1);
    scene->addItem(pic2);
    pic1->setZValue(5.0);
    pic2->setZValue(10.0);

    auto *group = new CGraphicsItemGroup(CGraphicsItemGroup::ENormalGroup);
    group->add(pic1, false, false);
    group->add(pic2, false, false);

    blurTool->_tempZs.clear();

    // saveItemZValue on a group recurses into children (lines 132-137).
    blurTool->saveItemZValue(group);

    // _tempZs should contain entries for both children and the group itself.
    EXPECT_GE(blurTool->_tempZs.size(), 2);
    EXPECT_TRUE(blurTool->_tempZs.contains(pic1));
    EXPECT_TRUE(blurTool->_tempZs.contains(pic2));

    // Clean up.
    group->clear();
    scene->removeItem(pic1);
    scene->removeItem(pic2);
    delete pic1;
    delete pic2;
    delete group;
}

// ===========================================================================
// toolStart — no blur-enabled items (lines 244-247)
// ===========================================================================

TEST(IBlurToolGroupTest, ToolStart_NoBlurItems_SetsPressedPosBlurEnableFalse)
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

    // Select a rect item (not blur-enabled).
    auto *rect = new CGraphicsRectItem(QRectF(0, 0, 100, 100));
    scene->addItem(rect);
    scene->selectGroup()->add(rect, false, false);

    // Create a touch event at the rect position.
    QTouchEvent touchEvent(QEvent::TouchBegin);
    CDrawToolEvent event = makeTouchEvent(QEvent::TouchBegin, QPointF(50, 50), scene, &touchEvent);

    // toolStart requires isFirstEvent() (_allITERecordInfo.count()==1).
    IDrawTool::ITERecordInfo info;
    info._isvaild = true;
    info._scene = scene;
    info._curEvent = event;
    info._startEvent = event;
    blurTool->_allITERecordInfo.insert(event.uuid(), info);

    // toolStart with no blur-enabled items → _pressedPosBlurEnable = false (lines 244-247).
    blurTool->_pressedPosBlurEnable = true;
    blurTool->toolStart(&event, &info);

    EXPECT_FALSE(blurTool->_pressedPosBlurEnable);

    // Clean up.
    scene->selectGroup()->clear();
    scene->removeItem(rect);
    delete rect;
}

// ===========================================================================
// toolFinish — no blur operation, clearCommand (lines 332-336)
// ===========================================================================

TEST(IBlurToolGroupTest, ToolFinish_NoBlurOperation_ClearsCommand)
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

    // Set up state where _pressedPosBlurEnable is true but _opeTpUpdate != EDoBLur.
    blurTool->_pressedPosBlurEnable = true;
    blurTool->_bluringItemsSet.clear();
    blurTool->_pLastTopItem = nullptr;
    blurTool->_tempZs.clear();

    // Create an event to pass to toolFinish.
    QTouchEvent touchEvent(QEvent::TouchEnd);
    CDrawToolEvent event = makeTouchEvent(QEvent::TouchEnd, QPointF(50, 50), scene, &touchEvent);

    // toolFinish requires isFinalEvent() (_allITERecordInfo.count()==1).
    IDrawTool::ITERecordInfo info;
    info._opeTpUpdate = 0; // not EDoBLur
    info._isvaild = true;
    info._scene = scene;
    blurTool->_allITERecordInfo.insert(event.uuid(), info);

    // toolFinish should call CUndoRedoCommand::clearCommand() and not crash.
    blurTool->toolFinish(&event, &info);

    // After finish, _pressedPosBlurEnable should be false.
    EXPECT_FALSE(blurTool->_pressedPosBlurEnable);
    EXPECT_EQ(blurTool->_pLastTopItem, nullptr);
}

// ===========================================================================
// JDynamicLayer — blurBegin/blurEnd without blurUpdate (issue V-9581 §4)
// ===========================================================================

TEST(IBlurToolGroupTest, DynamicLayer_BlurBeginEndWithoutUpdate_NotActived)
{
    createNewViewByShortcutKey();
    PageScene *scene = drawApp->topMainWindow()->drawBoard()->currentPage()->scene();
    ASSERT_NE(scene, nullptr);

    QImage img(100, 100, QImage::Format_ARGB32);
    img.fill(Qt::green);
    auto *layer = new JDynamicLayer(img, JDynamicLayer::EImageType);
    layer->setRect(QRectF(0, 0, 100, 100));
    scene->addItem(layer);

    // Make the layer blur-enabled (isBlocked() must be true for isBlurEnable()).
    layer->setBlocked(true);
    ASSERT_TRUE(layer->isBlurEnable());

    // Call blurBegin then blurEnd WITHOUT blurUpdate — should not crash and
    // isBlurActived() should be false (no blur path was drawn).
    layer->blurBegin(QPointF(10, 10));
    layer->blurEnd();

    EXPECT_FALSE(layer->isBlurActived());

    scene->removeItem(layer);
    delete layer;
}
