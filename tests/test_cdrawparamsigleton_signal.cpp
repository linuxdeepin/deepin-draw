// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for PageContext dirtyChanged signal
 * (issue V-7262)
 *
 * Source: src/drawshape/cdrawparamsigleton.cpp
 *
 * Target:
 *   void dirtyChanged(bool isDirty) signal
 *   setDirty(bool dirty) — emits dirtyChanged when `_dirty != dirty`; since
 *       `_dirty` is a signed 1-bit bitfield, only false -> false is suppressed
 *   setDefaultAttri(int type, const QVariant &var) — does NOT emit dirtyChanged
 *   isDirty() — returns current dirty state
 *
 * Note: The class is named PageContext in code (historically CDrawParamSigleton).
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QSignalSpy>
#include <QVariant>
#include <QRectF>

#define protected public
#define private public
#include "publicApi.h"
#include "cdrawparamsigleton.h"
#include "globaldefine.h"
#undef protected
#undef private

static PageContext *getPageContext()
{
    Page *page = drawApp->topMainWindow()->drawBoard()->currentPage();
    if (page == nullptr)
        return nullptr;
    return page->context();
}

// =========================================================================
// dirtyChanged signal — emitted by setDirty()
// =========================================================================

TEST(PageContextDirtyChangedTest, SetDirtyTrue_EmitsSignal)
{
    createNewViewByShortcutKey();
    PageContext *ctx = getPageContext();
    ASSERT_NE(ctx, nullptr);

    // Reset to known state
    ctx->setDirty(false);

    QSignalSpy spy(ctx, &PageContext::dirtyChanged);
    ctx->setDirty(true);

    EXPECT_EQ(spy.count(), 1);
    QList<QVariant> args = spy.takeFirst();
    EXPECT_EQ(args.at(0).toBool(), true);
}

TEST(PageContextDirtyChangedTest, SetDirtyFalse_EmitsSignal)
{
    createNewViewByShortcutKey();
    PageContext *ctx = getPageContext();
    ASSERT_NE(ctx, nullptr);

    // Set to dirty first
    ctx->setDirty(true);

    QSignalSpy spy(ctx, &PageContext::dirtyChanged);
    ctx->setDirty(false);

    EXPECT_EQ(spy.count(), 1);
    QList<QVariant> args = spy.takeFirst();
    EXPECT_EQ(args.at(0).toBool(), false);
}

TEST(PageContextDirtyChangedTest, SetDirtyTrueTwice_StillEmitsSignal)
{
    createNewViewByShortcutKey();
    PageContext *ctx = getPageContext();
    ASSERT_NE(ctx, nullptr);

    // Set to known state
    ctx->setDirty(true);

    QSignalSpy spy(ctx, &PageContext::dirtyChanged);
    // setDirty is guarded by `if (_dirty != dirty)`, but `_dirty` is declared
    // as `int _dirty: 1` — a signed 1-bit bitfield. Storing `true` yields -1,
    // so `_dirty != true` keeps holding and the signal is emitted again.
    // This test pins the current behaviour; the guard only suppresses a
    // false -> false transition (see SetDirtyFalseTwice_DoesNotEmitSignal).
    ctx->setDirty(true);

    EXPECT_EQ(spy.count(), 1);
}

TEST(PageContextDirtyChangedTest, SetDirtyFalseTwice_DoesNotEmitSignal)
{
    createNewViewByShortcutKey();
    PageContext *ctx = getPageContext();
    ASSERT_NE(ctx, nullptr);

    // Set to known state (false is stored as 0, so the guard works here)
    ctx->setDirty(false);

    QSignalSpy spy(ctx, &PageContext::dirtyChanged);
    ctx->setDirty(false);

    EXPECT_EQ(spy.count(), 0);
}

TEST(PageContextDirtyChangedTest, SetDirtyToggle_EmitsEachChange)
{
    createNewViewByShortcutKey();
    PageContext *ctx = getPageContext();
    ASSERT_NE(ctx, nullptr);

    ctx->setDirty(false);

    QSignalSpy spy(ctx, &PageContext::dirtyChanged);
    ctx->setDirty(true);   // emit (false -> true)
    ctx->setDirty(false);  // emit (true -> false)
    ctx->setDirty(true);   // emit (false -> true)
    // `_dirty` is `int _dirty: 1`, so true is stored as -1 and the
    // `_dirty != dirty` guard lets a true -> true transition through.
    ctx->setDirty(true);   // emit (-1 -> true)
    ctx->setDirty(false);  // emit (true -> false)

    EXPECT_EQ(spy.count(), 5);
}

// =========================================================================
// isDirty — reflects setDirty calls
// =========================================================================

TEST(PageContextDirtyChangedTest, IsDirty_ReflectsSetDirty)
{
    createNewViewByShortcutKey();
    PageContext *ctx = getPageContext();
    ASSERT_NE(ctx, nullptr);

    ctx->setDirty(true);
    EXPECT_TRUE(ctx->isDirty());

    ctx->setDirty(false);
    EXPECT_FALSE(ctx->isDirty());
}

// =========================================================================
// setDefaultAttri does NOT emit dirtyChanged
// =========================================================================

TEST(PageContextDirtyChangedTest, SetDefaultAttri_DoesNotEmitDirtyChanged)
{
    createNewViewByShortcutKey();
    PageContext *ctx = getPageContext();
    ASSERT_NE(ctx, nullptr);

    ctx->setDirty(false);

    QSignalSpy spy(ctx, &PageContext::dirtyChanged);
    ctx->setDefaultAttri(100, QVariant(42));

    // setDefaultAttri is a simple setter — it does NOT emit dirtyChanged
    EXPECT_EQ(spy.count(), 0);
}
