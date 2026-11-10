// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CGraphicsItem miscellaneous methods (issue V-8176)
 *
 * Source: src/drawshape/drawItems/bzItems/cgraphicsitem.cpp
 *
 * Target methods (base CGraphicsItem, 0-coverage per existing .gcda):
 *   hitTest(const QPointF&) const
 *   setCache(bool) / setFilpBaseOrg(EFilpDirect, bool)
 *   doScaling(CGraphItemScalEvent*)
 *   getGraphicsUnit(EDataReason) const / loadGraphicsUnit(const CGraphicsUnit&)
 *   blurBegin(const QPointF&) / blurUpdate(const QPointF&, bool) / blurEnd()
 *   rasterToSelfLayer(bool deleteSelf)
 *   mousePressEvent / mouseMoveEvent / mouseReleaseEvent / mouseDoubleClickEvent
 *
 * CGraphicsItem is abstract (pure virtual rect()), so the concrete
 * CGraphicsRectItem and TestGraphicsItem subclass are used.
 */

#include <gtest/gtest.h>

#include <QImage>
#include <QPainter>
#include <QPointF>
#include <QPainterPath>
#include <QGraphicsSceneMouseEvent>

#define protected public
#define private public
#include "drawshape/drawItems/bzItems/cgraphicsitem.h"
#include "drawshape/drawItems/bzItems/cgraphicsitemevent.h"
#include "drawshape/drawItems/bzItems/cgraphicsrectitem.h"
#include "drawshape/drawItems/cgraphicslayer.h"
#include "drawshape/sitemdata.h"
#include "drawshape/globaldefine.h"
#undef protected
#undef private

#include "testgraphicsitem.h"

// =========================================================================
// hitTest
// =========================================================================

TEST(CGraphicsItemMisc2Test, HitTest_PointInsideRect_ReturnsNoneWithoutScene)
{
    CGraphicsRectItem item(QRectF(10, 10, 100, 80));
    // Point inside the item. Without a scene, hitTest returns None (no handles registered)
    CSizeHandleRect::EDirection dir = item.hitTest(QPointF(50, 50));
    EXPECT_EQ(dir, CSizeHandleRect::None);
}

TEST(CGraphicsItemMisc2Test, HitTest_PointOutside_ReturnsNone)
{
    CGraphicsRectItem item(QRectF(10, 10, 100, 80));
    CSizeHandleRect::EDirection dir = item.hitTest(QPointF(500, 500));
    EXPECT_EQ(dir, CSizeHandleRect::None);
}

TEST(CGraphicsItemMisc2Test, HitTest_PointAtCorner_ReturnsNoneWithoutScene)
{
    CGraphicsRectItem item(QRectF(0, 0, 50, 50));
    // Corner point. Without a scene, hitTest returns None
    CSizeHandleRect::EDirection dir = item.hitTest(QPointF(1, 1));
    EXPECT_EQ(dir, CSizeHandleRect::None);
}

// =========================================================================
// setCache
// =========================================================================

TEST(CGraphicsItemMisc2Test, SetCache_True_EnablesCache)
{
    CGraphicsRectItem item(QRectF(0, 0, 50, 50));
    EXPECT_FALSE(item.isCached());
    item.setCache(true);
    // After enabling cache, isCached should return true (cache pixmap created)
    EXPECT_TRUE(item.isCached());
}

TEST(CGraphicsItemMisc2Test, SetCache_FalseThenTrue_CycleWorks)
{
    CGraphicsRectItem item(QRectF(0, 0, 50, 50));
    item.setCache(true);
    EXPECT_TRUE(item.isCached());
    item.setCache(false);
    EXPECT_FALSE(item.isCached());
    item.setCache(true);
    EXPECT_TRUE(item.isCached());
}

TEST(CGraphicsItemMisc2Test, SetCache_FalseWhenNotCached_NoCrash)
{
    CGraphicsRectItem item(QRectF(0, 0, 50, 50));
    // Cache was never enabled; disabling should be a safe no-op
    item.setCache(false);
    EXPECT_FALSE(item.isCached());
}

// =========================================================================
// setFilpBaseOrg
// =========================================================================

TEST(CGraphicsItemMisc2Test, SetFilpBaseOrg_Horizontal_TogglesFlip)
{
    CGraphicsRectItem item(QRectF(0, 0, 50, 50));
    EXPECT_FALSE(item.isFilped(CGraphicsItem::EFilpHor));
    item.setFilpBaseOrg(CGraphicsItem::EFilpHor, true);
    EXPECT_TRUE(item.isFilped(CGraphicsItem::EFilpHor));
    item.setFilpBaseOrg(CGraphicsItem::EFilpHor, false);
    EXPECT_FALSE(item.isFilped(CGraphicsItem::EFilpHor));
}

TEST(CGraphicsItemMisc2Test, SetFilpBaseOrg_Vertical_TogglesFlip)
{
    CGraphicsRectItem item(QRectF(0, 0, 50, 50));
    EXPECT_FALSE(item.isFilped(CGraphicsItem::EFilpVer));
    item.setFilpBaseOrg(CGraphicsItem::EFilpVer, true);
    EXPECT_TRUE(item.isFilped(CGraphicsItem::EFilpVer));
    item.setFilpBaseOrg(CGraphicsItem::EFilpVer, false);
    EXPECT_FALSE(item.isFilped(CGraphicsItem::EFilpVer));
}

TEST(CGraphicsItemMisc2Test, SetFilpBaseOrg_SameValue_NoOp)
{
    CGraphicsRectItem item(QRectF(0, 0, 50, 50));
    // Setting to current value should not crash or change state
    item.setFilpBaseOrg(CGraphicsItem::EFilpHor, false);
    EXPECT_FALSE(item.isFilped(CGraphicsItem::EFilpHor));
}

// =========================================================================
// doScaling (base class: no-op)
// =========================================================================

TEST(CGraphicsItemMisc2Test, DoScaling_BaseClass_NoCrash)
{
    TestGraphicsItem item;
    item.m_testRect = QRectF(0, 0, 100, 50);

    CGraphItemScalEvent event;
    event.setPos(QPointF(50, 25));
    event.setEventPhase(EChangedBegin);

    item.doScaling(&event);
    // Base class implementation is a no-op; verify no crash
    SUCCEED();
}

TEST(CGraphicsItemMisc2Test, DoScaling_NullEvent_NoCrash)
{
    TestGraphicsItem item;
    item.m_testRect = QRectF(0, 0, 100, 50);
    item.doScaling(nullptr);
    SUCCEED();
}

// =========================================================================
// getGraphicsUnit (base class: returns empty CGraphicsUnit)
// =========================================================================

TEST(CGraphicsItemMisc2Test, GetGraphicsUnit_BaseClass_ReturnsEmptyUnit)
{
    TestGraphicsItem item;
    item.m_testRect = QRectF(0, 0, 100, 50);

    CGraphicsUnit unit = item.getGraphicsUnit(EDuplicate);
    // Base class returns a default-constructed CGraphicsUnit
    EXPECT_EQ(unit.head.dataType, 0); // Base class returns default (0)
}

TEST(CGraphicsItemMisc2Test, GetGraphicsUnit_DifferentReasons_NoCrash)
{
    TestGraphicsItem item;
    item.m_testRect = QRectF(0, 0, 100, 50);

    item.getGraphicsUnit(EUndoRedo);
    item.getGraphicsUnit(ESaveToDDf);
    item.getGraphicsUnit(ENormal);
    SUCCEED();
}

// =========================================================================
// loadGraphicsUnit (base class: calls loadHeadData)
// =========================================================================

TEST(CGraphicsItemMisc2Test, LoadGraphicsUnit_BaseClass_NoCrash)
{
    TestGraphicsItem item;
    item.m_testRect = QRectF(0, 0, 100, 50);

    CGraphicsUnit unit = item.getGraphicsUnit(EDuplicate);
    item.loadGraphicsUnit(unit);
    // Should not crash; base class calls loadHeadData
    SUCCEED();
}

TEST(CGraphicsItemMisc2Test, LoadGraphicsUnit_EmptyUnit_NoCrash)
{
    TestGraphicsItem item;
    item.m_testRect = QRectF(0, 0, 100, 50);

    CGraphicsUnit unit;
    item.loadGraphicsUnit(unit);
    SUCCEED();
}

// =========================================================================
// blurBegin / blurUpdate / blurEnd (all commented-out no-ops in base)
// =========================================================================

TEST(CGraphicsItemMisc2Test, BlurBegin_NoCrash)
{
    CGraphicsRectItem item(QRectF(0, 0, 100, 80));
    item.blurBegin(QPointF(50, 40));
    // Implementation is fully commented out — safe no-op
    SUCCEED();
}

TEST(CGraphicsItemMisc2Test, BlurUpdate_NoCrash)
{
    CGraphicsRectItem item(QRectF(0, 0, 100, 80));
    item.blurBegin(QPointF(50, 40));
    item.blurUpdate(QPointF(60, 50), false);
    item.blurUpdate(QPointF(70, 60), true);
    SUCCEED();
}

TEST(CGraphicsItemMisc2Test, BlurEnd_NoCrash)
{
    CGraphicsRectItem item(QRectF(0, 0, 100, 80));
    item.blurBegin(QPointF(50, 40));
    item.blurUpdate(QPointF(60, 50), false);
    item.blurEnd();
    SUCCEED();
}

TEST(CGraphicsItemMisc2Test, BlurEnd_WithoutBegin_NoCrash)
{
    CGraphicsRectItem item(QRectF(0, 0, 100, 80));
    item.blurEnd();
    SUCCEED();
}

// =========================================================================
// mousePressEvent / mouseMoveEvent / mouseReleaseEvent / mouseDoubleClickEvent
// All set event->setAccepted(false) in base class.
// =========================================================================

TEST(CGraphicsItemMisc2Test, MousePressEvent_SetsUnaccepted)
{
    CGraphicsRectItem item(QRectF(0, 0, 100, 80));

    QGraphicsSceneMouseEvent event(QEvent::GraphicsSceneMousePress);
    event.setAccepted(true); // start as accepted
    item.mousePressEvent(&event);
    EXPECT_FALSE(event.isAccepted());
}

TEST(CGraphicsItemMisc2Test, MouseMoveEvent_SetsUnaccepted)
{
    CGraphicsRectItem item(QRectF(0, 0, 100, 80));

    QGraphicsSceneMouseEvent event(QEvent::GraphicsSceneMouseMove);
    event.setAccepted(true);
    item.mouseMoveEvent(&event);
    EXPECT_FALSE(event.isAccepted());
}

TEST(CGraphicsItemMisc2Test, MouseReleaseEvent_SetsUnaccepted)
{
    CGraphicsRectItem item(QRectF(0, 0, 100, 80));

    QGraphicsSceneMouseEvent event(QEvent::GraphicsSceneMouseRelease);
    event.setAccepted(true);
    item.mouseReleaseEvent(&event);
    EXPECT_FALSE(event.isAccepted());
}

TEST(CGraphicsItemMisc2Test, MouseDoubleClickEvent_SetsUnaccepted)
{
    CGraphicsRectItem item(QRectF(0, 0, 100, 80));

    QGraphicsSceneMouseEvent event(QEvent::GraphicsSceneMouseDoubleClick);
    event.setAccepted(true);
    item.mouseDoubleClickEvent(&event);
    EXPECT_FALSE(event.isAccepted());
}

// =========================================================================
// rasterToSelfLayer — requires a valid layer with an image.
// We create a CGraphicsLayer, set its rect (which allocates the image),
// set it as the item's layer, then call rasterToSelfLayer(false).
// =========================================================================

TEST(CGraphicsItemMisc2Test, RasterToSelfLayer_WithValidLayer_NoCrash)
{
    CGraphicsRectItem item(QRectF(0, 0, 50, 50));

    CGraphicsLayer layer;
    layer.setRect(QRectF(0, 0, 100, 100));
    item.setLayer(&layer);

    // deleteSelf=false to avoid scene removal (item has no scene)
    item.rasterToSelfLayer(false);
    SUCCEED();
}

TEST(CGraphicsItemMisc2Test, RasterToSelfLayer_DeleteSelfFalse_NoRemoval)
{
    CGraphicsRectItem item(QRectF(10, 10, 40, 40));

    CGraphicsLayer layer;
    layer.setRect(QRectF(0, 0, 100, 100));
    item.setLayer(&layer);

    // With deleteSelf=false, the item should still be valid afterwards
    item.rasterToSelfLayer(false);
    EXPECT_FALSE(item.rect().isNull());
}
