// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CGraphItemEvent and CGraphItemScalEvent (issue V-5645).
 *
 * Source: src/drawshape/drawItems/bzItems/cgraphicsitemevent.cpp
 *
 * Target methods:
 *   CGraphItemEvent: setDriverEvent, setTrans, transAllPosTo, creatTransDuplicate
 *   CGraphItemScalEvent: setYTransBlocked, setXNegtiveOffset,
 *                        setYNegtiveOffset, setMayResultPolygon
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QTransform>
#include <QPointF>
#include <QSizeF>
#include <QPolygonF>

#define protected public
#define private public
#include "drawshape/drawItems/bzItems/cgraphicsitemevent.h"
#undef protected
#undef private

// =========================================================================
// CGraphItemEvent::setDriverEvent
// =========================================================================

TEST(CGraphItemEventSetDriverEvent, SetValidEvent_StoresPointer)
{
    CGraphItemEvent event(CGraphItemEvent::EMove);
    CGraphItemEvent driver(CGraphItemEvent::EScal);

    event.setDriverEvent(&driver);

    EXPECT_EQ(event._driverEvent, &driver);
}

TEST(CGraphItemEventSetDriverEvent, SetNullEvent_StoresNull)
{
    CGraphItemEvent event(CGraphItemEvent::EMove);

    event.setDriverEvent(nullptr);

    EXPECT_EQ(event._driverEvent, nullptr);
}

TEST(CGraphItemEventSetDriverEvent, OverwritePreviousDriver)
{
    CGraphItemEvent event(CGraphItemEvent::EMove);
    CGraphItemEvent driver1(CGraphItemEvent::EScal);
    CGraphItemEvent driver2(CGraphItemEvent::ERot);

    event.setDriverEvent(&driver1);
    EXPECT_EQ(event._driverEvent, &driver1);

    event.setDriverEvent(&driver2);
    EXPECT_EQ(event._driverEvent, &driver2);
}

// =========================================================================
// CGraphItemEvent::setTrans
// =========================================================================

TEST(CGraphItemEventSetTrans, SetTrans_ClearsTransDirty)
{
    CGraphItemEvent event(CGraphItemEvent::EMove);
    EXPECT_TRUE(event._transDirty);

    QTransform trans;
    trans.translate(10.0, 20.0);
    event.setTrans(trans);

    EXPECT_FALSE(event._transDirty);
    EXPECT_TRUE(qFuzzyCompare(event._trans.dx(), trans.dx()));
    EXPECT_TRUE(qFuzzyCompare(event._trans.dy(), trans.dy()));
}

TEST(CGraphItemEventSetTrans, SetIdentityTrans)
{
    CGraphItemEvent event(CGraphItemEvent::EMove);

    event.setTrans(QTransform());

    EXPECT_FALSE(event._transDirty);
    EXPECT_TRUE(event._trans.isIdentity());
}

TEST(CGraphItemEventSetTrans, OverwriteTrans_UpdatesValue)
{
    CGraphItemEvent event(CGraphItemEvent::EMove);

    QTransform t1;
    t1.translate(5.0, 5.0);
    event.setTrans(t1);

    QTransform t2;
    t2.scale(2.0, 3.0);
    event.setTrans(t2);

    EXPECT_FALSE(event._transDirty);
    EXPECT_DOUBLE_EQ(event._trans.m11(), 2.0);
    EXPECT_DOUBLE_EQ(event._trans.m22(), 3.0);
}

// =========================================================================
// CGraphItemEvent::transAllPosTo
// =========================================================================

TEST(CGraphItemEventTransAllPosTo, TranslateTransform_MapsAllPositions)
{
    CGraphItemEvent event(CGraphItemEvent::EMove);
    event.setPos(QPointF(10.0, 20.0));
    event.setOldPos(QPointF(5.0, 10.0));
    event.setBeginPos(QPointF(0.0, 0.0));
    event.setCenterPos(QPointF(15.0, 25.0));

    QTransform tran;
    tran.translate(100.0, 200.0);
    event.transAllPosTo(tran);

    EXPECT_DOUBLE_EQ(event.pos().x(), 110.0);
    EXPECT_DOUBLE_EQ(event.pos().y(), 220.0);
    EXPECT_DOUBLE_EQ(event.oldPos().x(), 105.0);
    EXPECT_DOUBLE_EQ(event.oldPos().y(), 210.0);
    EXPECT_DOUBLE_EQ(event.beginPos().x(), 100.0);
    EXPECT_DOUBLE_EQ(event.beginPos().y(), 200.0);
    EXPECT_DOUBLE_EQ(event.centerPos().x(), 115.0);
    EXPECT_DOUBLE_EQ(event.centerPos().y(), 225.0);
}

TEST(CGraphItemEventTransAllPosTo, IdentityTransform_PositionsUnchanged)
{
    CGraphItemEvent event(CGraphItemEvent::EMove);
    event.setPos(QPointF(10.0, 20.0));
    event.setOldPos(QPointF(5.0, 10.0));
    event.setBeginPos(QPointF(1.0, 2.0));
    event.setCenterPos(QPointF(15.0, 25.0));

    event.transAllPosTo(QTransform());

    EXPECT_DOUBLE_EQ(event.pos().x(), 10.0);
    EXPECT_DOUBLE_EQ(event.pos().y(), 20.0);
    EXPECT_DOUBLE_EQ(event.oldPos().x(), 5.0);
    EXPECT_DOUBLE_EQ(event.oldPos().y(), 10.0);
    EXPECT_DOUBLE_EQ(event.beginPos().x(), 1.0);
    EXPECT_DOUBLE_EQ(event.beginPos().y(), 2.0);
    EXPECT_DOUBLE_EQ(event.centerPos().x(), 15.0);
    EXPECT_DOUBLE_EQ(event.centerPos().y(), 25.0);
}

TEST(CGraphItemEventTransAllPosTo, ScaleTransform_MapsPositions)
{
    CGraphItemEvent event(CGraphItemEvent::EMove);
    event.setPos(QPointF(10.0, 20.0));
    event.setOldPos(QPointF(5.0, 10.0));
    event.setBeginPos(QPointF(0.0, 0.0));
    event.setCenterPos(QPointF(15.0, 25.0));

    QTransform tran;
    tran.scale(2.0, 0.5);
    event.transAllPosTo(tran);

    EXPECT_DOUBLE_EQ(event.pos().x(), 20.0);
    EXPECT_DOUBLE_EQ(event.pos().y(), 10.0);
    EXPECT_DOUBLE_EQ(event.oldPos().x(), 10.0);
    EXPECT_DOUBLE_EQ(event.oldPos().y(), 5.0);
    EXPECT_DOUBLE_EQ(event.beginPos().x(), 0.0);
    EXPECT_DOUBLE_EQ(event.beginPos().y(), 0.0);
    EXPECT_DOUBLE_EQ(event.centerPos().x(), 30.0);
    EXPECT_DOUBLE_EQ(event.centerPos().y(), 12.5);
}

// =========================================================================
// CGraphItemEvent::creatTransDuplicate
// =========================================================================

TEST(CGraphItemEventCreatTransDuplicate, ReturnsNonNullInstance)
{
    CGraphItemEvent event(CGraphItemEvent::EMove);

    QTransform tran;
    tran.translate(10.0, 20.0);
    QSizeF newSize(100.0, 200.0);

    CGraphItemEvent *dup = event.creatTransDuplicate(tran, newSize);

    ASSERT_NE(dup, nullptr);
    EXPECT_EQ(dup->type(), CGraphItemEvent::EMove);
    EXPECT_DOUBLE_EQ(dup->orgSize().width(), 100.0);
    EXPECT_DOUBLE_EQ(dup->orgSize().height(), 200.0);

    delete dup;
}

TEST(CGraphItemEventCreatTransDuplicate, DuplicatePositionsAreMapped)
{
    CGraphItemEvent event(CGraphItemEvent::EMove);
    event.setPos(QPointF(10.0, 20.0));
    event.setOldPos(QPointF(5.0, 10.0));
    event.setBeginPos(QPointF(0.0, 0.0));
    event.setCenterPos(QPointF(15.0, 25.0));

    QTransform tran;
    tran.translate(100.0, 200.0);
    QSizeF newSize(50.0, 60.0);

    CGraphItemEvent *dup = event.creatTransDuplicate(tran, newSize);

    ASSERT_NE(dup, nullptr);
    EXPECT_DOUBLE_EQ(dup->pos().x(), 110.0);
    EXPECT_DOUBLE_EQ(dup->pos().y(), 220.0);
    EXPECT_DOUBLE_EQ(dup->oldPos().x(), 105.0);
    EXPECT_DOUBLE_EQ(dup->oldPos().y(), 210.0);
    EXPECT_DOUBLE_EQ(dup->beginPos().x(), 100.0);
    EXPECT_DOUBLE_EQ(dup->beginPos().y(), 200.0);
    EXPECT_DOUBLE_EQ(dup->centerPos().x(), 115.0);
    EXPECT_DOUBLE_EQ(dup->centerPos().y(), 225.0);

    delete dup;
}

TEST(CGraphItemEventCreatTransDuplicate, OriginalEventUnchangedAfterDuplicate)
{
    CGraphItemEvent event(CGraphItemEvent::EMove);
    event.setPos(QPointF(10.0, 20.0));
    event.setOldPos(QPointF(5.0, 10.0));

    QTransform tran;
    tran.translate(100.0, 200.0);

    CGraphItemEvent *dup = event.creatTransDuplicate(tran, QSizeF(50.0, 60.0));

    ASSERT_NE(dup, nullptr);
    // Original event should not be modified
    EXPECT_DOUBLE_EQ(event.pos().x(), 10.0);
    EXPECT_DOUBLE_EQ(event.pos().y(), 20.0);
    EXPECT_DOUBLE_EQ(event.oldPos().x(), 5.0);
    EXPECT_DOUBLE_EQ(event.oldPos().y(), 10.0);

    delete dup;
}

// =========================================================================
// CGraphItemScalEvent::setYTransBlocked
// =========================================================================

TEST(CGraphItemScalEventSetYTransBlocked, SetTrue_MarksTransDirty)
{
    CGraphItemScalEvent event(CGraphItemScalEvent::EScal);
    event.setTrans(QTransform());
    ASSERT_FALSE(event._transDirty);

    event.setYTransBlocked(true);

    EXPECT_TRUE(event._blockYTrans);
    EXPECT_TRUE(event._transDirty);
}

TEST(CGraphItemScalEventSetYTransBlocked, SetFalse_MarksTransDirty)
{
    CGraphItemScalEvent event(CGraphItemScalEvent::EScal);
    event._blockYTrans = true;
    event.setTrans(QTransform());
    ASSERT_FALSE(event._transDirty);

    event.setYTransBlocked(false);

    EXPECT_FALSE(event._blockYTrans);
    EXPECT_TRUE(event._transDirty);
}

// =========================================================================
// CGraphItemScalEvent::setXNegtiveOffset
// =========================================================================

TEST(CGraphItemScalEventSetXNegtiveOffset, SetTrue_MarksTransDirty)
{
    CGraphItemScalEvent event(CGraphItemScalEvent::EScal);
    event.setTrans(QTransform());
    ASSERT_FALSE(event._transDirty);

    event.setXNegtiveOffset(true);

    EXPECT_TRUE(event._isXNegtiveOffset);
    EXPECT_TRUE(event._transDirty);
}

TEST(CGraphItemScalEventSetXNegtiveOffset, SetFalse_MarksTransDirty)
{
    CGraphItemScalEvent event(CGraphItemScalEvent::EScal);
    event._isXNegtiveOffset = true;
    event.setTrans(QTransform());
    ASSERT_FALSE(event._transDirty);

    event.setXNegtiveOffset(false);

    EXPECT_FALSE(event._isXNegtiveOffset);
    EXPECT_TRUE(event._transDirty);
}

// =========================================================================
// CGraphItemScalEvent::setYNegtiveOffset
// =========================================================================

TEST(CGraphItemScalEventSetYNegtiveOffset, SetTrue_MarksTransDirty)
{
    CGraphItemScalEvent event(CGraphItemScalEvent::EScal);
    event.setTrans(QTransform());
    ASSERT_FALSE(event._transDirty);

    event.setYNegtiveOffset(true);

    EXPECT_TRUE(event._isYNegtiveOffset);
    EXPECT_TRUE(event._transDirty);
}

TEST(CGraphItemScalEventSetYNegtiveOffset, SetFalse_MarksTransDirty)
{
    CGraphItemScalEvent event(CGraphItemScalEvent::EScal);
    event._isYNegtiveOffset = true;
    event.setTrans(QTransform());
    ASSERT_FALSE(event._transDirty);

    event.setYNegtiveOffset(false);

    EXPECT_FALSE(event._isYNegtiveOffset);
    EXPECT_TRUE(event._transDirty);
}

// =========================================================================
// CGraphItemScalEvent::setMayResultPolygon
// =========================================================================

TEST(CGraphItemScalEventSetMayResultPolygon, SetValidPolygon_StoresValue)
{
    CGraphItemScalEvent event(CGraphItemScalEvent::EScal);

    QPolygonF poly;
    poly << QPointF(0, 0) << QPointF(100, 0) << QPointF(100, 100) << QPointF(0, 100);
    event.setMayResultPolygon(poly);

    EXPECT_EQ(event._driverMayResultRect.size(), 4);
    EXPECT_DOUBLE_EQ(event._driverMayResultRect[0].x(), 0.0);
    EXPECT_DOUBLE_EQ(event._driverMayResultRect[1].x(), 100.0);
    EXPECT_DOUBLE_EQ(event._driverMayResultRect[2].y(), 100.0);
}

TEST(CGraphItemScalEventSetMayResultPolygon, SetEmptyPolygon)
{
    CGraphItemScalEvent event(CGraphItemScalEvent::EScal);

    event.setMayResultPolygon(QPolygonF());

    EXPECT_TRUE(event._driverMayResultRect.isEmpty());
}

TEST(CGraphItemScalEventSetMayResultPolygon, OverwritePreviousPolygon)
{
    CGraphItemScalEvent event(CGraphItemScalEvent::EScal);

    QPolygonF poly1;
    poly1 << QPointF(0, 0) << QPointF(10, 10);
    event.setMayResultPolygon(poly1);

    QPolygonF poly2;
    poly2 << QPointF(50, 50) << QPointF(60, 60) << QPointF(70, 70);
    event.setMayResultPolygon(poly2);

    EXPECT_EQ(event._driverMayResultRect.size(), 3);
    EXPECT_DOUBLE_EQ(event._driverMayResultRect[0].x(), 50.0);
}
