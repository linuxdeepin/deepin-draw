// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CGraphicsItem transform / query methods (V-5932)
 *
 * Target methods (23):
 *   Transform lifecycle: doMoving, testMoving, doScalBegin,
 *                        doRotBegin, doRoting, testRoting
 *   Bounding / handle: selectedBoundingRect, handleNodes,
 *                      handleNode, isSizeHandleExisted
 *   Z-value / layer: drawZValue, setLayer
 *   Group queries: bzTopGroup, isBzGroup, thisBzProxyItem
 *   Flip: isFilped, getFilpTransform
 *   Cache / viewport: getCachePixmap, updateViewport,
 *                      changeTransCenterTo
 *   Blur: isBlurActived, updateBlurPixmapBySelfBlurInfo
 *   Registration: registerItem
 *
 * Source file:
 *   src/drawshape/drawItems/bzItems/cgraphicsitem.cpp
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QtCore/QtGlobal>
#include <QPen>
#include <QBrush>
#include <QPointF>
#include <QTransform>
#include <QGraphicsScene>
#include <QColor>
#include <QVariant>
#include <QPainterPath>
#include <QPainter>
#include <QPixmap>
#include <QStyleOptionGraphicsItem>

#define protected public
#define private public
#include "drawshape/drawItems/bzItems/cgraphicsitem.h"
#include "drawshape/drawItems/cgraphicsitemselectedmgr.h"
#include "drawshape/drawItems/bzItems/cgraphicsitemevent.h"
#include "drawshape/drawItems/csizehandlerect.h"
#include "drawshape/drawItems/cgraphicslayer.h"
#include "drawshape/sitemdata.h"
#include "drawshape/globaldefine.h"
#undef protected
#undef private

#include "testgraphicsitem.h"

// =========================================================================
// Transform lifecycle methods
// =========================================================================

// --- doMoving ---

TEST(CGraphicsItemTransformTest, DoMoving_PosChangedByDelta)
{
    TestGraphicsItem item(nullptr);
    item.setPos(QPointF(0, 0));
    CGraphItemMoveEvent event(CGraphItemEvent::EMove,
                              QPointF(0, 0), QPointF(10, 20));
    event._scenePos = QPointF(10, 20);
    event._oldScenePos = QPointF(0, 0);

    item.doMoving(&event);
    EXPECT_NEAR(item.pos().x(), 10.0, 0.01);
    EXPECT_NEAR(item.pos().y(), 20.0, 0.01);
}

TEST(CGraphicsItemTransformTest, DoMoving_ZeroDelta_PosUnchanged)
{
    TestGraphicsItem item(nullptr);
    item.setPos(QPointF(5, 5));
    CGraphItemMoveEvent event(CGraphItemEvent::EMove,
                              QPointF(0, 0), QPointF(0, 0));
    event._scenePos = QPointF(0, 0);
    event._oldScenePos = QPointF(0, 0);

    item.doMoving(&event);
    EXPECT_NEAR(item.pos().x(), 5.0, 0.01);
    EXPECT_NEAR(item.pos().y(), 5.0, 0.01);
}

// --- testMoving ---

TEST(CGraphicsItemTransformTest, TestMoving_AlwaysReturnsTrue)
{
    TestGraphicsItem item(nullptr);
    CGraphItemMoveEvent event(CGraphItemEvent::EMove,
                              QPointF(0, 0), QPointF(1, 1));
    EXPECT_TRUE(item.testMoving(&event));
}

// --- doScalBegin ---

TEST(CGraphicsItemTransformTest, DoScalBegin_NoCrash)
{
    TestGraphicsItem item(nullptr);
    item.m_testRect = QRectF(0, 0, 50, 50);
    CGraphItemScalEvent event(CGraphItemEvent::EScal,
                              QPointF(0, 0), QPointF(10, 10));
    item.doScalBegin(&event);
    SUCCEED();
}

// --- doRotBegin ---

TEST(CGraphicsItemTransformTest, DoRotBegin_NoCrash)
{
    TestGraphicsItem item(nullptr);
    item.m_testRect = QRectF(0, 0, 50, 50);
    CGraphItemRotEvent event(CGraphItemEvent::ERot,
                             QPointF(0, 0), QPointF(10, 10));
    item.doRotBegin(&event);
    SUCCEED();
}

// --- doRoting ---

TEST(CGraphicsItemTransformTest, DoRoting_UpdatesRotationAngle)
{
    TestGraphicsItem item(nullptr);
    item.m_testRect = QRectF(0, 0, 100, 100);
    item.setTransformOriginPoint(QPointF(50, 50));
    // oldPos at 0 degrees (straight up from center), pos at 90 degrees
    CGraphItemRotEvent event(CGraphItemEvent::ERot,
                             QPointF(50, 0), QPointF(100, 50));
    event.setCenterPos(QPointF(50, 50));

    qreal before = item._roteAgnel;
    item.doRoting(&event);
    EXPECT_NE(item._roteAgnel, before);
    EXPECT_GE(item._roteAgnel, 0.0);
    EXPECT_LT(item._roteAgnel, 360.0);
}

TEST(CGraphicsItemTransformTest, DoRoting_ZeroRotation_NoChange)
{
    TestGraphicsItem item(nullptr);
    item.m_testRect = QRectF(0, 0, 100, 100);
    item.setTransformOriginPoint(QPointF(50, 50));
    qreal before = item._roteAgnel;
    // Same old and new pos → zero angle change
    CGraphItemRotEvent event(CGraphItemEvent::ERot,
                             QPointF(60, 50), QPointF(60, 50));
    event.setCenterPos(QPointF(50, 50));

    item.doRoting(&event);
    EXPECT_NEAR(item._roteAgnel, before, 0.01);
}

// --- testRoting ---

TEST(CGraphicsItemTransformTest, TestRoting_AlwaysReturnsTrue)
{
    TestGraphicsItem item(nullptr);
    CGraphItemRotEvent event(CGraphItemEvent::ERot,
                             QPointF(0, 0), QPointF(1, 1));
    EXPECT_TRUE(item.testRoting(&event));
}

// =========================================================================
// Bounding / handle methods
// =========================================================================

// --- selectedBoundingRect ---

TEST(CGraphicsItemTransformTest, SelectedBoundingRect_EqualsRect)
{
    TestGraphicsItem item(nullptr);
    item.m_testRect = QRectF(10, 20, 100, 50);
    EXPECT_EQ(item.selectedBoundingRect(), item.rect());
}

TEST(CGraphicsItemTransformTest, SelectedBoundingRect_EmptyRect)
{
    TestGraphicsItem item(nullptr);
    EXPECT_TRUE(item.selectedBoundingRect().isNull());
}

// --- handleNodes ---

TEST(CGraphicsItemTransformTest, HandleNodes_DefaultEmpty)
{
    TestGraphicsItem item(nullptr);
    EXPECT_TRUE(item.handleNodes().isEmpty());
}

TEST(CGraphicsItemTransformTest, HandleNodes_AfterManualAdd_ReturnsHandles)
{
    TestGraphicsItem item(nullptr);
    auto *handle = new CSizeHandleRect(&item, CSizeHandleRect::Left);
    item.m_handles.append(handle);
    EXPECT_EQ(item.handleNodes().size(), 1);
    EXPECT_EQ(item.handleNodes().first(), handle);
}

// --- handleNode(direction) ---

TEST(CGraphicsItemTransformTest, HandleNode_FindsMatchingDirection)
{
    TestGraphicsItem item(nullptr);
    auto *handle = new CSizeHandleRect(&item, CSizeHandleRect::Right);
    item.m_handles.append(handle);
    auto *found = item.handleNode(CSizeHandleRect::Right);
    EXPECT_EQ(found, handle);
}

TEST(CGraphicsItemTransformTest, HandleNode_NoMatch_ReturnsNullptr)
{
    TestGraphicsItem item(nullptr);
    auto *handle = new CSizeHandleRect(&item, CSizeHandleRect::Left);
    item.m_handles.append(handle);
    auto *found = item.handleNode(CSizeHandleRect::Right);
    EXPECT_EQ(found, nullptr);
}

TEST(CGraphicsItemTransformTest, HandleNode_EmptyHandles_ReturnsNullptr)
{
    TestGraphicsItem item(nullptr);
    EXPECT_EQ(item.handleNode(CSizeHandleRect::Left), nullptr);
}

// --- isSizeHandleExisted ---

TEST(CGraphicsItemTransformTest, IsSizeHandleExisted_NoHandles_ReturnsFalse)
{
    TestGraphicsItem item(nullptr);
    EXPECT_FALSE(item.isSizeHandleExisted());
}

TEST(CGraphicsItemTransformTest, IsSizeHandleExisted_WithHandles_ReturnsTrue)
{
    TestGraphicsItem item(nullptr);
    auto *handle = new CSizeHandleRect(&item, CSizeHandleRect::Left);
    item.m_handles.append(handle);
    EXPECT_TRUE(item.isSizeHandleExisted());
}

// =========================================================================
// Z-value / layer methods
// =========================================================================

// --- drawZValue ---

TEST(CGraphicsItemTransformTest, DrawZValue_EqualsZValue)
{
    TestGraphicsItem item(nullptr);
    item.setZValue(3.5);
    EXPECT_DOUBLE_EQ(item.drawZValue(), item.zValue());
}

TEST(CGraphicsItemTransformTest, DrawZValue_DefaultZero)
{
    TestGraphicsItem item(nullptr);
    EXPECT_DOUBLE_EQ(item.drawZValue(), 0.0);
}

// --- setLayer ---

TEST(CGraphicsItemTransformTest, SetLayer_UpdatesLayerPointer)
{
    TestGraphicsItem item(nullptr);
    CGraphicsLayer layer;
    item.setLayer(&layer);
    EXPECT_EQ(item.layer(), &layer);
}

TEST(CGraphicsItemTransformTest, SetLayer_SwitchFromOldToNew)
{
    TestGraphicsItem item(nullptr);
    CGraphicsLayer oldLayer;
    CGraphicsLayer newLayer;
    item.setLayer(&oldLayer);
    EXPECT_EQ(item.layer(), &oldLayer);

    item.setLayer(&newLayer);
    EXPECT_EQ(item.layer(), &newLayer);
}

TEST(CGraphicsItemTransformTest, SetLayer_NullLayer_ClearsReference)
{
    TestGraphicsItem item(nullptr);
    CGraphicsLayer layer;
    item.setLayer(&layer);
    item.setLayer(nullptr);
    EXPECT_EQ(item.layer(), nullptr);
}

TEST(CGraphicsItemTransformTest, SetLayer_SameLayer_NoChange)
{
    TestGraphicsItem item(nullptr);
    CGraphicsLayer layer;
    item.setLayer(&layer);
    item.setLayer(&layer);
    EXPECT_EQ(item.layer(), &layer);
}

// =========================================================================
// Group query methods
// =========================================================================

// --- isBzGroup ---

TEST(CGraphicsItemTransformTest, IsBzGroup_NonGroupItem_ReturnsFalse)
{
    TestGraphicsItem item(nullptr);
    EXPECT_FALSE(item.isBzGroup());
}

TEST(CGraphicsItemTransformTest, IsBzGroup_GroupItem_ReturnsTrue)
{
    CGraphicsItemGroup group(CGraphicsItemGroup::ENormalGroup, "grp");
    int groupTp = -1;
    EXPECT_TRUE(group.isBzGroup(&groupTp));
    EXPECT_EQ(groupTp, CGraphicsItemGroup::ENormalGroup);
}

TEST(CGraphicsItemTransformTest, IsBzGroup_NoOutParam_Works)
{
    CGraphicsItemGroup group(CGraphicsItemGroup::ESelectGroup, "sel");
    EXPECT_TRUE(group.isBzGroup());
}

// --- bzTopGroup ---

TEST(CGraphicsItemTransformTest, BzTopGroup_NotInGroup_ReturnsNullptr)
{
    TestGraphicsItem item(nullptr);
    EXPECT_EQ(item.bzTopGroup(), nullptr);
}

TEST(CGraphicsItemTransformTest, BzTopGroup_InGroup_ReturnsTopGroup)
{
    CGraphicsItemGroup topGroup(CGraphicsItemGroup::ENormalGroup, "top");
    TestGraphicsItem item(nullptr);
    topGroup.add(&item, false, false);
    EXPECT_EQ(item.bzTopGroup(), &topGroup);
}

TEST(CGraphicsItemTransformTest, BzTopGroup_OnlyNormal_SkipsSelectGroup)
{
    CGraphicsItemGroup selectGroup(CGraphicsItemGroup::ESelectGroup, "sel");
    TestGraphicsItem item(nullptr);
    selectGroup.add(&item, false, false);
    // onlyNormal=true skips ESelectGroup, so should return nullptr
    EXPECT_EQ(item.bzTopGroup(true), nullptr);
}

// --- thisBzProxyItem ---

TEST(CGraphicsItemTransformTest, ThisBzProxyItem_NotInGroup_ReturnsSelf)
{
    TestGraphicsItem item(nullptr);
    EXPECT_EQ(item.thisBzProxyItem(), &item);
}

TEST(CGraphicsItemTransformTest, ThisBzProxyItem_InGroup_ReturnsGroup)
{
    CGraphicsItemGroup group(CGraphicsItemGroup::ENormalGroup, "grp");
    TestGraphicsItem item(nullptr);
    group.add(&item, false, false);
    EXPECT_EQ(item.thisBzProxyItem(false), &group);
}

TEST(CGraphicsItemTransformTest, ThisBzProxyItem_TopLevel_ReturnsTopGroup)
{
    CGraphicsItemGroup topGroup(CGraphicsItemGroup::ENormalGroup, "top");
    CGraphicsItemGroup subGroup(CGraphicsItemGroup::ENormalGroup, "sub");
    topGroup.add(&subGroup, false, false);
    TestGraphicsItem item(nullptr);
    subGroup.add(&item, false, false);
    EXPECT_EQ(item.thisBzProxyItem(true), &topGroup);
}

// =========================================================================
// Flip methods
// =========================================================================

// --- isFilped ---

TEST(CGraphicsItemTransformTest, IsFilped_DefaultHorizontal_False)
{
    TestGraphicsItem item(nullptr);
    EXPECT_FALSE(item.isFilped(CGraphicsItem::EFilpHor));
}

TEST(CGraphicsItemTransformTest, IsFilped_DefaultVertical_False)
{
    TestGraphicsItem item(nullptr);
    EXPECT_FALSE(item.isFilped(CGraphicsItem::EFilpVer));
}

TEST(CGraphicsItemTransformTest, IsFilped_AfterHorizontalFlip_True)
{
    TestGraphicsItem item(nullptr);
    item.doFilp(CGraphicsItem::EFilpHor);
    EXPECT_TRUE(item.isFilped(CGraphicsItem::EFilpHor));
}

TEST(CGraphicsItemTransformTest, IsFilped_AfterVerticalFlip_True)
{
    TestGraphicsItem item(nullptr);
    item.doFilp(CGraphicsItem::EFilpVer);
    EXPECT_TRUE(item.isFilped(CGraphicsItem::EFilpVer));
}

// --- getFilpTransform ---

TEST(CGraphicsItemTransformTest, GetFilpTransform_NoFlip_Identity)
{
    TestGraphicsItem item(nullptr);
    item.m_testRect = QRectF(0, 0, 100, 100);
    QTransform t = item.getFilpTransform();
    // Without flip, the transform should be identity (scale 1,1)
    EXPECT_NEAR(t.m11(), 1.0, 0.01);
    EXPECT_NEAR(t.m22(), 1.0, 0.01);
}

TEST(CGraphicsItemTransformTest, GetFilpTransform_HorizontalFlip_NegativeM11)
{
    TestGraphicsItem item(nullptr);
    item.m_testRect = QRectF(0, 0, 100, 100);
    item.doFilp(CGraphicsItem::EFilpHor);
    QTransform t = item.getFilpTransform();
    EXPECT_NEAR(t.m11(), -1.0, 0.01);
}

TEST(CGraphicsItemTransformTest, GetFilpTransform_VerticalFlip_NegativeM22)
{
    TestGraphicsItem item(nullptr);
    item.m_testRect = QRectF(0, 0, 100, 100);
    item.doFilp(CGraphicsItem::EFilpVer);
    QTransform t = item.getFilpTransform();
    EXPECT_NEAR(t.m22(), -1.0, 0.01);
}

// =========================================================================
// Cache / viewport methods
// =========================================================================

// --- changeTransCenterTo ---

TEST(CGraphicsItemTransformTest, ChangeTransCenterTo_UpdatesOriginPoint)
{
    TestGraphicsItem item(nullptr);
    item.m_testRect = QRectF(0, 0, 100, 100);
    item.changeTransCenterTo(QPointF(50, 50));
    EXPECT_EQ(item.transformOriginPoint(), QPointF(50, 50));
}

TEST(CGraphicsItemTransformTest, ChangeTransCenterTo_DefaultOrigin)
{
    TestGraphicsItem item(nullptr);
    EXPECT_EQ(item.transformOriginPoint(), QPointF(0, 0));
}

// --- updateViewport ---

TEST(CGraphicsItemTransformTest, UpdateViewport_NoView_NoCrash)
{
    TestGraphicsItem item(nullptr);
    item.updateViewport();
    SUCCEED();
}

// --- getCachePixmap ---

TEST(CGraphicsItemTransformTest, GetCachePixmap_NoCrash)
{
    TestGraphicsItem item(nullptr);
    item.m_testRect = QRectF(0, 0, 50, 50);
    QPixmap pix = item.getCachePixmap(true);
    // TestGraphicsItem never calls updateShape(), so boundingRectTruly()
    // is null and the resulting cache pixmap is null.
    EXPECT_TRUE(pix.isNull());
}

// =========================================================================
// Blur methods
// =========================================================================

// --- isBlurActived ---

TEST(CGraphicsItemTransformTest, IsBlurActived_Default_False)
{
    TestGraphicsItem item(nullptr);
    EXPECT_FALSE(item.isBlurActived());
}

TEST(CGraphicsItemTransformTest, IsBlurActived_WithBlurPath_True)
{
    TestGraphicsItem item(nullptr);
    QPainterPath blurPath;
    blurPath.addRect(QRectF(0, 0, 10, 10));
    item._tempActiveBlurPath = blurPath;
    EXPECT_TRUE(item.isBlurActived());
}

// --- updateBlurPixmapBySelfBlurInfo ---

TEST(CGraphicsItemTransformTest, UpdateBlurPixmapBySelfBlurInfo_NoBlurInfos_NoCrash)
{
    TestGraphicsItem item(nullptr);
    item.m_testRect = QRectF(0, 0, 50, 50);
    item.updateBlurPixmapBySelfBlurInfo();
    SUCCEED();
}

// =========================================================================
// Registration method
// =========================================================================

// --- registerItem ---

TEST(CGraphicsItemTransformTest, RegisterItem_StoresClassName_NoCrash)
{
    TestGraphicsItem item(nullptr);
    item.registerItem("TestItemClass", 9999);
    SUCCEED();
}

TEST(CGraphicsItemTransformTest, RegisterItem_OverwriteExisting_NoCrash)
{
    TestGraphicsItem item(nullptr);
    item.registerItem("FirstClass", 8888);
    item.registerItem("SecondClass", 8888);
    SUCCEED();
}
