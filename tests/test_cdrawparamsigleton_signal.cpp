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
 *   setDirty(bool dirty) — emits dirtyChanged when dirty state changes
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

TEST(PageContextDirtyChangedTest, SetDirtySameValue_StillEmitsSignal)
{
    createNewViewByShortcutKey();
    PageContext *ctx = getPageContext();
    ASSERT_NE(ctx, nullptr);

    // Set to known state
    ctx->setDirty(true);

    QSignalSpy spy(ctx, &PageContext::dirtyChanged);
    // setDirty emits dirtyChanged unconditionally (even when value is unchanged)
    ctx->setDirty(true);

    EXPECT_EQ(spy.count(), 1);
}

TEST(PageContextDirtyChangedTest, SetDirtyToggle_EmitsEachChange)
{
    createNewViewByShortcutKey();
    PageContext *ctx = getPageContext();
    ASSERT_NE(ctx, nullptr);

    ctx->setDirty(false);

    QSignalSpy spy(ctx, &PageContext::dirtyChanged);
    ctx->setDirty(true);   // emit
    ctx->setDirty(false);  // emit
    ctx->setDirty(true);   // emit
    ctx->setDirty(true);   // emit (setDirty emits unconditionally)
    ctx->setDirty(false);  // emit

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
