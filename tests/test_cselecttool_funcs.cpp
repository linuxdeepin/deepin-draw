// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CSelectTool methods (issue V-5429)
 *
 * Source: src/drawshape/drawTools/cselecttool.cpp
 *
 * Target methods:
 *   CSelectTool: cursor, drawMore, processItemsRot,
 *                processItemsScal, registerAttributionWidgets
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <memory>
#include <QPainter>
#include <QImage>
#include <QCursor>

#define protected public
#define private public
#include "drawshape/drawTools/idrawtool.h"
#include "drawshape/drawTools/cdrawtoolfactory.h"
#include "drawshape/drawTools/cselecttool.h"
#include "drawshape/drawTools/idrawtoolevent.h"
#include "drawshape/globaldefine.h"
#include "cattributemanagerwgt.h"
#include "publicApi.h"
#undef protected
#undef private

// =========================================================================
// CSelectTool::cursor
// =========================================================================

TEST(CSelectToolCursor, ReturnsArrowCursor)
{
    std::unique_ptr<IDrawTool> tool(CDrawToolFactory::Create(selection));
    ASSERT_NE(tool, nullptr);

    auto *selectTool = static_cast<CSelectTool *>(tool.get());
    QCursor c = selectTool->cursor();
    EXPECT_EQ(c.shape(), Qt::ArrowCursor);
}

// =========================================================================
// CSelectTool::drawMore
// =========================================================================

TEST(CSelectToolDrawMore, EmptyITERecordInfo_NoCrash)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    std::unique_ptr<IDrawTool> tool(CDrawToolFactory::Create(selection));
    ASSERT_NE(tool, nullptr);
    tool->setDrawBoard(board);
    auto *selectTool = static_cast<CSelectTool *>(tool.get());

    auto *page = board->currentPage();
    ASSERT_NE(page, nullptr);
    auto *scene = page->scene();
    ASSERT_NE(scene, nullptr);

    QImage img(200, 200, QImage::Format_ARGB32);
    img.fill(Qt::white);
    QPainter painter(&img);

    QRectF rect(0, 0, 200, 200);
    selectTool->_allITERecordInfo.clear();
    selectTool->drawMore(&painter, rect, scene);
    SUCCEED();
}

TEST(CSelectToolDrawMore, WithERectSelectEntry_DrawsRect)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    std::unique_ptr<IDrawTool> tool(CDrawToolFactory::Create(selection));
    ASSERT_NE(tool, nullptr);
    tool->setDrawBoard(board);
    auto *selectTool = static_cast<CSelectTool *>(tool.get());

    auto *page = board->currentPage();
    ASSERT_NE(page, nullptr);
    auto *scene = page->scene();
    ASSERT_NE(scene, nullptr);

    IDrawTool::ITERecordInfo info;
    info._opeTpUpdate = CSelectTool::ERectSelect;
    info._startPos = QPointF(10, 10);
    info._curEvent = CDrawToolEvent(QPointF(100, 100));
    selectTool->_allITERecordInfo.clear();
    selectTool->_allITERecordInfo.insert(0, info);

    QImage img(200, 200, QImage::Format_ARGB32);
    img.fill(Qt::white);
    QPainter painter(&img);
    QRectF rect(0, 0, 200, 200);

    selectTool->drawMore(&painter, rect, scene);
    SUCCEED();

    selectTool->_allITERecordInfo.clear();
}

// =========================================================================
// CSelectTool::registerAttributionWidgets
// =========================================================================

TEST(CSelectToolRegisterAttrWidgets, RegistersTitleAndGroupWidgets)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    std::unique_ptr<IDrawTool> tool(CDrawToolFactory::Create(selection));
    ASSERT_NE(tool, nullptr);
    tool->setDrawBoard(board);
    auto *selectTool = static_cast<CSelectTool *>(tool.get());

    selectTool->registerAttributionWidgets();

    auto *attrWgt = board->attributionWidget();
    ASSERT_NE(attrWgt, nullptr);
    using namespace DrawAttribution;
    EXPECT_TRUE(attrWgt->s_allInstalledAttriWgts.contains(ETitle));
    EXPECT_TRUE(attrWgt->s_allInstalledAttriWgts.contains(EGroupWgt));
}

// =========================================================================
// CSelectTool::processItemsRot
// =========================================================================

TEST(CSelectToolProcessItemsRot, EmptyEtcItems_NoCrash)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    std::unique_ptr<IDrawTool> tool(CDrawToolFactory::Create(selection));
    ASSERT_NE(tool, nullptr);
    tool->setDrawBoard(board);
    auto *selectTool = static_cast<CSelectTool *>(tool.get());

    auto *page = board->currentPage();
    ASSERT_NE(page, nullptr);
    auto *scene = page->scene();
    ASSERT_NE(scene, nullptr);

    CDrawToolEvent event(QPointF(50, 50), QPointF(50, 50),
                         QPointF(50, 50), scene);

    IDrawTool::ITERecordInfo info;
    info._opeTpUpdate = CSelectTool::ERotateMove;
    info._startPos = QPointF(10, 10);
    info._prePos = QPointF(20, 20);
    info.etcItems.clear();

    selectTool->processItemsRot(&event, &info, EChangedUpdate);
    SUCCEED();
}

TEST(CSelectToolProcessItemsRot, WithRectItem_BeginPhase_NoCrash)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    std::unique_ptr<IDrawTool> tool(CDrawToolFactory::Create(selection));
    ASSERT_NE(tool, nullptr);
    tool->setDrawBoard(board);
    auto *selectTool = static_cast<CSelectTool *>(tool.get());

    auto *page = board->currentPage();
    ASSERT_NE(page, nullptr);
    auto *scene = page->scene();
    ASSERT_NE(scene, nullptr);

    CGraphicsRectItem *rectItem = new CGraphicsRectItem(QRectF(0, 0, 100, 100));
    scene->addItem(rectItem);

    CDrawToolEvent event(QPointF(50, 50), QPointF(50, 50),
                         QPointF(50, 50), scene);

    IDrawTool::ITERecordInfo info;
    info._opeTpUpdate = CSelectTool::ERotateMove;
    info._startPos = QPointF(10, 10);
    info._prePos = QPointF(20, 20);
    info.etcItems.clear();
    info.etcItems.append(rectItem);

    selectTool->processItemsRot(&event, &info, EChangedBegin);
    SUCCEED();

    scene->removeItem(rectItem);
    delete rectItem;
}

// =========================================================================
// CSelectTool::processItemsScal
// =========================================================================

TEST(CSelectToolProcessItemsScal, EmptyEtcItems_NoCrash)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    std::unique_ptr<IDrawTool> tool(CDrawToolFactory::Create(selection));
    ASSERT_NE(tool, nullptr);
    tool->setDrawBoard(board);
    auto *selectTool = static_cast<CSelectTool *>(tool.get());

    auto *page = board->currentPage();
    ASSERT_NE(page, nullptr);
    auto *scene = page->scene();
    ASSERT_NE(scene, nullptr);

    CDrawToolEvent event(QPointF(50, 50), QPointF(50, 50),
                         QPointF(50, 50), scene);

    IDrawTool::ITERecordInfo info;
    info._opeTpUpdate = CSelectTool::EResizeMove;
    info._etcopeTpUpdate = 0;
    info._startPos = QPointF(10, 10);
    info._prePos = QPointF(20, 20);
    info.etcItems.clear();

    selectTool->processItemsScal(&event, &info, EChangedUpdate);
    SUCCEED();
}

TEST(CSelectToolProcessItemsScal, WithRectItem_BeginPhase_NoCrash)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    std::unique_ptr<IDrawTool> tool(CDrawToolFactory::Create(selection));
    ASSERT_NE(tool, nullptr);
    tool->setDrawBoard(board);
    auto *selectTool = static_cast<CSelectTool *>(tool.get());

    auto *page = board->currentPage();
    ASSERT_NE(page, nullptr);
    auto *scene = page->scene();
    ASSERT_NE(scene, nullptr);

    CGraphicsRectItem *rectItem = new CGraphicsRectItem(QRectF(0, 0, 100, 100));
    scene->addItem(rectItem);

    CDrawToolEvent event(QPointF(80, 80), QPointF(80, 80),
                         QPointF(80, 80), scene);

    IDrawTool::ITERecordInfo info;
    info._opeTpUpdate = CSelectTool::EResizeMove;
    info._etcopeTpUpdate = 0;
    info._startPos = QPointF(10, 10);
    info._prePos = QPointF(20, 20);
    info.etcItems.clear();
    info.etcItems.append(rectItem);

    selectTool->processItemsScal(&event, &info, EChangedBegin);
    SUCCEED();

    scene->removeItem(rectItem);
    delete rectItem;
}
