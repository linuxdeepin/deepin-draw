// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for PageContext methods (issue V-5645)
 *
 * Source: src/drawshape/cdrawparamsigleton.cpp
 *
 * Target methods:
 *   PageContext: setDefaultAttri(...)/defaultAttri(...),
 *                pageRect()/setPageRect(...)
 *
 * Pattern: follows test_pagecontext.cpp / test_pagecontext_addsceneitem.cpp —
 *          uses createNewViewByShortcutKey() and gets PageContext
 *          from the current page's context.
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QRectF>
#include <QVariant>

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
// PageContext::setDefaultAttri / defaultAttri
// =========================================================================

TEST(PageContextAttriTest, SetAndGet_DefaultAttri)
{
    createNewViewByShortcutKey();
    PageContext *ctx = getPageContext();
    ASSERT_NE(ctx, nullptr);

    QVariant value(42);
    ctx->setDefaultAttri(100, value);

    QVariant got = ctx->defaultAttri(100);
    EXPECT_EQ(got.toInt(), 42);
}

TEST(PageContextAttriTest, DefaultAttri_NotSet_ReturnsInvalid)
{
    createNewViewByShortcutKey();
    PageContext *ctx = getPageContext();
    ASSERT_NE(ctx, nullptr);

    QVariant got = ctx->defaultAttri(9999);
    EXPECT_FALSE(got.isValid());
}

TEST(PageContextAttriTest, SetDefaultAttri_Overwrite)
{
    createNewViewByShortcutKey();
    PageContext *ctx = getPageContext();
    ASSERT_NE(ctx, nullptr);

    ctx->setDefaultAttri(200, QVariant("first"));
    ctx->setDefaultAttri(200, QVariant("second"));

    QVariant got = ctx->defaultAttri(200);
    EXPECT_EQ(got.toString(), QString("second"));
}

TEST(PageContextAttriTest, SetDefaultAttri_StringValue)
{
    createNewViewByShortcutKey();
    PageContext *ctx = getPageContext();
    ASSERT_NE(ctx, nullptr);

    ctx->setDefaultAttri(300, QVariant(QString("hello")));

    QVariant got = ctx->defaultAttri(300);
    EXPECT_EQ(got.toString(), QString("hello"));
}

TEST(PageContextAttriTest, SetDefaultAttri_RectValue)
{
    createNewViewByShortcutKey();
    PageContext *ctx = getPageContext();
    ASSERT_NE(ctx, nullptr);

    QRectF rect(0, 0, 100, 200);
    ctx->setDefaultAttri(400, rect);

    QVariant got = ctx->defaultAttri(400);
    EXPECT_EQ(got.toRectF(), rect);
}

// =========================================================================
// PageContext::pageRect / setPageRect
// =========================================================================

TEST(PageContextRectTest, DefaultPageRect_NotEmpty)
{
    createNewViewByShortcutKey();
    PageContext *ctx = getPageContext();
    ASSERT_NE(ctx, nullptr);

    QRectF rect = ctx->pageRect();
    // Default page rect should be valid (non-zero area).
    EXPECT_GT(rect.width(), 0);
    EXPECT_GT(rect.height(), 0);
}

TEST(PageContextRectTest, SetPageRect_ThenGet)
{
    createNewViewByShortcutKey();
    PageContext *ctx = getPageContext();
    ASSERT_NE(ctx, nullptr);

    QRectF newRect(10, 20, 300, 400);
    ctx->setPageRect(newRect);

    QRectF got = ctx->pageRect();
    EXPECT_EQ(got, newRect);
}

TEST(PageContextRectTest, SetPageRect_ZeroSize)
{
    createNewViewByShortcutKey();
    PageContext *ctx = getPageContext();
    ASSERT_NE(ctx, nullptr);

    QRectF zeroRect(0, 0, 0, 0);
    ctx->setPageRect(zeroRect);

    QRectF got = ctx->pageRect();
    EXPECT_EQ(got, zeroRect);
}

TEST(PageContextRectTest, SetPageRect_Overwrite)
{
    createNewViewByShortcutKey();
    PageContext *ctx = getPageContext();
    ASSERT_NE(ctx, nullptr);

    ctx->setPageRect(QRectF(0, 0, 100, 100));
    ctx->setPageRect(QRectF(50, 50, 200, 200));

    QRectF got = ctx->pageRect();
    EXPECT_EQ(got, QRectF(50, 50, 200, 200));
}
