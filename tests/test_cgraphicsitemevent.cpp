// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CGraphItemEvent accessors (cgraphicsitemevent.h).
 *
 * Target functions (from issue V-5568):
 *   - CGraphItemEvent::centerPos()   (inline getter, line 42)
 *   - CGraphItemEvent::setCenterPos()
 *
 * These are simple inline getters/setters in the header, so no
 * application instance is needed.
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QPointF>
#include <QSizeF>

#define protected public
#define private public
#include "drawshape/drawItems/bzItems/cgraphicsitemevent.h"
#undef protected
#undef private

TEST(CGraphItemEventCenterPos, DefaultConstructor_HasInvalidCenterPos)
{
    CGraphItemEvent event(CGraphItemEvent::EMove);
    // Default-constructed centerPos is QPointF() which is invalid
    EXPECT_TRUE(event.centerPos().isNull());
    // Actually QPointF() default is (0,0) which IS null, so check
    // that it starts as (0,0)
    EXPECT_DOUBLE_EQ(event.centerPos().x(), 0.0);
    EXPECT_DOUBLE_EQ(event.centerPos().y(), 0.0);
}

TEST(CGraphItemEventCenterPos, SetCenterPos_ReturnsSameValue)
{
    CGraphItemEvent event(CGraphItemEvent::EScal);

    QPointF expected(10.5, -20.3);
    event.setCenterPos(expected);

    EXPECT_DOUBLE_EQ(event.centerPos().x(), expected.x());
    EXPECT_DOUBLE_EQ(event.centerPos().y(), expected.y());
}

TEST(CGraphItemEventCenterPos, SetCenterPos_OverwritePreviousValue)
{
    CGraphItemEvent event(CGraphItemEvent::ERot);

    event.setCenterPos(QPointF(100.0, 200.0));
    EXPECT_DOUBLE_EQ(event.centerPos().x(), 100.0);
    EXPECT_DOUBLE_EQ(event.centerPos().y(), 200.0);

    event.setCenterPos(QPointF(-50.0, -75.0));
    EXPECT_DOUBLE_EQ(event.centerPos().x(), -50.0);
    EXPECT_DOUBLE_EQ(event.centerPos().y(), -75.0);
}

TEST(CGraphItemEventCenterPos, SetCenterPos_ZeroPoint)
{
    CGraphItemEvent event(CGraphItemEvent::EBlur);

    event.setCenterPos(QPointF(0.0, 0.0));
    EXPECT_TRUE(event.centerPos().isNull());
}

TEST(CGraphItemEventCenterPos, OtherAccessors_WorkWithCenterPos)
{
    CGraphItemEvent event(CGraphItemEvent::EMove);

    event.setCenterPos(QPointF(5.0, 5.0));
    event.setOldPos(QPointF(0.0, 0.0));
    event.setPos(QPointF(10.0, 10.0));
    event.setBeginPos(QPointF(2.0, 2.0));

    EXPECT_DOUBLE_EQ(event.centerPos().x(), 5.0);
    EXPECT_DOUBLE_EQ(event.centerPos().y(), 5.0);
    EXPECT_DOUBLE_EQ(event.oldPos().x(), 0.0);
    EXPECT_DOUBLE_EQ(event.pos().x(), 10.0);
    EXPECT_DOUBLE_EQ(event.beginPos().x(), 2.0);

    // offset = pos - oldPos
    EXPECT_DOUBLE_EQ(event.offset().x(), 10.0);
    // totalOffset = pos - beginPos
    EXPECT_DOUBLE_EQ(event.totalOffset().x(), 8.0);
}
