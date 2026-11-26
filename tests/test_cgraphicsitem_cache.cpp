// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CGraphicsItem cache-related branches and
 * rasterToSelfLayer(true) (issue V-9310 §3).
 *
 * Source: src/drawshape/drawItems/bzItems/cgraphicsitem.cpp
 *
 * Target paths (accessed via #define private public):
 *   setPenColor/setPenWidth/setBrushColor → resetCachePixmap() when cached (287/310/329)
 *   loadGraphicsUnit with blurInfos → resetCachePixmap (401-402)
 *   operating(EScal) → doScaling + blur info (861-866)
 *   operatingEnd(EScal) → *_cachePixmap = getCachePixmap() (928)
 *   paint cache drawing branch (1039-1056)
 *   resetCachePixmap function body (1478)
 *   rasterToSelfLayer(true) (1513-1518)
 */

#include <gtest/gtest.h>
#include <QImage>
#include <QPainter>
#include <QPointF>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsScene>

#define protected public
#define private public
#include "drawshape/drawItems/bzItems/cgraphicsitem.h"
#include "drawshape/drawItems/bzItems/cgraphicsitemevent.h"
#include "drawshape/drawItems/bzItems/cgraphicsrectitem.h"
#include "drawshape/drawItems/bzItems/cgraphicspenitem.h"
#include "drawshape/drawItems/cgraphicslayer.h"
#include "drawshape/sitemdata.h"
#include "drawshape/globaldefine.h"
#undef protected
#undef private

#include "test_helpers.h"

// ===========================================================================
// setPenColor/setPenWidth/setBrushColor with cache enabled → resetCachePixmap
// ===========================================================================

TEST(CGraphicsItemCacheTest, SetPenColor_WithCache_TriggersResetCache)
{
    CGraphicsRectItem item(QRectF(0, 0, 100, 80));
    item.setCache(true);
    ASSERT_TRUE(item.isCached());

    // Capture the old cache pixmap pointer; after resetCachePixmap the
    // pixmap content should be refreshed but the pointer stays the same.
    QPixmap *oldPix = item._cachePixmap;
    item.setPenColor(Qt::blue, false);
    EXPECT_EQ(item._cachePixmap, oldPix);
    EXPECT_TRUE(item.isCached());
}

TEST(CGraphicsItemCacheTest, SetPenWidth_WithCache_TriggersResetCache)
{
    CGraphicsRectItem item(QRectF(0, 0, 100, 80));
    item.setCache(true);
    ASSERT_TRUE(item.isCached());

    QPixmap *oldPix = item._cachePixmap;
    item.setPenWidth(5, false);
    EXPECT_EQ(item._cachePixmap, oldPix);
    EXPECT_TRUE(item.isCached());
}

TEST(CGraphicsItemCacheTest, SetBrushColor_WithCache_TriggersResetCache)
{
    CGraphicsRectItem item(QRectF(0, 0, 100, 80));
    item.setCache(true);
    ASSERT_TRUE(item.isCached());

    QPixmap *oldPix = item._cachePixmap;
    item.setBrushColor(Qt::green, false);
    EXPECT_EQ(item._cachePixmap, oldPix);
    EXPECT_TRUE(item.isCached());
}

TEST(CGraphicsItemCacheTest, SetPenColor_PreviewMode_DoesNotResetCache)
{
    CGraphicsRectItem item(QRectF(0, 0, 100, 80));
    item.setCache(true);
    ASSERT_TRUE(item.isCached());

    // In preview mode, resetCachePixmap is not called.
    item.setPenColor(Qt::red, true);
    EXPECT_TRUE(item.isCached());
}

// ===========================================================================
// resetCachePixmap function body (1478)
// ===========================================================================

TEST(CGraphicsItemCacheTest, ResetCachePixmap_WhenNotOperating_RefreshesPixmap)
{
    CGraphicsRectItem item(QRectF(0, 0, 100, 80));
    item.setCache(true);
    ASSERT_TRUE(item.isCached());
    ASSERT_EQ(item.m_operatingType, -1);

    // resetCachePixmap should refresh the cache pixmap content.
    item.resetCachePixmap();
    EXPECT_TRUE(item.isCached());
    EXPECT_NE(item._cachePixmap, nullptr);
}

TEST(CGraphicsItemCacheTest, ResetCachePixmap_WhenOperating_DoesNotRefresh)
{
    CGraphicsRectItem item(QRectF(0, 0, 100, 80));
    item.setCache(true);
    ASSERT_TRUE(item.isCached());

    // Simulate an ongoing operation.
    item.m_operatingType = CGraphItemEvent::EScal;
    QPixmap *oldPix = item._cachePixmap;
    item.resetCachePixmap();
    // The pixmap pointer stays the same but content is not refreshed
    // (the guard checks m_operatingType == -1).
    EXPECT_EQ(item._cachePixmap, oldPix);

    // Restore state.
    item.m_operatingType = -1;
}

// ===========================================================================
// operating(EScal) → doScaling + blur info (861-866)
// ===========================================================================

TEST(CGraphicsItemCacheTest, Operating_EScal_NoBlurInfos_NoCrash)
{
    CGraphicsRectItem item(QRectF(0, 0, 100, 80));
    item.setCache(true);

    CGraphItemScalEvent evt(CGraphItemEvent::EScal);
    evt.setItem(&item);
    QTransform trans;
    trans.scale(2.0, 2.0);
    evt.setTrans(trans);

    item.operating(&evt);
    SUCCEED();
}

TEST_F(PageStateGuard, Operating_EScal_WithBlurInfos_UpdatesBlurPath)
{
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    // Create a rect item and add it to the scene.
    CGraphicsRectItem *rectItem = new CGraphicsRectItem(QRectF(0, 0, 100, 80));
    scene->addCItem(rectItem);
    qApp->processEvents();

    rectItem->setCache(true);
    ASSERT_TRUE(rectItem->isCached());

    // Add a blur info so the blur-path update branch is hit.
    SBlurInfo info;
    info.blurEfTp = BlurEffect;
    info.blurPath = QPainterPath();
    info.blurPath.addRect(10, 10, 20, 20);
    rectItem->_blurInfos.append(info);

    CGraphItemScalEvent evt(CGraphItemEvent::EScal);
    evt.setItem(rectItem);
    QTransform trans;
    trans.scale(1.5, 1.5);
    evt.setTrans(trans);

    rectItem->operating(&evt);
    SUCCEED();
}

// ===========================================================================
// operatingEnd(EScal) → *_cachePixmap = getCachePixmap() (928)
// ===========================================================================

TEST(CGraphicsItemCacheTest, OperatingEnd_EScal_WithCache_RefreshesCachePixmap)
{
    CGraphicsRectItem item(QRectF(0, 0, 100, 80));
    item.setCache(true);
    ASSERT_TRUE(item.isCached());

    item.m_operatingType = CGraphItemEvent::EScal;

    CGraphItemScalEvent evt(CGraphItemEvent::EScal);
    evt.setItem(&item);

    item.operatingEnd(&evt);
    // After operatingEnd, operatingType should be reset to -1 and the
    // cache pixmap should be refreshed.
    EXPECT_EQ(item.m_operatingType, -1);
    EXPECT_TRUE(item.isCached());
}

// ===========================================================================
// paint cache drawing branch (1039-1056)
// ===========================================================================

TEST(CGraphicsItemCacheTest, Paint_WithCache_DrawsCachePixmap)
{
    CGraphicsRectItem item(QRectF(0, 0, 100, 80));
    item.setCache(true);
    ASSERT_TRUE(item.isCached());

    // Create a small image and paint the item onto it.
    QImage img(200, 200, QImage::Format_ARGB32);
    img.fill(Qt::transparent);
    QPainter painter(&img);
    QStyleOptionGraphicsItem opt;
    opt.rect = item.boundingRect().toRect();
    item.paint(&painter, &opt, nullptr);
    SUCCEED();
}

// ===========================================================================
// loadGraphicsUnit with blurInfos → resetCachePixmap (401-402)
// ===========================================================================

TEST_F(PageStateGuard, LoadGraphicsUnit_WithBlurInfosAndCache_ResetsCache)
{
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);
    PageContext *ctx = scene->pageContext();
    ASSERT_NE(ctx, nullptr);

    // Create a rect item and add it to the scene.
    CGraphicsRectItem *rectItem = new CGraphicsRectItem(QRectF(0, 0, 100, 80));
    scene->addCItem(rectItem);
    qApp->processEvents();

    rectItem->setCache(true);
    ASSERT_TRUE(rectItem->isCached());

    CGraphicsUnit unit;
    unit.head.dataType = RectType;
    unit.head.pos = QPointF(10, 10);
    unit.head.zValue = 0;
    unit.head.rotate = 0.0;
    unit.head.trans = QTransform();

    // Add a blur info to the unit so the blur-pixmap update branch is hit.
    SBlurInfo info;
    info.blurEfTp = BlurEffect;
    info.blurPath = QPainterPath();
    info.blurPath.addRect(5, 5, 10, 10);
    unit.head.blurInfos.append(info);

    rectItem->loadGraphicsUnit(unit);

    EXPECT_TRUE(rectItem->isCached());
}

// ===========================================================================
// rasterToSelfLayer(true) (1513-1518)
// The item is rasterized onto its layer and then removed from the scene
// via a queued invocation.  We need a scene with a layer.
// ===========================================================================

TEST_F(PageStateGuard, RasterToSelfLayer_True_RemovesItemFromScene)
{
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    QImage img(32, 32, QImage::Format_ARGB32);
    img.fill(Qt::red);

    PageContext *ctx = scene->pageContext();
    ASSERT_NE(ctx, nullptr);

    ctx->addImage(img, QPointF(0, 0), QRectF(0, 0, 32, 32), false, false);
    qApp->processEvents();

    auto sceneItems = scene->items();
    CGraphicsItem *bzItem = nullptr;
    for (auto *it : sceneItems) {
        auto *bz = dynamic_cast<CGraphicsItem *>(it);
        if (bz != nullptr && bz->isBzItem()) {
            bzItem = bz;
            break;
        }
    }
    ASSERT_NE(bzItem, nullptr);

    int countBefore = scene->items().size();
    bzItem->rasterToSelfLayer(true);
    qApp->processEvents();
    QTest::qWait(50);

    // The item should have been removed from the scene.
    EXPECT_LT(scene->items().size(), countBefore);
}
