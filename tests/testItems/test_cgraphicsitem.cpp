// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <sstream>

#define protected public
#define private public
#include "cgraphicsview.h"
#include "cgraphicsrectitem.h"
#include "cgraphicstextitem.h"
#include "csizehandlerect.h"
#include "cgraphicsitemselectedmgr.h"
#include "sitemdata.h"
#include "cgraphicsitemevent.h"
#include "globaldefine.h"
#include "application.h"
#include "mainwindow.h"
#include "cdrawscene.h"
#undef protected
#undef private

#include "publicApi.h"

// =========================================================================
// Test fixture — provides a fresh view for each test
// =========================================================================
class CGraphicsItemTest : public ::testing::Test {
protected:
    void SetUp() override
    {
        createNewViewByShortcutKey();
        m_view = getCurView();
        ASSERT_NE(m_view, nullptr);
    }

    PageView *m_view = nullptr;
};

// =========================================================================
// CGraphicsItem::getCenter tests
// =========================================================================

TEST_F(CGraphicsItemTest, GetCenterAllDirections)
{
    CGraphicsRectItem *item = new CGraphicsRectItem(10.0, 20.0, 100.0, 50.0, nullptr);
    ASSERT_NE(item, nullptr);

    QRectF rect = item->rect();
    // rect is normalized: topLeft=(10,20), size=(100,50)
    // bottomRight = (110, 70), center = (60, 45)

    // LeftTop -> bottomRight
    EXPECT_EQ(item->getCenter(CSizeHandleRect::LeftTop), rect.bottomRight());
    // Top -> (center.x, bottom)
    EXPECT_EQ(item->getCenter(CSizeHandleRect::Top), QPointF(rect.center().x(), rect.bottom()));
    // RightTop -> bottomLeft
    EXPECT_EQ(item->getCenter(CSizeHandleRect::RightTop), rect.bottomLeft());
    // Right -> (left, center.y)
    EXPECT_EQ(item->getCenter(CSizeHandleRect::Right), QPointF(rect.left(), rect.center().y()));
    // RightBottom -> topLeft
    EXPECT_EQ(item->getCenter(CSizeHandleRect::RightBottom), rect.topLeft());
    // Bottom -> (center.x, top)
    EXPECT_EQ(item->getCenter(CSizeHandleRect::Bottom), QPointF(rect.center().x(), rect.top()));
    // LeftBottom -> topRight
    EXPECT_EQ(item->getCenter(CSizeHandleRect::LeftBottom), rect.topRight());
    // Left -> (right, center.y)
    EXPECT_EQ(item->getCenter(CSizeHandleRect::Left), QPointF(rect.right(), rect.center().y()));
    // Rotation -> center
    EXPECT_EQ(item->getCenter(CSizeHandleRect::Rotation), rect.center());
    // InRect (default) -> center
    EXPECT_EQ(item->getCenter(CSizeHandleRect::InRect), rect.center());

    delete item;
}

// =========================================================================
// CGraphicsItem::itemChange tests
// =========================================================================

TEST_F(CGraphicsItemTest, ItemChangeSceneHasChanged)
{
    CGraphicsRectItem *item = new CGraphicsRectItem(0, 0, 50, 50, nullptr);
    ASSERT_NE(item, nullptr);

    // Adding to scene triggers ItemSceneHasChanged
    m_view->drawScene()->addItem(item);

    QVariant result = item->itemChange(QGraphicsItem::ItemSceneHasChanged,
                                        QVariant::fromValue<QGraphicsScene *>(m_view->drawScene()));
    EXPECT_FALSE(result.isNull());

    delete item;
}

TEST_F(CGraphicsItemTest, ItemChangeZValueHasChanged)
{
    CGraphicsRectItem *item = new CGraphicsRectItem(0, 0, 50, 50, nullptr);
    ASSERT_NE(item, nullptr);
    m_view->drawScene()->addItem(item);

    // ItemZValueHasChanged should return value unchanged
    QVariant testVal(42);
    QVariant result = item->itemChange(QGraphicsItem::ItemZValueHasChanged, testVal);
    EXPECT_EQ(result.toInt(), 42);

    delete item;
}

TEST_F(CGraphicsItemTest, ItemChangeSelectedHasChanged)
{
    CGraphicsRectItem *item = new CGraphicsRectItem(0, 0, 50, 50, nullptr);
    ASSERT_NE(item, nullptr);
    m_view->drawScene()->addItem(item);

    // ItemSelectedHasChanged with false
    QVariant result = item->itemChange(QGraphicsItem::ItemSelectedHasChanged, QVariant(false));
    EXPECT_FALSE(result.toBool());

    // ItemSelectedHasChanged with true
    result = item->itemChange(QGraphicsItem::ItemSelectedHasChanged, QVariant(true));
    EXPECT_TRUE(result.toBool());

    delete item;
}

TEST_F(CGraphicsItemTest, ItemChangeUnknownChangeReturnsValue)
{
    CGraphicsRectItem *item = new CGraphicsRectItem(0, 0, 50, 50, nullptr);
    ASSERT_NE(item, nullptr);
    m_view->drawScene()->addItem(item);

    QVariant testVal("test");
    QVariant result = item->itemChange(QGraphicsItem::ItemVisibleChange, testVal);
    EXPECT_EQ(result.toString(), QString("test"));

    delete item;
}

// =========================================================================
// CGraphicsItem::operating tests
// =========================================================================

TEST_F(CGraphicsItemTest, OperatingEBlurEventNoOp)
{
    CGraphicsRectItem *item = new CGraphicsRectItem(0, 0, 50, 50, nullptr);
    ASSERT_NE(item, nullptr);
    m_view->drawScene()->addItem(item);

    CGraphItemEvent event(CGraphItemEvent::EBlur);
    // Should not crash, EBlur is a no-op
    item->operating(&event);

    delete item;
}

TEST_F(CGraphicsItemTest, OperatingUnknownEventNoOp)
{
    CGraphicsRectItem *item = new CGraphicsRectItem(0, 0, 50, 50, nullptr);
    ASSERT_NE(item, nullptr);
    m_view->drawScene()->addItem(item);

    CGraphItemEvent event(static_cast<CGraphItemEvent::EItemType>(999));
    // Should not crash, unknown type hits default
    item->operating(&event);

    delete item;
}

// =========================================================================
// CGraphicsItem::zItem tests
// =========================================================================

TEST(CGraphicsItemZItem, EmptyListReturnsNullptr)
{
    QList<CGraphicsItem *> emptyList;
    CGraphicsRectItem dummyItem(0, 0, 10, 10, nullptr);
    EXPECT_EQ(dummyItem.zItem(emptyList, -1), nullptr);
    EXPECT_EQ(dummyItem.zItem(emptyList, -2), nullptr);
}

TEST_F(CGraphicsItemTest, ZItemAscendingOrderReturnsLowestZ)
{
    CGraphicsRectItem *item1 = new CGraphicsRectItem(0, 0, 10, 10, nullptr);
    CGraphicsRectItem *item2 = new CGraphicsRectItem(0, 0, 10, 10, nullptr);
    CGraphicsRectItem *item3 = new CGraphicsRectItem(0, 0, 10, 10, nullptr);

    item1->setZValue(1.0);
    item2->setZValue(5.0);
    item3->setZValue(3.0);

    QList<CGraphicsItem *> items;
    items << item1 << item2 << item3;

    // wantZitemTp == -1 means ascending sort, return first = lowest z = item1
    CGraphicsItem *result = item1->zItem(items, -1);
    ASSERT_NE(result, nullptr);
    EXPECT_EQ(result->zValue(), 1.0);

    delete item1;
    delete item2;
    delete item3;
}

TEST_F(CGraphicsItemTest, ZItemDescendingOrderReturnsHighestZ)
{
    CGraphicsRectItem *item1 = new CGraphicsRectItem(0, 0, 10, 10, nullptr);
    CGraphicsRectItem *item2 = new CGraphicsRectItem(0, 0, 10, 10, nullptr);
    CGraphicsRectItem *item3 = new CGraphicsRectItem(0, 0, 10, 10, nullptr);

    item1->setZValue(1.0);
    item2->setZValue(5.0);
    item3->setZValue(3.0);

    QList<CGraphicsItem *> items;
    items << item1 << item2 << item3;

    // wantZitemTp == -2 means descending sort, return first = highest z = item2
    CGraphicsItem *result = item1->zItem(items, -2);
    ASSERT_NE(result, nullptr);
    EXPECT_EQ(result->zValue(), 5.0);

    delete item1;
    delete item2;
    delete item3;
}

TEST_F(CGraphicsItemTest, ZItemInvalidWantZItemTpReturnsNullptr)
{
    CGraphicsRectItem *item1 = new CGraphicsRectItem(0, 0, 10, 10, nullptr);
    item1->setZValue(1.0);

    QList<CGraphicsItem *> items;
    items << item1;

    // wantZitemTp == 0 is not -1 or -2, should return nullptr
    CGraphicsItem *result = item1->zItem(items, 0);
    EXPECT_EQ(result, nullptr);

    delete item1;
}
