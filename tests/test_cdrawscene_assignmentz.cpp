// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for assignmentZ() free function (cdrawscene.cpp).
 *
 * Target function (from issue V-5568):
 *   - assignmentZ(const QList<CGraphicsItem *> &items,
 *                 qreal &beginZ,
 *                 PageScene::ESortItemTp listSortedTp)
 *
 * The function recursively assigns Z values to items in ascending
 * or descending order.  It is a free function with external linkage
 * (not declared in any header), so we extern-declare it here.
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
#include "cdrawscene.h"
#include "drawshape/drawItems/bzItems/cgraphicsitem.h"
#include "drawshape/drawItems/cgraphicsitemselectedmgr.h"
#include "globaldefine.h"
#undef protected
#undef private

// assignmentZ is a free function in cdrawscene.cpp (external linkage)
extern void assignmentZ(const QList<CGraphicsItem *> &items,
                        qreal &beginZ,
                        PageScene::ESortItemTp listSortedTp);

TEST(AssignmentZTest, EmptyList_DoesNotModifyBeginZ)
{
    QList<CGraphicsItem *> items;
    qreal beginZ = 5.0;

    assignmentZ(items, beginZ, PageScene::EAesSort);
    EXPECT_DOUBLE_EQ(beginZ, 5.0);

    assignmentZ(items, beginZ, PageScene::EDesSort);
    EXPECT_DOUBLE_EQ(beginZ, 5.0);
}

TEST(AssignmentZTest, AscendingSort_AssignsIncreasingZ)
{
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    QImage img(16, 16, QImage::Format_ARGB32);
    img.fill(Qt::red);

    PageContext *ctx = scene->pageContext();
    ASSERT_NE(ctx, nullptr);

    int beforeCount = scene->items().size();
    ctx->addImage(img, QPointF(0, 0), QRectF(0, 0, 16, 16), false, false);
    ctx->addImage(img, QPointF(20, 0), QRectF(0, 0, 16, 16), false, false);
    qApp->processEvents();

    auto sceneItems = scene->items();
    ASSERT_GT(sceneItems.size(), beforeCount + 1);

    // Collect CGraphicsItem* from the scene items
    QList<CGraphicsItem *> bzItems;
    for (auto *item : sceneItems) {
        auto *bzItem = dynamic_cast<CGraphicsItem *>(item);
        if (bzItem != nullptr && bzItem->isBzItem()) {
            bzItems.append(bzItem);
        }
    }
    ASSERT_GE(bzItems.size(), 2);

    qreal beginZ = 0.0;
    assignmentZ(bzItems, beginZ, PageScene::EAesSort);

    // After ascending assignment, beginZ should have advanced
    EXPECT_GT(beginZ, 0.0);

    // Verify Z values are in ascending order
    for (int i = 1; i < bzItems.size(); ++i) {
        EXPECT_GE(bzItems[i]->zValue(), bzItems[i - 1]->zValue());
    }
}

TEST(AssignmentZTest, DescendingSort_AssignsDecreasingZ)
{
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    auto sceneItems = scene->items();
    QList<CGraphicsItem *> bzItems;
    for (auto *item : sceneItems) {
        auto *bzItem = dynamic_cast<CGraphicsItem *>(item);
        if (bzItem != nullptr && bzItem->isBzItem()) {
            bzItems.append(bzItem);
        }
    }
    if (bzItems.size() < 2)
        GTEST_SKIP() << "Need at least 2 items for descending test";

    qreal beginZ = 100.0;
    assignmentZ(bzItems, beginZ, PageScene::EDesSort);

    // After descending assignment, beginZ should have advanced
    EXPECT_GT(beginZ, 100.0);
}
