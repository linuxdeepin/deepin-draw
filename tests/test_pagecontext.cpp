// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QString>

#define protected public
#define private public
#include "publicApi.h"
#include "ccentralwidget.h"
#include "globaldefine.h"
#include "cdrawparamsigleton.h"

/*
 * PageContext unit tests (V-5185)
 *
 * Exercises the key methods:
 *   constructor, isEmpty, isDirty, file, name, key,
 *   page, scene, update, save
 *
 * Note: genericOneKey() is a file-local (static) free function in
 * cdrawparamsigleton.cpp and is not callable from a separate
 * translation unit, so it is not tested here.
 *
 * PageContext instances are obtained from Page::context().
 * The application-managed page is reached via:
 *   drawApp->topMainWindow()->drawBoard()->currentPage()
 */

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------

TEST(PageContextConstructorTest, ConstructsWithFilePath)
{
    PageContext ctx("test_file.ddf", nullptr);
    // A freshly constructed context should be empty and not dirty.
    EXPECT_TRUE(ctx.isEmpty());
    EXPECT_FALSE(ctx.isDirty());
}

// ---------------------------------------------------------------------------
// isEmpty
// ---------------------------------------------------------------------------

TEST(PageContextIsEmptyTest, FreshPageContextIsEmpty)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);
    Page *page = board->currentPage();
    ASSERT_NE(page, nullptr);
    PageContext *ctx = page->context();
    ASSERT_NE(ctx, nullptr);

    // A fresh page with no drawn items has an empty context.
    EXPECT_TRUE(ctx->isEmpty());
}

// ---------------------------------------------------------------------------
// isDirty
// ---------------------------------------------------------------------------

TEST(PageContextIsDirtyTest, SetDirtyTogglesFlag)
{
    createNewViewByShortcutKey();
    Page *page = drawApp->topMainWindow()->drawBoard()->currentPage();
    ASSERT_NE(page, nullptr);
    PageContext *ctx = page->context();
    ASSERT_NE(ctx, nullptr);

    ctx->setDirty(false);
    EXPECT_FALSE(ctx->isDirty());

    ctx->setDirty(true);
    EXPECT_TRUE(ctx->isDirty());

    ctx->setDirty(false);
    EXPECT_FALSE(ctx->isDirty());
}

// ---------------------------------------------------------------------------
// file
// ---------------------------------------------------------------------------

TEST(PageContextFileTest, ReturnsFilePathFromConstructor)
{
    PageContext ctx("/tmp/test_page.ddf", nullptr);
    EXPECT_EQ(ctx.file(), QString("/tmp/test_page.ddf"));
}

TEST(PageContextFileTest, SetFileUpdatesFilePath)
{
    PageContext ctx("", nullptr);
    ctx.setFile("/tmp/updated.ddf");
    EXPECT_EQ(ctx.file(), QString("/tmp/updated.ddf"));
}

// ---------------------------------------------------------------------------
// name
// ---------------------------------------------------------------------------

TEST(PageContextNameTest, SetNameReturnsName)
{
    PageContext ctx("", nullptr);
    ctx.setName("MyPage");
    EXPECT_EQ(ctx.name(), QString("MyPage"));
}

// ---------------------------------------------------------------------------
// key
// ---------------------------------------------------------------------------

TEST(PageContextKeyTest, ReturnsNonEmptyKey)
{
    PageContext ctx("test.ddf", nullptr);
    // key() should return a valid identifier string.
    EXPECT_FALSE(ctx.key().isEmpty());
}

// ---------------------------------------------------------------------------
// page
// ---------------------------------------------------------------------------

TEST(PageContextPageTest, ReturnsOwningPage)
{
    createNewViewByShortcutKey();
    Page *page = drawApp->topMainWindow()->drawBoard()->currentPage();
    ASSERT_NE(page, nullptr);
    PageContext *ctx = page->context();
    ASSERT_NE(ctx, nullptr);

    // The context's page should be the page that owns it.
    EXPECT_EQ(ctx->page(), page);
}

// ---------------------------------------------------------------------------
// scene
// ---------------------------------------------------------------------------

TEST(PageContextSceneTest, ReturnsNonNullScene)
{
    createNewViewByShortcutKey();
    Page *page = drawApp->topMainWindow()->drawBoard()->currentPage();
    ASSERT_NE(page, nullptr);
    PageContext *ctx = page->context();
    ASSERT_NE(ctx, nullptr);

    PageScene *scene = ctx->scene();
    EXPECT_NE(scene, nullptr);
}

// ---------------------------------------------------------------------------
// update
// ---------------------------------------------------------------------------

TEST(PageContextUpdateTest, DoesNotCrash)
{
    createNewViewByShortcutKey();
    Page *page = drawApp->topMainWindow()->drawBoard()->currentPage();
    ASSERT_NE(page, nullptr);
    PageContext *ctx = page->context();
    ASSERT_NE(ctx, nullptr);

    ctx->update();
    SUCCEED();
}

// ---------------------------------------------------------------------------
// save
// ---------------------------------------------------------------------------

TEST(PageContextSaveTest, SaveEmptyContextDoesNotCrash)
{
    createNewViewByShortcutKey();
    Page *page = drawApp->topMainWindow()->drawBoard()->currentPage();
    ASSERT_NE(page, nullptr);
    PageContext *ctx = page->context();
    ASSERT_NE(ctx, nullptr);

    // Save an empty context to a temp file — should not crash.
    QString tmpFile = "/tmp/test_pagecontext_save.ddf";
    (void)ctx->save(tmpFile);
    SUCCEED();
}


