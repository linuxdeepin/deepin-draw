// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CItemMoveCommand and CSceneBoundingChangedCommand (issue V-8176)
 *
 * Source: src/frame/cundoredocommand.cpp / .h
 *
 * Target methods (0-coverage per existing .gcda):
 *   CUndoRedoCommand::real_undo() / real_redo()        (base no-ops)
 *   CUndoRedoCommand::parsingVars(...)                  (base no-op)
 *   CItemMoveCommand::real_undo() / real_redo()
 *   CSceneBoundingChangedCommand::real_undo() / real_redo()
 *
 * CItemMoveCommand::real_undo sets item()->setPos(_pos[UndoVar]);
 * real_redo sets item()->setPos(_pos[RedoVar]).
 * CSceneBoundingChangedCommand::real_undo sets scene()->setSceneRect(_rect[UndoVar]);
 * real_redo sets scene()->setSceneRect(_rect[RedoVar]).
 */

#include <gtest/gtest.h>

#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QPointF>
#include <QRectF>
#include <QVariant>

#define protected public
#define private public
#include "frame/cundoredocommand.h"
#include "drawshape/globaldefine.h"
#undef protected
#undef private

// =========================================================================
// CUndoRedoCommand base no-ops
// =========================================================================

TEST(CUndoRedoCommandMisc, BaseRealUndoIsNoOp)
{
    CUndoRedoCommand cmd;
    cmd.real_undo();
    SUCCEED();
}

TEST(CUndoRedoCommandMisc, BaseRealRedoIsNoOp)
{
    CUndoRedoCommand cmd;
    cmd.real_redo();
    SUCCEED();
}

TEST(CUndoRedoCommandMisc, BaseParsingVarsIsNoOp)
{
    CUndoRedoCommand cmd;
    QList<QVariant> vars;
    vars << QVariant(42);
    cmd.parsingVars(vars, UndoVar);
    SUCCEED();
}

// =========================================================================
// CItemMoveCommand
// =========================================================================

TEST(CItemMoveCommandMisc, DefaultConstructorNoCrash)
{
    CItemMoveCommand cmd;
    SUCCEED();
}

TEST(CItemMoveCommandMisc, RealUndoSetsItemPos)
{
    CItemMoveCommand cmd;
    QGraphicsRectItem item;
    item.setPos(QPointF(0, 0));

    QPointF undoPos(100.0, 200.0);
    QList<QVariant> undoVars;
    undoVars << QVariant(reinterpret_cast<qlonglong>(&item));
    undoVars << QVariant(undoPos);
    cmd.setVar(undoVars, UndoVar);

    cmd.real_undo();
    EXPECT_EQ(item.pos(), undoPos);
}

TEST(CItemMoveCommandMisc, RealRedoSetsItemPos)
{
    CItemMoveCommand cmd;
    QGraphicsRectItem item;
    item.setPos(QPointF(0, 0));

    QPointF redoPos(300.0, 400.0);
    QList<QVariant> redoVars;
    redoVars << QVariant(reinterpret_cast<qlonglong>(&item));
    redoVars << QVariant(redoPos);
    cmd.setVar(redoVars, RedoVar);

    cmd.real_redo();
    EXPECT_EQ(item.pos(), redoPos);
}

TEST(CItemMoveCommandMisc, RealUndoThenRedo)
{
    CItemMoveCommand cmd;
    QGraphicsRectItem item;

    QPointF undoPos(10.0, 20.0);
    QPointF redoPos(50.0, 60.0);

    QList<QVariant> undoVars;
    undoVars << QVariant(reinterpret_cast<qlonglong>(&item));
    undoVars << QVariant(undoPos);
    cmd.setVar(undoVars, UndoVar);

    QList<QVariant> redoVars;
    redoVars << QVariant(reinterpret_cast<qlonglong>(&item));
    redoVars << QVariant(redoPos);
    cmd.setVar(redoVars, RedoVar);

    cmd.real_undo();
    EXPECT_EQ(item.pos(), undoPos);

    cmd.real_redo();
    EXPECT_EQ(item.pos(), redoPos);
}

TEST(CItemMoveCommandMisc, RealUndoNullItemNoCrash)
{
    CItemMoveCommand cmd;
    QList<QVariant> vars;
    vars << QVariant(static_cast<qlonglong>(0));
    vars << QVariant(QPointF(1.0, 2.0));
    cmd.setVar(vars, UndoVar);

    cmd.real_undo();
    SUCCEED();
}

TEST(CItemMoveCommandMisc, RealRedoNullItemNoCrash)
{
    CItemMoveCommand cmd;
    QList<QVariant> vars;
    vars << QVariant(static_cast<qlonglong>(0));
    vars << QVariant(QPointF(3.0, 4.0));
    cmd.setVar(vars, RedoVar);

    cmd.real_redo();
    SUCCEED();
}

TEST(CItemMoveCommandMisc, ParsingVarsTooFewVarsNoCrash)
{
    CItemMoveCommand cmd;
    QList<QVariant> vars;
    vars << QVariant(static_cast<qlonglong>(0));  // only 1 var, count < 2
    cmd.setVar(vars, UndoVar);
    SUCCEED();
}

// =========================================================================
// CSceneBoundingChangedCommand
// =========================================================================

TEST(CSceneBoundingChangedCommandMisc, DefaultConstructorNoCrash)
{
    CSceneBoundingChangedCommand cmd;
    SUCCEED();
}

TEST(CSceneBoundingChangedCommandMisc, RealUndoSetsSceneRect)
{
    CSceneBoundingChangedCommand cmd;
    QGraphicsScene scene;
    scene.setSceneRect(QRectF(0, 0, 100, 100));

    QRectF undoRect(0, 0, 200, 200);
    QList<QVariant> undoVars;
    undoVars << QVariant(reinterpret_cast<qlonglong>(&scene));
    undoVars << QVariant(undoRect);
    cmd.setVar(undoVars, UndoVar);

    cmd.real_undo();
    EXPECT_EQ(scene.sceneRect(), undoRect);
}

TEST(CSceneBoundingChangedCommandMisc, RealRedoSetsSceneRect)
{
    CSceneBoundingChangedCommand cmd;
    QGraphicsScene scene;
    scene.setSceneRect(QRectF(0, 0, 100, 100));

    QRectF redoRect(10, 10, 300, 300);
    QList<QVariant> redoVars;
    redoVars << QVariant(reinterpret_cast<qlonglong>(&scene));
    redoVars << QVariant(redoRect);
    cmd.setVar(redoVars, RedoVar);

    cmd.real_redo();
    EXPECT_EQ(scene.sceneRect(), redoRect);
}

TEST(CSceneBoundingChangedCommandMisc, RealUndoThenRedo)
{
    CSceneBoundingChangedCommand cmd;
    QGraphicsScene scene;

    QRectF undoRect(0, 0, 150, 150);
    QRectF redoRect(5, 5, 250, 250);

    QList<QVariant> undoVars;
    undoVars << QVariant(reinterpret_cast<qlonglong>(&scene));
    undoVars << QVariant(undoRect);
    cmd.setVar(undoVars, UndoVar);

    QList<QVariant> redoVars;
    redoVars << QVariant(reinterpret_cast<qlonglong>(&scene));
    redoVars << QVariant(redoRect);
    cmd.setVar(redoVars, RedoVar);

    cmd.real_undo();
    EXPECT_EQ(scene.sceneRect(), undoRect);

    cmd.real_redo();
    EXPECT_EQ(scene.sceneRect(), redoRect);
}

TEST(CSceneBoundingChangedCommandMisc, RealUndoNullSceneNoCrash)
{
    CSceneBoundingChangedCommand cmd;
    QList<QVariant> vars;
    vars << QVariant(static_cast<qlonglong>(0));
    vars << QVariant(QRectF(0, 0, 10, 10));
    cmd.setVar(vars, UndoVar);

    cmd.real_undo();
    SUCCEED();
}

TEST(CSceneBoundingChangedCommandMisc, RealRedoNullSceneNoCrash)
{
    CSceneBoundingChangedCommand cmd;
    QList<QVariant> vars;
    vars << QVariant(static_cast<qlonglong>(0));
    vars << QVariant(QRectF(0, 0, 20, 20));
    cmd.setVar(vars, RedoVar);

    cmd.real_redo();
    SUCCEED();
}

TEST(CSceneBoundingChangedCommandMisc, ParsingVarsTooFewVarsNoCrash)
{
    CSceneBoundingChangedCommand cmd;
    QList<QVariant> vars;
    vars << QVariant(static_cast<qlonglong>(0));  // only 1 var
    cmd.setVar(vars, UndoVar);
    SUCCEED();
}
