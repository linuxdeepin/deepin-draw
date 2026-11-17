// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CGraphicsLineItem / CGraphicsCutItem constructors and
 * factory registration (issue V-8626)
 *
 * Existing test_cgraphicslineitem_funcs.cpp tests CGraphicsLineItem(qreal,...)
 * and methods but NOT the QLineF/QPointF constructors or CGraphicsCutItem.
 *
 * Source:
 *   src/drawshape/drawItems/bzItems/cgraphicslineitem.cpp
 *   src/drawshape/drawItems/bzItems/cgraphicscutitem.cpp
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QLineF>
#include <QPointF>
#include <QRectF>

#define protected public
#define private public
#include "drawshape/drawItems/bzItems/cgraphicslineitem.h"
#include "drawshape/drawItems/bzItems/cgraphicscutitem.h"
#include "drawshape/drawItems/bzItems/cgraphicsitem.h"
#include "service/dyncreatobject.h"
#include "drawshape/globaldefine.h"
#undef protected
#undef private

// ===========================================================================
// CGraphicsLineItem(const QLineF &, QGraphicsItem *)
// ===========================================================================

TEST(CGraphicsLineItemCtorTest, QLineFConstructorSetsLine)
{
    QLineF line(10.0, 20.0, 30.0, 40.0);
    CGraphicsLineItem item(line);

    EXPECT_EQ(item.line(), line);
}

TEST(CGraphicsLineItemCtorTest, QLineFConstructorWithParent)
{
    QLineF line(0.0, 0.0, 100.0, 100.0);
    CGraphicsLineItem parentItem;
    CGraphicsLineItem item(line, &parentItem);

    EXPECT_EQ(item.line(), line);
    EXPECT_EQ(item.parentItem(), &parentItem);
}

// ===========================================================================
// CGraphicsLineItem(const QPointF &, const QPointF &, QGraphicsItem *)
// ===========================================================================

TEST(CGraphicsLineItemCtorTest, TwoPointFConstructorSetsLine)
{
    QPointF p1(5.0, 15.0);
    QPointF p2(95.0, 85.0);
    CGraphicsLineItem item(p1, p2);

    EXPECT_EQ(item.line().p1(), p1);
    EXPECT_EQ(item.line().p2(), p2);
}

TEST(CGraphicsLineItemCtorTest, TwoPointFConstructorWithParent)
{
    QPointF p1(0.0, 0.0);
    QPointF p2(50.0, 50.0);
    CGraphicsLineItem parentItem;
    CGraphicsLineItem item(p1, p2, &parentItem);

    EXPECT_EQ(item.line().p1(), p1);
    EXPECT_EQ(item.line().p2(), p2);
    EXPECT_EQ(item.parentItem(), &parentItem);
}

// ===========================================================================
// CGraphicsCutItem(CGraphicsItem *)
// ===========================================================================

TEST(CGraphicsCutItemCtorTest, DefaultConstructorDoesNotCrash)
{
    CGraphicsCutItem cutItem;
    EXPECT_GE(cutItem.type(), 0);
}

TEST(CGraphicsCutItemCtorTest, ConstructorWithParentItem)
{
    CGraphicsLineItem parentItem;
    CGraphicsCutItem cutItem(static_cast<CGraphicsItem *>(&parentItem));

    EXPECT_EQ(cutItem.parentItem(), &parentItem);
}

TEST(CGraphicsCutItemCtorTest, ConstructorWithNullParent)
{
    CGraphicsCutItem cutItem(nullptr);
    EXPECT_EQ(cutItem.parentItem(), nullptr);
}

// ===========================================================================
// CGraphicsCutItemRegisterNameSpace::creatInstance (via ClassObjectFactory)
// ===========================================================================

TEST(CGraphicsCutItemRegisterTest, FactoryCreatesCutItem)
{
    // The REGISTITEMCLASS macro registered CGraphicsCutItem at static init
    auto *obj = ClassObjectFactory::creatObject<CGraphicsCutItem>("CGraphicsCutItem");
    ASSERT_NE(obj, nullptr);
    EXPECT_GE(obj->type(), 0);
    delete obj;
}

TEST(CGraphicsCutItemRegisterTest, FactoryReturnsNullForUnknownName)
{
    auto *obj = ClassObjectFactory::creatObject<CGraphicsCutItem>("NonExistentClass");
    EXPECT_EQ(obj, nullptr);
}
