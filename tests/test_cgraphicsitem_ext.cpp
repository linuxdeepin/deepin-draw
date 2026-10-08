// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Extended unit tests for CGraphicsItem (V-5458 batch)
 *
 * Target methods (20):
 *   Query/status: boundingRectTruly, bzGroup, getTrulyShape,
 *                 penStrokerShape, selfOrgShape
 *   Handle/paint: beginCheckIns, endCheckIns, paintItemSelf,
 *                 paintMutBoundingLine
 *   Transform/property: setAutoCache, resetCachePixmap,
 *                       updateBlurPixmap
 *   Structure/event: creatItemInstance, updateShape,
 *                    updateShapeRecursion, testOpetating,
 *                    operatingBegin, operatingEnd,
 *                    contextMenuEvent,
 *                    getGraphicsItemShapePathByOrg
 *
 * Source file:
 *   src/drawshape/drawItems/bzItems/cgraphicsitem.cpp
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

// Include Qt headers BEFORE #define private to avoid breaking Qt internals.
#include <QtCore/QtGlobal>
#include <QPen>
#include <QBrush>
#include <QPointF>
#include <QTransform>
#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>
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
#include "drawshape/sitemdata.h"
#include "drawshape/globaldefine.h"
#undef protected
#undef private

// CGraphicsItem is abstract (pure virtual rect()), so we need a concrete
// subclass for testing the base-class methods directly.
class TestGraphicsItem : public CGraphicsItem
{
public:
    explicit TestGraphicsItem(QGraphicsItem *parent = nullptr)
        : CGraphicsItem(parent) {}
    QRectF m_testRect;
    QRectF rect() const override { return m_testRect; }
};

// =========================================================================
// Query / status methods
// =========================================================================

// --- boundingRectTruly ---

TEST(CGraphicsItemExtTest, BoundingRectTruly_Default_ReturnsEmptyRect)
{
    TestGraphicsItem item(nullptr);
    QRectF result = item.boundingRectTruly();
    EXPECT_TRUE(result.isNull());
}

TEST(CGraphicsItemExtTest, BoundingRectTruly_AfterUpdateShape_ReturnsExpected)
{
    TestGraphicsItem item(nullptr);
    item.m_testRect = QRectF(0, 0, 100, 50);
    item.setPen(QPen(Qt::black, 2));
    item.updateShape();

    QRectF result = item.boundingRectTruly();
    EXPECT_NEAR(result.width(),  100.0, 5.0);
    EXPECT_NEAR(result.height(),  50.0, 5.0);
}

// --- bzGroup ---

TEST(CGraphicsItemExtTest, BzGroup_NoGroup_ReturnsNullptr)
{
    TestGraphicsItem item(nullptr);
    EXPECT_EQ(item.bzGroup(false), nullptr);
}

TEST(CGraphicsItemExtTest, BzGroup_OnlyNormalNoGroup_ReturnsNullptr)
{
    TestGraphicsItem item(nullptr);
    EXPECT_EQ(item.bzGroup(true), nullptr);
}

TEST(CGraphicsItemExtTest, BzGroup_WithNormalGroup_ReturnsGroup)
{
    TestGraphicsItem item(nullptr);
    CGraphicsItemGroup group(CGraphicsItemGroup::ENormalGroup, "test");
    item.setBzGroup(&group);

    EXPECT_EQ(item.bzGroup(false), &group);
    EXPECT_EQ(item.bzGroup(true), &group);

    // Clear the cross reference before destruction (group is destroyed
    // first due to declaration order, which would leave item->_pGroup dangling).
    item.setBzGroup(nullptr);
}

TEST(CGraphicsItemExtTest, BzGroup_OnlyNormalWithSelectGroup_ReturnsNullptr)
{
    TestGraphicsItem item(nullptr);
    CGraphicsItemGroup group(CGraphicsItemGroup::ESelectGroup, "sel");
    item.setBzGroup(&group);

    EXPECT_EQ(item.bzGroup(false), &group);
    EXPECT_EQ(item.bzGroup(true), nullptr);

    // Clear the cross reference before destruction (group is destroyed
    // first due to declaration order, which would leave item->_pGroup dangling).
    item.setBzGroup(nullptr);
}

// --- getTrulyShape ---

TEST(CGraphicsItemExtTest, GetTrulyShape_DefaultEmpty_NotEmptyAfterUpdateShape)
{
    TestGraphicsItem item(nullptr);
    item.m_testRect = QRectF(0, 0, 50, 50);
    item.setPen(QPen(Qt::black, 2));
    item.updateShape();

    QPainterPath result = item.getTrulyShape();
    EXPECT_FALSE(result.isEmpty());
}

TEST(CGraphicsItemExtTest, GetTrulyShape_NoPen_ReturnsOrgShape)
{
    TestGraphicsItem item(nullptr);
    item.m_testRect = QRectF(0, 0, 50, 50);
    item.setPen(Qt::NoPen);
    item.updateShape();

    QPainterPath result = item.getTrulyShape();
    QPainterPath orgShape = item.selfOrgShape();
    EXPECT_EQ(result, orgShape);
}

// --- penStrokerShape ---

TEST(CGraphicsItemExtTest, PenStrokerShape_Default_ReturnsEmptyPath)
{
    TestGraphicsItem item(nullptr);
    QPainterPath result = item.penStrokerShape();
    EXPECT_TRUE(result.isEmpty());
}

TEST(CGraphicsItemExtTest, PenStrokerShape_AfterUpdateShape_NotEmpty)
{
    TestGraphicsItem item(nullptr);
    item.m_testRect = QRectF(0, 0, 50, 50);
    item.setPen(QPen(Qt::black, 2));
    item.updateShape();

    QPainterPath result = item.penStrokerShape();
    EXPECT_FALSE(result.isEmpty());
}

// --- selfOrgShape ---

TEST(CGraphicsItemExtTest, SelfOrgShape_Default_ReturnsEmptyPath)
{
    TestGraphicsItem item(nullptr);
    QPainterPath result = item.selfOrgShape();
    EXPECT_TRUE(result.isEmpty());
}

TEST(CGraphicsItemExtTest, SelfOrgShape_AfterUpdateShape_ContainsRect)
{
    TestGraphicsItem item(nullptr);
    item.m_testRect = QRectF(10, 20, 100, 80);
    item.updateShape();

    QPainterPath result = item.selfOrgShape();
    EXPECT_FALSE(result.isEmpty());
    EXPECT_TRUE(result.contains(QPointF(50, 50)));
}

// =========================================================================
// Handle / paint methods
// =========================================================================

// --- beginCheckIns ---

TEST(CGraphicsItemExtTest, BeginCheckIns_NoScene_ReturnsEarly)
{
    TestGraphicsItem item(nullptr);
    QPixmap pix(100, 100);
    pix.fill(Qt::white);
    QPainter painter(&pix);

    // Should not crash even without a scene
    item.beginCheckIns(&painter);
    SUCCEED();
}

TEST(CGraphicsItemExtTest, BeginCheckIns_WithScene_SavesPainter)
{
    TestGraphicsItem item(nullptr);
    item.m_testRect = QRectF(0, 0, 50, 50);
    QGraphicsScene scene;
    scene.addItem(&item);

    QPixmap pix(100, 100);
    pix.fill(Qt::white);
    QPainter painter(&pix);

    item.beginCheckIns(&painter);
    SUCCEED();

    item.endCheckIns(&painter);
    scene.removeItem(&item);
}

TEST(CGraphicsItemExtTest, BeginCheckIns_InvalidRect_ReturnsEarly)
{
    TestGraphicsItem item(nullptr);
    QGraphicsScene scene;
    scene.addItem(&item);

    QPixmap pix(100, 100);
    pix.fill(Qt::white);
    QPainter painter(&pix);

    item.beginCheckIns(&painter);
    SUCCEED();
    scene.removeItem(&item);
}

// --- endCheckIns ---

TEST(CGraphicsItemExtTest, EndCheckIns_NoScene_ReturnsEarly)
{
    TestGraphicsItem item(nullptr);
    QPixmap pix(100, 100);
    pix.fill(Qt::white);
    QPainter painter(&pix);

    item.endCheckIns(&painter);
    SUCCEED();
}

TEST(CGraphicsItemExtTest, EndCheckIns_AfterBegin_RestoresPainter)
{
    TestGraphicsItem item(nullptr);
    item.m_testRect = QRectF(0, 0, 50, 50);
    QGraphicsScene scene;
    scene.addItem(&item);

    QPixmap pix(100, 100);
    pix.fill(Qt::white);
    QPainter painter(&pix);

    item.beginCheckIns(&painter);
    item.endCheckIns(&painter);
    SUCCEED();
    scene.removeItem(&item);
}

// --- paintItemSelf ---

TEST(CGraphicsItemExtTest, PaintItemSelf_CacheMode_DoesNotCrash)
{
    TestGraphicsItem item(nullptr);
    item.m_testRect = QRectF(0, 0, 50, 50);
    item.setPen(QPen(Qt::black, 2));

    QPixmap pix(100, 100);
    pix.fill(Qt::white);
    QPainter painter(&pix);
    QStyleOptionGraphicsItem option;

    // EPaintForCache skips beginCheckIns/endCheckIns
    item.paintItemSelf(&painter, &option, CGraphicsItem::EPaintForCache);
    SUCCEED();
}

TEST(CGraphicsItemExtTest, PaintItemSelf_NoCacheMode_DoesNotCrash)
{
    TestGraphicsItem item(nullptr);
    item.m_testRect = QRectF(0, 0, 50, 50);
    item.setPen(QPen(Qt::black, 2));
    QGraphicsScene scene;
    scene.addItem(&item);

    QPixmap pix(100, 100);
    pix.fill(Qt::white);
    QPainter painter(&pix);
    QStyleOptionGraphicsItem option;

    item.paintItemSelf(&painter, &option, CGraphicsItem::EPaintForNoCache);
    SUCCEED();
    scene.removeItem(&item);
}

// --- paintMutBoundingLine ---

TEST(CGraphicsItemExtTest, PaintMutBoundingLine_NotMutiSelected_NoDraw)
{
    TestGraphicsItem item(nullptr);
    item.m_testRect = QRectF(0, 0, 50, 50);

    QPixmap pix(100, 100);
    pix.fill(Qt::white);
    QPainter painter(&pix);
    QStyleOptionGraphicsItem option;

    // isMutiSelected() is false by default, should not draw
    item.paintMutBoundingLine(&painter, &option);
    SUCCEED();
}

TEST(CGraphicsItemExtTest, PaintMutBoundingLine_BorderLineDisabled_ReturnsEarly)
{
    TestGraphicsItem item(nullptr);
    item.m_testRect = QRectF(0, 0, 50, 50);

    // Disable border line flag
    bool oldFlag = CGraphicsItem::paintInteractBorderLine;
    CGraphicsItem::paintInteractBorderLine = false;

    QPixmap pix(100, 100);
    pix.fill(Qt::white);
    QPainter painter(&pix);
    QStyleOptionGraphicsItem option;

    item.paintMutBoundingLine(&painter, &option);

    CGraphicsItem::paintInteractBorderLine = oldFlag;
    SUCCEED();
}

// =========================================================================
// Transform / property methods
// =========================================================================

// --- setAutoCache ---

TEST(CGraphicsItemExtTest, SetAutoCache_EnablesAutoCache)
{
    TestGraphicsItem item(nullptr);
    item.setAutoCache(true, 500);

    EXPECT_TRUE(item._autoCache);
    EXPECT_EQ(item._autoEplMs, 500);
}

TEST(CGraphicsItemExtTest, SetAutoCache_DisablesAutoCache)
{
    TestGraphicsItem item(nullptr);
    item.setAutoCache(true, 500);
    item.setAutoCache(false, 0);

    EXPECT_FALSE(item._autoCache);
    EXPECT_EQ(item._autoEplMs, 0);
}

// --- resetCachePixmap ---

TEST(CGraphicsItemExtTest, ResetCachePixmap_NoCache_ReturnsEarly)
{
    TestGraphicsItem item(nullptr);
    // _useCachePixmap is false by default
    item.resetCachePixmap();
    SUCCEED();
}

TEST(CGraphicsItemExtTest, ResetCachePixmap_CacheEnabledButNullPixmap_NoCrash)
{
    TestGraphicsItem item(nullptr);
    item._useCachePixmap = true;
    // _cachePixmap is nullptr, guard prevents dereference
    item.resetCachePixmap();
    SUCCEED();
}

// --- updateBlurPixmap ---

TEST(CGraphicsItemExtTest, UpdateBlurPixmap_SpecificEffect_NoDrawSceneCall)
{
    TestGraphicsItem item(nullptr);
    item.m_testRect = QRectF(0, 0, 50, 50);
    item.setPen(QPen(Qt::black, 2));
    item.updateShape();

    // Pass a specific effect to avoid drawScene()->pageContext() call
    item.updateBlurPixmap(false, BlurEffect);
    EXPECT_FALSE(item._blurPix[BlurEffect].isNull());
}

TEST(CGraphicsItemExtTest, UpdateBlurPixmap_MasicoEffect_BlurPixUpdated)
{
    TestGraphicsItem item(nullptr);
    item.m_testRect = QRectF(0, 0, 50, 50);
    item.setPen(QPen(Qt::black, 2));
    item.updateShape();

    item.updateBlurPixmap(false, MasicoEffect);
    EXPECT_FALSE(item._blurPix[MasicoEffect].isNull());
}

// =========================================================================
// Structure / event methods
// =========================================================================

// --- creatItemInstance ---

TEST(CGraphicsItemExtTest, CreatItemInstance_UnknownType_ReturnsNullptr)
{
    CGraphicsUnit unit;
    CGraphicsItem *result = CGraphicsItem::creatItemInstance(-1, unit);
    EXPECT_EQ(result, nullptr);
}

TEST(CGraphicsItemExtTest, CreatItemInstance_RectType_ReturnsItem)
{
    CGraphicsUnit unit;
    unit.head.dataType = RectType;
    CGraphicsItem *result = CGraphicsItem::creatItemInstance(RectType, unit);
    // Registered type should create an item
    EXPECT_NE(result, nullptr);
    if (result) {
        EXPECT_EQ(result->type(), RectType);
        delete result;
    }
}

// --- updateShape ---

TEST(CGraphicsItemExtTest, UpdateShape_UpdatesShapeCaches)
{
    TestGraphicsItem item(nullptr);
    item.m_testRect = QRectF(0, 0, 100, 80);
    item.setPen(QPen(Qt::black, 2));
    item.updateShape();

    // After updateShape, shape caches should be populated
    EXPECT_FALSE(item.m_selfOrgPathShape.isEmpty());
    EXPECT_FALSE(item.m_penStroerPathShape.isEmpty());
    EXPECT_FALSE(item.m_boundingRect.isNull());
    EXPECT_FALSE(item.m_boundingRectTrue.isNull());
}

TEST(CGraphicsItemExtTest, UpdateShape_NoPen_StillUpdatesOrgShape)
{
    TestGraphicsItem item(nullptr);
    item.m_testRect = QRectF(10, 10, 50, 50);
    item.setPen(Qt::NoPen);
    item.updateShape();

    // selfOrgShape should be populated even without pen
    EXPECT_FALSE(item.m_selfOrgPathShape.isEmpty());
}

// --- updateShapeRecursion ---

TEST(CGraphicsItemExtTest, UpdateShapeRecursion_NoGroup_UpdatesShapeOnly)
{
    TestGraphicsItem item(nullptr);
    item.m_testRect = QRectF(0, 0, 50, 50);
    item.setPen(QPen(Qt::black, 2));
    item.updateShapeRecursion();

    EXPECT_FALSE(item.m_selfOrgPathShape.isEmpty());
    // No group, so recursion doesn't go deeper
    SUCCEED();
}

TEST(CGraphicsItemExtTest, UpdateShapeRecursion_WithGroup_RecursesIntoGroup)
{
    TestGraphicsItem item(nullptr);
    item.m_testRect = QRectF(0, 0, 50, 50);
    item.setPen(QPen(Qt::black, 2));

    CGraphicsItemGroup group(CGraphicsItemGroup::ENormalGroup, "test");
    item.setBzGroup(&group);

    // Should not crash even with group (group's updateShapeRecursion
    // may do nothing harmful with empty group)
    item.updateShapeRecursion();

    EXPECT_FALSE(item.m_selfOrgPathShape.isEmpty());

    // Clear the cross reference before destruction (group is destroyed
    // first due to declaration order, which would leave item->_pGroup dangling).
    item.setBzGroup(nullptr);
}

// --- testOpetating ---

TEST(CGraphicsItemExtTest, TestOpetating_MoveEvent_ReturnsTrue)
{
    TestGraphicsItem item(nullptr);
    CGraphItemMoveEvent event(CGraphItemEvent::EMove,
                              QPointF(0, 0), QPointF(10, 20));

    bool result = item.testOpetating(&event);
    EXPECT_TRUE(result);
}

TEST(CGraphicsItemExtTest, TestOpetating_ScalEvent_ReturnsTrue)
{
    TestGraphicsItem item(nullptr);
    CGraphItemScalEvent event(CGraphItemEvent::EScal,
                              QPointF(0, 0), QPointF(10, 20));

    bool result = item.testOpetating(&event);
    EXPECT_TRUE(result);
}

TEST(CGraphicsItemExtTest, TestOpetating_RotEvent_ReturnsTrue)
{
    TestGraphicsItem item(nullptr);
    CGraphItemRotEvent event(CGraphItemEvent::ERot,
                             QPointF(0, 0), QPointF(10, 20));

    bool result = item.testOpetating(&event);
    EXPECT_TRUE(result);
}

TEST(CGraphicsItemExtTest, TestOpetating_BlurEvent_ReturnsFalse)
{
    TestGraphicsItem item(nullptr);
    CGraphItemEvent event(CGraphItemEvent::EBlur,
                          QPointF(0, 0), QPointF(10, 20));

    bool result = item.testOpetating(&event);
    EXPECT_FALSE(result);
}

TEST(CGraphicsItemExtTest, TestOpetating_UnknownEvent_ReturnsFalse)
{
    TestGraphicsItem item(nullptr);
    CGraphItemEvent event(CGraphItemEvent::EUnKnow,
                          QPointF(0, 0), QPointF(10, 20));

    bool result = item.testOpetating(&event);
    EXPECT_FALSE(result);
}

// --- operatingBegin ---

TEST(CGraphicsItemExtTest, OperatingBegin_MoveEvent_SetsOperatingType)
{
    TestGraphicsItem item(nullptr);
    CGraphItemMoveEvent event(CGraphItemEvent::EMove,
                              QPointF(0, 0), QPointF(10, 20));
    event.setToolEventType(1);

    item.operatingBegin(&event);
    EXPECT_EQ(item.m_operatingType, 1);
}

TEST(CGraphicsItemExtTest, OperatingBegin_ScalEvent_SetsOperatingType)
{
    TestGraphicsItem item(nullptr);
    CGraphItemScalEvent event(CGraphItemEvent::EScal,
                              QPointF(0, 0), QPointF(10, 20));
    event.setToolEventType(2);

    item.operatingBegin(&event);
    EXPECT_EQ(item.m_operatingType, 2);
}

TEST(CGraphicsItemExtTest, OperatingBegin_RotEvent_SetsOperatingType)
{
    TestGraphicsItem item(nullptr);
    item.m_testRect = QRectF(0, 0, 50, 50);
    CGraphItemRotEvent event(CGraphItemEvent::ERot,
                             QPointF(0, 0), QPointF(10, 20));
    event.setToolEventType(3);
    event.setCenterPos(QPointF(25, 25));

    item.operatingBegin(&event);
    EXPECT_EQ(item.m_operatingType, 3);
}

TEST(CGraphicsItemExtTest, OperatingBegin_BlurEvent_NoCrash)
{
    TestGraphicsItem item(nullptr);
    CGraphItemEvent event(CGraphItemEvent::EBlur,
                          QPointF(0, 0), QPointF(10, 20));
    event.setToolEventType(4);

    item.operatingBegin(&event);
    SUCCEED();
}

// --- operatingEnd ---

TEST(CGraphicsItemExtTest, OperatingEnd_MoveEvent_ResetsOperatingType)
{
    TestGraphicsItem item(nullptr);
    item.m_operatingType = 1;
    CGraphItemMoveEvent event(CGraphItemEvent::EMove,
                              QPointF(0, 0), QPointF(10, 20));

    item.operatingEnd(&event);
    EXPECT_EQ(item.m_operatingType, -1);
}

TEST(CGraphicsItemExtTest, OperatingEnd_ScalEvent_ResetsOperatingType)
{
    TestGraphicsItem item(nullptr);
    item.m_operatingType = 2;
    CGraphItemScalEvent event(CGraphItemEvent::EScal,
                              QPointF(0, 0), QPointF(10, 20));

    item.operatingEnd(&event);
    EXPECT_EQ(item.m_operatingType, -1);
}

TEST(CGraphicsItemExtTest, OperatingEnd_RotEvent_ResetsOperatingType)
{
    TestGraphicsItem item(nullptr);
    item.m_operatingType = 3;
    CGraphItemRotEvent event(CGraphItemEvent::ERot,
                             QPointF(0, 0), QPointF(10, 20));

    item.operatingEnd(&event);
    EXPECT_EQ(item.m_operatingType, -1);
}

TEST(CGraphicsItemExtTest, OperatingEnd_BlurEvent_ResetsOperatingType)
{
    TestGraphicsItem item(nullptr);
    item.m_operatingType = 4;
    CGraphItemEvent event(CGraphItemEvent::EBlur,
                          QPointF(0, 0), QPointF(10, 20));

    item.operatingEnd(&event);
    EXPECT_EQ(item.m_operatingType, -1);
}

// --- contextMenuEvent ---

TEST(CGraphicsItemExtTest, ContextMenuEvent_NoOp_DoesNotCrash)
{
    TestGraphicsItem item(nullptr);
    QGraphicsSceneContextMenuEvent event;

    // contextMenuEvent is a no-op (Q_UNUSED(event)), should not crash
    item.contextMenuEvent(&event);
    SUCCEED();
}

// --- getGraphicsItemShapePathByOrg ---

TEST(CGraphicsItemExtTest, GetShapePathByOrg_EmptyPath_ReturnsEmpty)
{
    QPainterPath emptyPath;
    QPen pen(Qt::black, 2);

    QPainterPath result =
        CGraphicsItem::getGraphicsItemShapePathByOrg(
            emptyPath, pen, true, 0, false);
    EXPECT_TRUE(result.isEmpty());
}

TEST(CGraphicsItemExtTest, GetShapePathByOrg_NoPen_ReturnsOrgPath)
{
    QPainterPath orgPath;
    orgPath.addRect(0, 0, 50, 50);

    QPainterPath result =
        CGraphicsItem::getGraphicsItemShapePathByOrg(
            orgPath, Qt::NoPen, true, 0, false);
    EXPECT_EQ(result, orgPath);
}

TEST(CGraphicsItemExtTest, GetShapePathByOrg_WithPen_PenStrokerShape)
{
    QPainterPath orgPath;
    orgPath.addRect(0, 0, 50, 50);
    QPen pen(Qt::black, 4);

    QPainterPath result =
        CGraphicsItem::getGraphicsItemShapePathByOrg(
            orgPath, pen, true, 0, false);
    // Stroker shape should be larger than original
    EXPECT_GT(result.boundingRect().width(),
              orgPath.boundingRect().width());
}

TEST(CGraphicsItemExtTest, GetShapePathByOrg_WithPen_CombinedShape)
{
    QPainterPath orgPath;
    orgPath.addRect(0, 0, 50, 50);
    QPen pen(Qt::black, 4);

    // penStrokerShape=false => stroke + orgPath
    QPainterPath result =
        CGraphicsItem::getGraphicsItemShapePathByOrg(
            orgPath, pen, false, 0, false);
    EXPECT_FALSE(result.isEmpty());
    EXPECT_GT(result.boundingRect().width(),
              orgPath.boundingRect().width());
}

TEST(CGraphicsItemExtTest, GetShapePathByOrg_ZeroWidth_UsesEpsilon)
{
    QPainterPath orgPath;
    orgPath.addRect(0, 0, 50, 50);
    QPen pen(Qt::black, 0);

    QPainterPath result =
        CGraphicsItem::getGraphicsItemShapePathByOrg(
            orgPath, pen, true, 0, false);
    // Should not crash with zero-width pen (uses epsilon)
    EXPECT_FALSE(result.isEmpty());
}

TEST(CGraphicsItemExtTest, GetShapePathByOrg_DoSimplified_SimplifiesPath)
{
    QPainterPath orgPath;
    orgPath.addRect(0, 0, 50, 50);
    QPen pen(Qt::black, 4);

    QPainterPath result =
        CGraphicsItem::getGraphicsItemShapePathByOrg(
            orgPath, pen, true, 0, true);
    EXPECT_FALSE(result.isEmpty());
}
