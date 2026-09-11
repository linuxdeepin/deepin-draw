// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <cstring>

#define protected public
#define private public
#include "sitemdata.h"
#include "cgraphicsview.h"
#include "globaldefine.h"
#include "application.h"
#include "mainwindow.h"
#include "cdrawscene.h"
#undef protected
#undef private

#include "publicApi.h"

// =========================================================================
// CGraphicsUnit::deepCopy tests
// =========================================================================

TEST(CGraphicsUnitDeepCopy, RectTypeCopySuccess)
{
    CGraphicsUnit source;
    source.head.dataType = RectType;
    source.data.pRect = new SGraphicsRectUnitData;
    source.data.pRect->topLeft = QPointF(10.0, 20.0);
    source.data.pRect->bottomRight = QPointF(100.0, 200.0);
    source.data.pRect->xRedius = 5;
    source.data.pRect->yRedius = 10;

    CGraphicsUnit des;
    CGraphicsUnit::deepCopy(des, source);

    ASSERT_NE(des.data.pRect, nullptr);
    EXPECT_EQ(des.data.pRect->topLeft, QPointF(10.0, 20.0));
    EXPECT_EQ(des.data.pRect->bottomRight, QPointF(100.0, 200.0));
    EXPECT_EQ(des.data.pRect->xRedius, 5);
    EXPECT_EQ(des.data.pRect->yRedius, 10);
    // Verify deep copy: pointers should be different
    EXPECT_NE(des.data.pRect, source.data.pRect);

    source.release();
    des.release();
}

TEST(CGraphicsUnitDeepCopy, RectTypeNullSourceReleasesDes)
{
    CGraphicsUnit source;
    source.head.dataType = RectType;
    source.data.pRect = nullptr;

    CGraphicsUnit des;
    des.data.pRect = new SGraphicsRectUnitData;
    des.data.pRect->topLeft = QPointF(1.0, 2.0);

    CGraphicsUnit::deepCopy(des, source);
    // Source pRect is null, so des should be released
    EXPECT_EQ(des.data.pRect, nullptr);

    source.release();
}

TEST(CGraphicsUnitDeepCopy, EllipseTypeCopySuccess)
{
    CGraphicsUnit source;
    source.head.dataType = EllipseType;
    source.data.pCircle = new SGraphicsCircleUnitData;
    source.data.pCircle->rect.topLeft = QPointF(15.0, 25.0);
    source.data.pCircle->rect.bottomRight = QPointF(95.0, 85.0);
    source.data.pCircle->rect.xRedius = 3;
    source.data.pCircle->rect.yRedius = 7;

    CGraphicsUnit des;
    CGraphicsUnit::deepCopy(des, source);

    ASSERT_NE(des.data.pCircle, nullptr);
    EXPECT_NE(des.data.pCircle, source.data.pCircle);
    EXPECT_EQ(des.data.pCircle->rect.topLeft, QPointF(15.0, 25.0));
    EXPECT_EQ(des.data.pCircle->rect.bottomRight, QPointF(95.0, 85.0));
    EXPECT_EQ(des.data.pCircle->rect.xRedius, 3);
    EXPECT_EQ(des.data.pCircle->rect.yRedius, 7);

    source.release();
    des.release();
}

TEST(CGraphicsUnitDeepCopy, LineTypeCopySuccess)
{
    CGraphicsUnit source;
    source.head.dataType = LineType;
    source.data.pLine = new SGraphicsLineUnitData;
    source.data.pLine->point1 = QPointF(10.0, 20.0);
    source.data.pLine->point2 = QPointF(80.0, 90.0);
    source.data.pLine->start_type = normalArrow;
    source.data.pLine->end_type = soildArrow;

    CGraphicsUnit des;
    CGraphicsUnit::deepCopy(des, source);

    ASSERT_NE(des.data.pLine, nullptr);
    EXPECT_NE(des.data.pLine, source.data.pLine);
    EXPECT_EQ(des.data.pLine->point1, QPointF(10.0, 20.0));
    EXPECT_EQ(des.data.pLine->point2, QPointF(80.0, 90.0));
    EXPECT_EQ(des.data.pLine->start_type, normalArrow);
    EXPECT_EQ(des.data.pLine->end_type, soildArrow);

    source.release();
    des.release();
}

TEST(CGraphicsUnitDeepCopy, MgrTypeCopySuccess)
{
    CGraphicsUnit source;
    source.head.dataType = MgrType;
    source.data.pGroup = new SGraphicsGroupUnitData;
    source.data.pGroup->isCancelAble = false;
    source.data.pGroup->name = QStringLiteral("testGroup");
    source.data.pGroup->groupType = 5;

    CGraphicsUnit des;
    CGraphicsUnit::deepCopy(des, source);

    ASSERT_NE(des.data.pGroup, nullptr);
    EXPECT_NE(des.data.pGroup, source.data.pGroup);
    EXPECT_EQ(des.data.pGroup->isCancelAble, false);
    EXPECT_EQ(des.data.pGroup->name, QStringLiteral("testGroup"));
    EXPECT_EQ(des.data.pGroup->groupType, 5);

    source.release();
    des.release();
}

TEST(CGraphicsUnitDeepCopy, CopiesHeadTailReson)
{
    CGraphicsUnit source;
    source.head.dataType = RectType;
    source.data.pRect = new SGraphicsRectUnitData;
    source.tail.tailCheck[0] = 1;
    source.tail.tailCheck[1] = 2;
    source.reson = ENormal;

    CGraphicsUnit des;
    CGraphicsUnit::deepCopy(des, source);

    EXPECT_EQ(des.head.dataType, RectType);
    EXPECT_EQ(des.tail.tailCheck[0], (qint8)1);
    EXPECT_EQ(des.tail.tailCheck[1], (qint8)2);
    EXPECT_EQ(des.reson, ENormal);

    source.release();
    des.release();
}

TEST(CGraphicsUnitDeepCopy, UnknownDataTypeNoCrash)
{
    CGraphicsUnit source;
    source.head.dataType = static_cast<EGraphicUserType>(999);

    CGraphicsUnit des;
    CGraphicsUnit::deepCopy(des, source);
    // Should not crash, hits default case
    EXPECT_EQ(des.head.dataType, static_cast<EGraphicUserType>(999));

    source.release();
    des.release();
}

// =========================================================================
// CGraphicsUnit::release tests
// =========================================================================

TEST(CGraphicsUnitRelease, RectTypeReleasesPointer)
{
    CGraphicsUnit unit;
    unit.head.dataType = RectType;
    unit.data.pRect = new SGraphicsRectUnitData;
    unit.data.pRect->topLeft = QPointF(1.0, 2.0);

    unit.release();
    // After release, data should be zeroed out
    EXPECT_EQ(unit.data.pRect, nullptr);
}

TEST(CGraphicsUnitRelease, EllipseTypeReleasesPointer)
{
    CGraphicsUnit unit;
    unit.head.dataType = EllipseType;
    unit.data.pCircle = new SGraphicsCircleUnitData;

    unit.release();
    EXPECT_EQ(unit.data.pCircle, nullptr);
}

TEST(CGraphicsUnitRelease, LineTypeReleasesPointer)
{
    CGraphicsUnit unit;
    unit.head.dataType = LineType;
    unit.data.pLine = new SGraphicsLineUnitData;

    unit.release();
    EXPECT_EQ(unit.data.pLine, nullptr);
}

TEST(CGraphicsUnitRelease, MgrTypeReleasesPointer)
{
    CGraphicsUnit unit;
    unit.head.dataType = MgrType;
    unit.data.pGroup = new SGraphicsGroupUnitData;

    unit.release();
    EXPECT_EQ(unit.data.pGroup, nullptr);
}

TEST(CGraphicsUnitRelease, NullPointerNoCrash)
{
    CGraphicsUnit unit;
    unit.head.dataType = RectType;
    unit.data.pRect = nullptr;

    // Should not crash when pointer is already null
    unit.release();
    EXPECT_EQ(unit.data.pRect, nullptr);
}

TEST(CGraphicsUnitRelease, UnknownDataTypeNoCrash)
{
    CGraphicsUnit unit;
    unit.head.dataType = static_cast<EGraphicUserType>(999);

    // Should not crash, hits default case
    unit.release();
    // memset still zeroes data
    EXPECT_EQ(unit.data.pRect, nullptr);
}

TEST(CGraphicsUnitRelease, DoubleReleaseNoCrash)
{
    CGraphicsUnit unit;
    unit.head.dataType = RectType;
    unit.data.pRect = new SGraphicsRectUnitData;

    unit.release();
    // After first release, data is zeroed (pRect == nullptr, dataType == RectType)
    // Second release should not crash (null check protects)
    unit.head.dataType = RectType;
    unit.release();
    EXPECT_EQ(unit.data.pRect, nullptr);
}
