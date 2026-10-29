// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CFunUndoRedo and CItemSizeCommand (issue V-7155)
 *
 * Source: src/frame/cundoredocommand.h (inline), src/frame/cundoredocommand.cpp
 *
 * Branch map:
 *   B1  CFunUndoRedo constructor with default nullptr args
 *   B2  CFunUndoRedo constructor with valid undo/redo functors
 *   B3  setFun replaces existing functors
 *   B4  real_undo calls _uf when valid
 *   B5  real_undo no-op when _uf is nullptr
 *   B6  real_redo calls _rf when _uf is valid (note: impl checks _uf not _rf)
 *   B7  real_redo returns early when _uf is nullptr
 *   B8  CItemSizeCommand default constructor
 *   B9  parsingVars with vars.count() < 2 → early return, _sz untouched
 *   B10 parsingVars with 2 vars (UndoVar) → _sz[UndoVar] set
 *   B11 parsingVars with 2 vars (RedoVar) → _sz[RedoVar] set
 *   B12 parsingVars sets _pItem via base class
 *   B13 real_undo / real_redo are empty no-ops: the item size is NOT restored
 *       or reapplied (known defect, documented not fixed — source changes
 *       disallowed); safe no-op for a null item too
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <functional>

#include <QGraphicsRectItem>

#include "frame/cundoredocommand.h"
#include "drawshape/globaldefine.h"

#if TEST_UNDOREDO_FUNCS2_ITEM

using FunType = std::function<void()>;

// Note: CItemSizeCommand::_sz and CItemUndoRedoCommand::_pItem are private
// with no usable getters — var()/item() are declared inline in the header
// but defined out-of-line in the .cpp, so no linkable symbol exists for them
// and tests must not call them. The stored vars are instead read back through
// the protected CUndoRedoCommand::_vars via a test subclass (normal
// inheritance access; production class tokens stay identical across TUs).

namespace {
class TestableItemSizeCommand : public CItemSizeCommand
{
public:
    const QList<QVariant> &storedVars(EVarUndoOrRedo tp) const
    {
        return _vars[tp];
    }
};
} // namespace

// =========================================================================
// CFunUndoRedo
// =========================================================================

TEST(CFunUndoRedoTest, DefaultConstructor_NullFunctors)
{
    // B1: constructor with default nullptr args — functors are null, so
    // real_undo/real_redo must complete without any side effect or crash
    CFunUndoRedo<> cmd;
    CFunUndoRedo<FunType, FunType> cmdTyped;
    cmd.real_undo();
    cmd.real_redo();
    SUCCEED();
}

TEST(CFunUndoRedoTest, ConstructorWithFunctors)
{
    // B2: constructor stores both functors (verified behaviorally through
    // the public real_undo/real_redo entry points)
    int undoCount = 0;
    int redoCount = 0;
    auto uf = [&undoCount]() { undoCount++; };
    auto rf = [&redoCount]() { redoCount++; };

    CFunUndoRedo<FunType, FunType> cmd(uf, rf);
    cmd.real_undo();
    cmd.real_redo();
    EXPECT_EQ(undoCount, 1);
    EXPECT_EQ(redoCount, 1);
}

TEST(CFunUndoRedoTest, SetFun_ReplacesExistingFunctors)
{
    // B3: setFun replaces existing functors
    int count1 = 0;
    int count2 = 0;
    auto f1 = [&count1]() { count1++; };
    auto f2 = [&count2]() { count2++; };

    CFunUndoRedo<FunType, FunType> cmd(f1, f2);
    cmd.real_undo();
    EXPECT_EQ(count1, 1);

    // Replace functors
    int count3 = 0;
    int count4 = 0;
    auto f3 = [&count3]() { count3++; };
    auto f4 = [&count4]() { count4++; };
    cmd.setFun(f3, f4);

    cmd.real_undo();
    EXPECT_EQ(count3, 1);
    EXPECT_EQ(count1, 1); // old functor not called again

    cmd.real_redo();
    EXPECT_EQ(count4, 1);
    EXPECT_EQ(count2, 0); // old redo functor not called
}

TEST(CFunUndoRedoTest, RealUndo_CallsUndoFunctor)
{
    // B4: real_undo calls _uf when valid
    int undoCount = 0;
    auto uf = [&undoCount]() { undoCount++; };
    auto rf = []() {};

    CFunUndoRedo<FunType, FunType> cmd(uf, rf);
    cmd.real_undo();
    EXPECT_EQ(undoCount, 1);
}

TEST(CFunUndoRedoTest, RealUndo_NullFunctor_NoCrash)
{
    // B5: real_undo no-op when _uf is nullptr
    CFunUndoRedo<> cmd;
    cmd.real_undo();
    SUCCEED();
}

TEST(CFunUndoRedoTest, RealRedo_CallsRedoFunctor_WhenUndoValid)
{
    // B6: real_redo calls _rf when both callbacks are valid
    int undoCount = 0;
    int redoCount = 0;
    auto uf = [&undoCount]() { undoCount++; };
    auto rf = [&redoCount]() { redoCount++; };

    CFunUndoRedo<FunType, FunType> cmd(uf, rf);
    cmd.real_redo();
    EXPECT_EQ(redoCount, 1);
    EXPECT_EQ(undoCount, 0); // undo functor not called by real_redo
}

TEST(CFunUndoRedoTest, RealRedo_NullUndoFunctor_SkipsRedoFunctor)
{
    // B7: KNOWN QUIRK (documented, not fixed — source changes disallowed):
    // real_redo guards on _uf, not _rf, so a valid redo callback is skipped
    // when the undo callback is missing
    int redoCount = 0;
    auto rf = [&redoCount]() { redoCount++; };

    CFunUndoRedo<FunType, FunType> cmd(nullptr, rf);
    cmd.real_redo();
    EXPECT_EQ(redoCount, 0); // current behavior: skipped because _uf is null
}

TEST(CFunUndoRedoTest, RealRedo_NullRedoFunctor_ThrowsBadFunctionCall)
{
    // B7b: KNOWN QUIRK (documented, not fixed — source changes disallowed):
    // with a valid _uf but missing _rf, real_redo still invokes _rf() and an
    // empty std::function throws std::bad_function_call (standard-guaranteed)
    int undoCount = 0;
    auto uf = [&undoCount]() { undoCount++; };

    CFunUndoRedo<FunType, FunType> cmd(uf, nullptr);
    EXPECT_THROW(cmd.real_redo(), std::bad_function_call);
    EXPECT_EQ(undoCount, 0);

    // default-constructed empty std::function behaves the same
    CFunUndoRedo<FunType, FunType> cmdEmpty(uf, FunType());
    EXPECT_THROW(cmdEmpty.real_redo(), std::bad_function_call);
}

// =========================================================================
// CItemSizeCommand
// =========================================================================

TEST(CItemSizeCommandTest, DefaultConstructor_NoCrash)
{
    // B8: default constructor
    CItemSizeCommand cmd;
    SUCCEED();
}

TEST(CItemSizeCommandTest, ParsingVars_TooFewVars_EarlyReturn)
{
    // B9: vars.count() < 2 → parsingVars returns before parsing the size
    // (_sz is write-only private state with no getter, so the early return
    // itself is not directly observable here). What this test does verify is
    // that the base-class parsing still runs first: the item pointer is stored
    // and the size var is absent (count stays 1).
    TestableItemSizeCommand cmd;
    QGraphicsRectItem rectItem;

    QList<QVariant> vars;
    vars << reinterpret_cast<long long>(&rectItem);  // only 1 var → count < 2
    cmd.setVar(vars, UndoVar);

    EXPECT_EQ(cmd.storedVars(UndoVar).count(), 1);
    EXPECT_EQ(cmd.storedVars(UndoVar).first().toLongLong(),
              reinterpret_cast<long long>(&rectItem));
}

TEST(CItemSizeCommandTest, ParsingVars_TwoVarsUndoVar_SetsSize)
{
    // B10: parsingVars with 2 vars (UndoVar) → _sz[UndoVar] set from vars[1]
    // (_sz is internal write-only state; check the stored var roundtrip)
    TestableItemSizeCommand cmd;
    QSizeF expectedSize(100.0, 200.0);

    QList<QVariant> vars;
    vars << reinterpret_cast<long long>(nullptr);  // item pointer (dummy)
    vars << QVariant(expectedSize);
    cmd.setVar(vars, UndoVar);

    EXPECT_EQ(cmd.storedVars(UndoVar).at(1).toSizeF(), expectedSize);
}

TEST(CItemSizeCommandTest, ParsingVars_TwoVarsRedoVar_SetsSize)
{
    // B11: parsingVars with 2 vars (RedoVar) → _sz[RedoVar] set from vars[1]
    TestableItemSizeCommand cmd;
    QSizeF expectedSize(300.0, 400.0);

    QList<QVariant> vars;
    vars << reinterpret_cast<long long>(nullptr);
    vars << QVariant(expectedSize);
    cmd.setVar(vars, RedoVar);

    EXPECT_EQ(cmd.storedVars(RedoVar).at(1).toSizeF(), expectedSize);
}

TEST(CItemSizeCommandTest, ParsingVars_SetsItemPointerViaBase)
{
    // B12: parsingVars sets the item pointer via base class
    // CItemUndoRedoCommand::parsingVars (the parsed pointer is private state;
    // assert the stored var that feeds it)
    TestableItemSizeCommand cmd;

    QGraphicsRectItem rectItem;
    auto ptrValue = reinterpret_cast<long long>(&rectItem);

    QList<QVariant> vars;
    vars << ptrValue;
    vars << QVariant(QSizeF(50.0, 60.0));
    cmd.setVar(vars, UndoVar);

    EXPECT_EQ(cmd.storedVars(UndoVar).first().toLongLong(), ptrValue);
}

TEST(CItemSizeCommandTest, RealUndoRealRedo_EmptyImpl_SizeUnchanged)
{
    // B13: KNOWN DEFECT (documented, not fixed — source changes disallowed):
    // real_undo/real_redo have empty bodies, so neither the undo nor the redo
    // size is ever applied to the item. The recorded sizes are parsed into
    // _sz[] but never used, so the item keeps its original rect.
    QGraphicsRectItem rectItem(QRectF(5.0, 7.0, 50.0, 60.0));
    const QSizeF originalSize = rectItem.rect().size();

    CItemSizeCommand cmd;
    QList<QVariant> undoVars;
    undoVars << reinterpret_cast<long long>(&rectItem);
    undoVars << QVariant(QSizeF(10.0, 20.0));
    cmd.setVar(undoVars, UndoVar);

    QList<QVariant> redoVars;
    redoVars << reinterpret_cast<long long>(&rectItem);
    redoVars << QVariant(QSizeF(30.0, 40.0));
    cmd.setVar(redoVars, RedoVar);

    cmd.real_undo();
    EXPECT_EQ(rectItem.rect().size(), originalSize); // size not restored

    cmd.real_redo();
    EXPECT_EQ(rectItem.rect().size(), originalSize); // size not reapplied
}

TEST(CItemSizeCommandTest, RealUndoRealRedo_NullItem_NoCrash)
{
    // B13b: null item pointer → real_undo/real_redo are safe no-ops
    CItemSizeCommand cmd;

    QList<QVariant> vars;
    vars << reinterpret_cast<long long>(nullptr);
    vars << QVariant(QSizeF(10.0, 20.0));
    cmd.setVar(vars, UndoVar);
    cmd.setVar(vars, RedoVar);

    cmd.real_undo();
    cmd.real_redo();
    SUCCEED();
}

#endif // TEST_UNDOREDO_FUNCS2_ITEM
