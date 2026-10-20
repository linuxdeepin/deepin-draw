// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for PageView cache & state queries (src/frame/cgraphicsview.cpp).
 *
 * Covers (issue V-5981 §二):
 *   - setCacheEnable / isCacheEnabled — toggle on/off
 *   - isKeySpacePressed              — space key press/release state
 *   - isModified                     — scene modification flag
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QKeyEvent>

#define protected public
#define private public
#include "publicApi.h"
#undef protected
#undef private

TEST(PageViewCacheTest, SetCacheEnable_TrueThenIsCacheEnabledTrue)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    view->setCacheEnable(true);
    EXPECT_TRUE(view->isCacheEnabled());
}

TEST(PageViewCacheTest, SetCacheEnable_FalseThenIsCacheEnabledFalse)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    view->setCacheEnable(true);
    view->setCacheEnable(false);
    EXPECT_FALSE(view->isCacheEnabled());
}

TEST(PageViewCacheTest, CachedPixmap_NonEmptyAfterEnableWithScene)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    // Add a small image so the scene has content to cache.
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);
    PageContext *ctx = scene->pageContext();
    ASSERT_NE(ctx, nullptr);
    QImage img(16, 16, QImage::Format_ARGB32);
    img.fill(Qt::red);
    ctx->addImage(img, QPointF(0, 0), QRectF(0, 0, 16, 16), false, false);
    qApp->processEvents();

    view->setCacheEnable(true, true);
    EXPECT_FALSE(view->cachedPixmap().isNull());
}

TEST(PageViewCacheTest, SetCacheEnable_ToggleRoundTripRestoresState)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    view->setCacheEnable(false);
    EXPECT_FALSE(view->isCacheEnabled());
    view->setCacheEnable(true);
    EXPECT_TRUE(view->isCacheEnabled());
    view->setCacheEnable(false);
    EXPECT_FALSE(view->isCacheEnabled());
}

TEST(PageViewKeySpaceTest, IsKeySpacePressed_DefaultFalse)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    EXPECT_FALSE(view->isKeySpacePressed());
}

TEST(PageViewKeySpaceTest, IsKeySpacePressed_TrueAfterSpacePress)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    QKeyEvent press(QEvent::KeyPress, Qt::Key_Space, Qt::NoModifier);
    view->keyPressEvent(&press);
    EXPECT_TRUE(view->isKeySpacePressed());
}

TEST(PageViewKeySpaceTest, IsKeySpacePressed_FalseAfterSpaceRelease)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    QKeyEvent press(QEvent::KeyPress, Qt::Key_Space, Qt::NoModifier);
    view->keyPressEvent(&press);
    EXPECT_TRUE(view->isKeySpacePressed());

    QKeyEvent release(QEvent::KeyRelease, Qt::Key_Space, Qt::NoModifier);
    view->keyReleaseEvent(&release);
    EXPECT_FALSE(view->isKeySpacePressed());
}

TEST(PageViewModifiedTest, IsModified_FalseOnFreshView)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    // A fresh view with no edits should not be in modified state.
    EXPECT_FALSE(view->isModified());
}

TEST(PageViewModifiedTest, IsModified_TrueAfterSceneEdit)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    // pushActionCount increments the user action counter, which
    // isModified() checks to determine whether the view has been
    // modified since the last save.
    view->pushActionCount();
    EXPECT_TRUE(view->isModified());
}
