// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for layer/cut/align QUndoCommand subclasses in cundocommands.
 *
 * Testable (active implementations):
 *   - CSceneCutCommand   (ctor / dtor / undo / redo)
 *   - CItemsAlignCommand (ctor / undo / redo)
 *
 * Source defects (implementations commented out in cundocommands.cpp):
 *   - COneLayerUpCommand / COneLayerDownCommand
 *   - CBringToFrontCommand / CSendToBackCommand
 *   Their constructors and methods have NO active definition anywhere in the
 *   codebase (confirmed via codebase-memory-mcp: 0 Method nodes). Referencing
 *   them causes linker "undefined reference" errors, so they cannot be
 *   exercised. Each is covered by a GTEST_SKIP() placeholder and reported as a
 *   source defect; production code is not modified.
 *
 * Source: src/frame/cundocommands.cpp / src/frame/cundocommands.h
 *
 * Case counts:
 *   CSceneCutCommand   -> 7 cases
 *   CItemsAlignCommand -> 8 cases
 *   4 defect classes   -> 1 GTEST_SKIP each
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#define protected public
#define private public
#include "frame/cundocommands.h"
#include "drawshape/globaldefine.h"
#include "drawshape/sitemdata.h"
#include "cgraphicsrectitem.h"
#include "publicApi.h"
#undef protected
#undef private

#include <QPointF>
#include <QRectF>
#include <QMap>

// ---------------------------------------------------------------------------
// helpers
// ---------------------------------------------------------------------------

// Create a fresh offscreen view/scene and clear any prior selection.
static PageScene *freshScene()
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    if (view == nullptr)
        return nullptr;
    PageScene *scene = view->drawScene();
    if (scene == nullptr)
        return nullptr;
    scene->clearSelectGroup();
    return scene;
}

// Add a rectangle item to the scene and place it at `pos` (item coordinates
// origin), returning it for alignment/position assertions.
static CGraphicsRectItem *addRectItem(PageScene *scene, const QPointF &pos)
{
    auto *item = new CGraphicsRectItem(QRectF(0, 0, 100, 80));
    scene->addCItem(item, true, false);
    item->setPos(pos);
    return item;
}

// =========================================================================
// CSceneCutCommand
// =========================================================================

TEST(CSceneCutCommand, Constructor_RecordsOldRectAndStoresFields)
{
    PageScene *scene = freshScene();
    ASSERT_NE(scene, nullptr);
    QRectF oldRect = scene->sceneRect();
    QRectF newRect(10, 20, 300, 200);

    CSceneCutCommand cmd(scene, newRect);
    EXPECT_EQ(cmd.m_oldRect, oldRect);
    EXPECT_EQ(cmd.m_newRect, newRect);
    EXPECT_EQ(cmd.myGraphicsScene, scene);
    EXPECT_EQ(cmd.m_item, nullptr);
}

TEST(CSceneCutCommand, Redo_SetsSceneRectToNewRect)
{
    PageScene *scene = freshScene();
    ASSERT_NE(scene, nullptr);
    QRectF oldRect = scene->sceneRect();
    QRectF newRect(10, 20, 300, 200);

    CSceneCutCommand cmd(scene, newRect);
    cmd.redo();
    EXPECT_EQ(scene->sceneRect(), newRect);
    EXPECT_EQ(cmd.m_oldRect, oldRect);
}

TEST(CSceneCutCommand, Undo_AfterRedoRestoresOldRect)
{
    PageScene *scene = freshScene();
    ASSERT_NE(scene, nullptr);
    QRectF oldRect = scene->sceneRect();
    QRectF newRect(10, 20, 300, 200);

    CSceneCutCommand cmd(scene, newRect);
    cmd.redo();
    ASSERT_EQ(scene->sceneRect(), newRect);
    cmd.undo();
    EXPECT_EQ(scene->sceneRect(), oldRect);
}

TEST(CSceneCutCommand, RoundTrip_MultipleCycles_StaysConsistent)
{
    PageScene *scene = freshScene();
    ASSERT_NE(scene, nullptr);
    QRectF oldRect = scene->sceneRect();
    QRectF newRect(5, 5, 500, 400);

    CSceneCutCommand cmd(scene, newRect);
    for (int i = 0; i < 5; ++i) {
        cmd.redo();
        EXPECT_EQ(scene->sceneRect(), newRect);
        cmd.undo();
        EXPECT_EQ(scene->sceneRect(), oldRect);
    }
}

TEST(CSceneCutCommand, UndoWithoutRedo_KeepsOldRect)
{
    PageScene *scene = freshScene();
    ASSERT_NE(scene, nullptr);
    QRectF oldRect = scene->sceneRect();
    QRectF newRect(10, 20, 300, 200);

    CSceneCutCommand cmd(scene, newRect);
    // undo with no prior redo: sceneRect is set back to m_oldRect (== current)
    cmd.undo();
    EXPECT_EQ(scene->sceneRect(), oldRect);
    EXPECT_NE(scene->sceneRect(), newRect);
}

TEST(CSceneCutCommand, Constructor_WithItemPointer_RedoUndoNoCrash)
{
    PageScene *scene = freshScene();
    ASSERT_NE(scene, nullptr);
    auto *item = addRectItem(scene, QPointF(0, 0));
    QRectF newRect(0, 0, 200, 200);

    CSceneCutCommand cmd(scene, newRect, nullptr, item);
    EXPECT_EQ(cmd.m_item, item);
    cmd.redo();
    EXPECT_EQ(scene->sceneRect(), newRect);
    cmd.undo();
}

TEST(CSceneCutCommand, Destructor_AfterRedo_NoCrash)
{
    PageScene *scene = freshScene();
    ASSERT_NE(scene, nullptr);
    QRectF newRect(0, 0, 100, 100);

    auto *cmd = new CSceneCutCommand(scene, newRect);
    cmd->redo();
    delete cmd; // ~CSceneCutCommand is empty but must not crash
    SUCCEED();
}

// =========================================================================
// CItemsAlignCommand
//
// Constructor sets m_isMoved = true (already-applied pattern): items are
// expected to already be at their endPos when the command is created, so the
// first redo() is a no-op and undo() moves them back to startPos.
// CGraphicsItem::move(begin, end) does setPos(pos() + end - begin).
// =========================================================================

TEST(CItemsAlignCommand, Constructor_StoresMapsAndIsMovedTrue)
{
    PageScene *scene = freshScene();
    ASSERT_NE(scene, nullptr);
    auto *a = addRectItem(scene, QPointF(100, 100));
    auto *b = addRectItem(scene, QPointF(200, 100));

    QMap<CGraphicsItem *, QPointF> start{{a, QPointF(0, 0)}, {b, QPointF(10, 0)}};
    QMap<CGraphicsItem *, QPointF> end{{a, QPointF(100, 100)}, {b, QPointF(200, 100)}};

    CItemsAlignCommand cmd(scene, start, end);
    EXPECT_EQ(cmd.m_itemsStartPos, start);
    EXPECT_EQ(cmd.m_itemsEndPos, end);
    EXPECT_TRUE(cmd.m_isMoved);
    EXPECT_EQ(cmd.myGraphicsScene, scene);
}

TEST(CItemsAlignCommand, Undo_MovesItemsFromEndToStart)
{
    PageScene *scene = freshScene();
    ASSERT_NE(scene, nullptr);
    auto *a = addRectItem(scene, QPointF(100, 100)); // already at endPos
    auto *b = addRectItem(scene, QPointF(200, 100));

    QMap<CGraphicsItem *, QPointF> start{{a, QPointF(0, 0)}, {b, QPointF(10, 0)}};
    QMap<CGraphicsItem *, QPointF> end{{a, QPointF(100, 100)}, {b, QPointF(200, 100)}};

    CItemsAlignCommand cmd(scene, start, end);
    scene->selectItem(a, true, false, false);
    scene->selectItem(b, true, false, false);

    cmd.undo();
    EXPECT_EQ(a->pos(), QPointF(0, 0));
    EXPECT_EQ(b->pos(), QPointF(10, 0));
    EXPECT_FALSE(cmd.m_isMoved);
}

TEST(CItemsAlignCommand, RedoAfterUndo_MovesItemsToEnd)
{
    PageScene *scene = freshScene();
    ASSERT_NE(scene, nullptr);
    auto *a = addRectItem(scene, QPointF(100, 100));

    QMap<CGraphicsItem *, QPointF> start{{a, QPointF(0, 0)}};
    QMap<CGraphicsItem *, QPointF> end{{a, QPointF(100, 100)}};

    CItemsAlignCommand cmd(scene, start, end);
    scene->selectItem(a, true, false, false);
    cmd.undo();
    ASSERT_EQ(a->pos(), QPointF(0, 0));

    cmd.redo();
    EXPECT_EQ(a->pos(), QPointF(100, 100));
    EXPECT_TRUE(cmd.m_isMoved);
}

TEST(CItemsAlignCommand, RedoBeforeUndo_IsNoOpWhenAlreadyMoved)
{
    PageScene *scene = freshScene();
    ASSERT_NE(scene, nullptr);
    auto *a = addRectItem(scene, QPointF(100, 100));

    QMap<CGraphicsItem *, QPointF> start{{a, QPointF(0, 0)}};
    QMap<CGraphicsItem *, QPointF> end{{a, QPointF(100, 100)}};

    CItemsAlignCommand cmd(scene, start, end);
    scene->selectItem(a, true, false, false);
    // m_isMoved == true -> redo() must not move the item
    cmd.redo();
    EXPECT_EQ(a->pos(), QPointF(100, 100));
    EXPECT_TRUE(cmd.m_isMoved);
}

TEST(CItemsAlignCommand, RoundTrip_MultipleCycles_StaysConsistent)
{
    PageScene *scene = freshScene();
    ASSERT_NE(scene, nullptr);
    auto *a = addRectItem(scene, QPointF(100, 100));
    auto *b = addRectItem(scene, QPointF(200, 100));

    QMap<CGraphicsItem *, QPointF> start{{a, QPointF(0, 0)}, {b, QPointF(10, 0)}};
    QMap<CGraphicsItem *, QPointF> end{{a, QPointF(100, 100)}, {b, QPointF(200, 100)}};

    CItemsAlignCommand cmd(scene, start, end);
    scene->selectItem(a, true, false, false);
    scene->selectItem(b, true, false, false);
    for (int i = 0; i < 5; ++i) {
        cmd.undo();
        EXPECT_EQ(a->pos(), QPointF(0, 0));
        EXPECT_EQ(b->pos(), QPointF(10, 0));
        cmd.redo();
        EXPECT_EQ(a->pos(), QPointF(100, 100));
        EXPECT_EQ(b->pos(), QPointF(200, 100));
    }
}

TEST(CItemsAlignCommand, SingleItem_AlignUndoRedoWorks)
{
    PageScene *scene = freshScene();
    ASSERT_NE(scene, nullptr);
    auto *a = addRectItem(scene, QPointF(50, 50));

    QMap<CGraphicsItem *, QPointF> start{{a, QPointF(0, 0)}};
    QMap<CGraphicsItem *, QPointF> end{{a, QPointF(50, 50)}};

    CItemsAlignCommand cmd(scene, start, end);
    scene->selectItem(a, true, false, false);
    cmd.undo();
    EXPECT_EQ(a->pos(), QPointF(0, 0));
    cmd.redo();
    EXPECT_EQ(a->pos(), QPointF(50, 50));
}

TEST(CItemsAlignCommand, EmptyMaps_UndoRedo_NoCrash)
{
    PageScene *scene = freshScene();
    ASSERT_NE(scene, nullptr);
    QMap<CGraphicsItem *, QPointF> empty;

    CItemsAlignCommand cmd(scene, empty, empty);
    cmd.undo();
    EXPECT_FALSE(cmd.m_isMoved);
    cmd.redo();
    EXPECT_TRUE(cmd.m_isMoved);
}

TEST(CItemsAlignCommand, ManyItems_AlignAllPositions)
{
    PageScene *scene = freshScene();
    ASSERT_NE(scene, nullptr);
    QMap<CGraphicsItem *, QPointF> start, end;
    QList<CGraphicsItem *> items;
    for (int i = 0; i < 5; ++i) {
        auto *it = addRectItem(scene, QPointF(100, i * 10));
        items << it;
        start[it] = QPointF(0, i * 10);
        end[it] = QPointF(100, i * 10);
        scene->selectItem(it, true, false, false);
    }

    CItemsAlignCommand cmd(scene, start, end);
    cmd.undo();
    for (int i = 0; i < 5; ++i)
        EXPECT_EQ(items[i]->pos(), QPointF(0, i * 10));
    cmd.redo();
    for (int i = 0; i < 5; ++i)
        EXPECT_EQ(items[i]->pos(), QPointF(100, i * 10));
}

// =========================================================================
// Source-defect placeholders: layer command implementations are commented out
// in cundocommands.cpp. These cases skip without referencing the classes (any
// reference would cause a linker "undefined reference" error).
// =========================================================================

TEST(COneLayerUpCommand, SourceDefect_Skipped)
{
    GTEST_SKIP() << "COneLayerUpCommand: ctor/undo/redo are commented out in "
                    "cundocommands.cpp (source defect, no active definition).";
}

TEST(COneLayerDownCommand, SourceDefect_Skipped)
{
    GTEST_SKIP() << "COneLayerDownCommand: ctor/dtor/undo/redo are commented "
                    "out in cundocommands.cpp (source defect).";
}

TEST(CBringToFrontCommand, SourceDefect_Skipped)
{
    GTEST_SKIP() << "CBringToFrontCommand: ctor/undo/redo are commented out in "
                    "cundocommands.cpp (source defect).";
}

TEST(CSendToBackCommand, SourceDefect_Skipped)
{
    GTEST_SKIP() << "CSendToBackCommand: ctor/dtor/undo/redo are commented out "
                    "in cundocommands.cpp (source defect).";
}
