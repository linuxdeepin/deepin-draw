// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for PageContext::addSceneItem (cdrawparamsigleton.cpp).
 *
 * Target function (from issue V-5568):
 *   - PageContext::addSceneItem(const CGraphicsUnit &, bool, bool, bool)
 *
 * Requires a running application instance (drawApp) because it calls
 * CGraphicsItem::creatItemInstance internally.
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QImage>
#include <QPointF>
#include <QRectF>

#define protected public
#define private public
#include "publicApi.h"
#undef protected
#undef private
#define protected public
#define private public
#include "cdrawparamsigleton.h"
#include "cdrawscene.h"
#include "sitemdata.h"
#include "globaldefine.h"
#undef protected
#undef private

static PageContext *getPageContext()
{
    PageView *view = getCurView();
    if (view == nullptr)
        return nullptr;
    return view->drawScene()->pageContext();
}

TEST(PageContextAddSceneItemTest, AddImageItemCreatesSceneItem)
{
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    PageContext *ctx = getPageContext();
    ASSERT_NE(ctx, nullptr);
    ASSERT_NE(ctx->scene(), nullptr);

    int beforeCount = ctx->scene()->items().size();

    QImage img(32, 32, QImage::Format_ARGB32);
    img.fill(Qt::blue);
    ctx->addImage(img, QPointF(0, 0), QRectF(0, 0, 32, 32), false, false);

    qApp->processEvents();

    int afterCount = ctx->scene()->items().size();
    EXPECT_GT(afterCount, beforeCount);
}

TEST(PageContextAddSceneItemTest, AddSceneItemWithInvalidTypeNoCrash)
{
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    PageContext *ctx = getPageContext();
    ASSERT_NE(ctx, nullptr);

    // Use a type value that doesn't match any known graphic type
    CGraphicsUnit unit;
    unit.head.dataType = 9999;

    int beforeCount = ctx->scene()->items().size();

    ctx->addSceneItem(unit, false, false, false);

    int afterCount = ctx->scene()->items().size();
    EXPECT_EQ(afterCount, beforeCount);
}

TEST(PageContextAddSceneItemTest, AddSceneItemWithRecordFlagNoCrash)
{
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    PageContext *ctx = getPageContext();
    ASSERT_NE(ctx, nullptr);

    QImage img(16, 16, QImage::Format_ARGB32);
    img.fill(Qt::red);
    ctx->addImage(img, QPointF(10, 10), QRectF(0, 0, 16, 16), true, false);

    qApp->processEvents();
    SUCCEED();
}
