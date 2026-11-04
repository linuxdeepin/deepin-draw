// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CLineTool
 *
 * Source: src/drawshape/drawTools/clinetool.cpp
 *
 * Target methods:
 *   CLineTool()                  — 工具模式为 line
 *   attributions()               — 5 个默认属性项
 *   toolCreatItemUpdate()        — 更新直线终点
 *   toolCreatItemFinish()        — 未移动移除图元 / 移动后选中图元
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#define protected public
#define private public
#include "drawshape/drawTools/clinetool.h"
#include "drawshape/drawTools/cdrawtoolfactory.h"
#include "drawshape/drawTools/idrawtool.h"
#include "drawshape/drawTools/idrawtoolevent.h"
#include "drawshape/globaldefine.h"
#include "drawshape/cdrawscene.h"
#include "cgraphicslineitem.h"
#include "application.h"
#include "publicApi.h"
#undef protected
#undef private

// =========================================================================
// Helpers
// =========================================================================

static CLineTool *createLineToolForTest()
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    auto *tool = qobject_cast<CLineTool *>(CDrawToolFactory::Create(line));
    if (tool != nullptr)
        tool->setDrawBoard(board);
    return tool;
}

static PageScene *getTestScene()
{
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    auto *page = board->currentPage();
    return page ? page->scene() : nullptr;
}

// 构造一个模拟左键鼠标事件（eventType() == EMouseEvent）
static CDrawToolEvent makeMouseSceneEvent(const QPointF &pos, PageScene *scene,
                                          Qt::KeyboardModifiers mods = Qt::NoModifier)
{
    auto *orgEvent = new QMouseEvent(QEvent::MouseButtonPress, pos,
                                     Qt::LeftButton, Qt::LeftButton, mods);
    CDrawToolEvent ev(pos, pos, pos, scene);
    ev._orgEvent = orgEvent;
    ev._msBtns = Qt::LeftButton;
    ev._kbMods = mods;
    return ev;
}

static void releaseEvent(CDrawToolEvent &ev)
{
    delete ev._orgEvent;
    ev._orgEvent = nullptr;
}

static IDrawTool::ITERecordInfo makeRecordInfo(PageScene *scene,
                                               CGraphicsLineItem *item,
                                               const QPointF &startPos)
{
    IDrawTool::ITERecordInfo info;
    info.eventLife = IDrawTool::ENormal;
    info._startPos = startPos;
    info._prePos = startPos;
    info._scene = scene;
    info._isvaild = true;
    info.businessItem = item;
    return info;
}

// =========================================================================
// 构造函数与基础接口
// =========================================================================

TEST(CLineToolCtorTest, ModeIsLine)
{
    CLineTool tool;
    EXPECT_EQ(tool.getDrawToolMode(), line);
}

TEST(CLineToolAttributionsTest, ContainsFiveExpectedKeys)
{
    CLineTool *tool = createLineToolForTest();
    ASSERT_NE(tool, nullptr);

    DrawAttribution::SAttrisList attrs = tool->attributions();
    ASSERT_EQ(attrs.size(), 5);

    QSet<int> keys;
    for (const auto &attr : attrs)
        keys.insert(attr.attri);

    EXPECT_TRUE(keys.contains(DrawAttribution::EPenColor));
    EXPECT_TRUE(keys.contains(DrawAttribution::EBorderWidth));
    EXPECT_TRUE(keys.contains(1775));
    EXPECT_TRUE(keys.contains(DrawAttribution::EStreakBeginStyle));
    EXPECT_TRUE(keys.contains(DrawAttribution::EStreakEndStyle));

    delete tool;
}

// =========================================================================
// toolCreatItemUpdate
// =========================================================================

TEST(CLineToolUpdateTest, NoModifier_LineFollowsCursor)
{
    auto *tool = createLineToolForTest();
    ASSERT_NE(tool, nullptr);
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    CGraphicsLineItem *item = new CGraphicsLineItem(50, 50, 50, 50);
    IDrawTool::ITERecordInfo info = makeRecordInfo(scene, item, QPointF(50, 50));

    CDrawToolEvent ev = makeMouseSceneEvent(QPointF(120, 80), scene);
    tool->toolCreatItemUpdate(&ev, &info);
    releaseEvent(ev);

    EXPECT_EQ(item->line(), QLineF(QPointF(50, 50), QPointF(120, 80)));

    delete item;
    delete tool;
}

// =========================================================================
// toolCreatItemFinish
// =========================================================================

TEST(CLineToolFinishTest, ClickWithoutMove_RemovesItem)
{
    auto *tool = createLineToolForTest();
    ASSERT_NE(tool, nullptr);
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    int countBefore = scene->items().count();

    CGraphicsLineItem *item = new CGraphicsLineItem(50, 50, 50, 50);
    scene->addCItem(item);
    ASSERT_TRUE(scene->items().contains(item));

    IDrawTool::ITERecordInfo info = makeRecordInfo(scene, item, QPointF(50, 50));
    info._moved = false;

    CDrawToolEvent ev = makeMouseSceneEvent(QPointF(51, 51), scene);
    tool->toolCreatItemFinish(&ev, &info);
    releaseEvent(ev);

    // 未移动：图元被移除，businessItem 置空，事件被接受
    EXPECT_EQ(info.businessItem, nullptr);
    EXPECT_TRUE(ev.isAccepted());
    EXPECT_EQ(scene->items().count(), countBefore);

    delete tool;
}

TEST(CLineToolFinishTest, AfterMove_KeepsItemAndSelectsIt)
{
    auto *tool = createLineToolForTest();
    ASSERT_NE(tool, nullptr);
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    int countBefore = scene->items().count();

    CGraphicsLineItem *item = new CGraphicsLineItem(50, 50, 50, 50);
    scene->addCItem(item);

    IDrawTool::ITERecordInfo info = makeRecordInfo(scene, item, QPointF(50, 50));
    info._prePos = QPointF(150, 150);
    info._moved = true;

    CDrawToolEvent ev = makeMouseSceneEvent(QPointF(150, 150), scene);
    tool->toolCreatItemFinish(&ev, &info);
    releaseEvent(ev);

    // 已移动：图元保留在场景中并置选中态
    // （直线图元构造时会附带 leftTop / rightBottom 两个控制点子项）
    EXPECT_TRUE(scene->items().contains(item));
    EXPECT_EQ(scene->items().count(), countBefore + 3);
    EXPECT_TRUE(item->QGraphicsItem::isSelected());

    scene->removeCItem(item, true);
    delete tool;
}
