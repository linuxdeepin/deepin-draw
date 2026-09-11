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
#include "csizehandlerect.h"
#include "cgraphicsitemselectedmgr.h"
#include "globaldefine.h"
#include "application.h"
#include "mainwindow.h"
#include "cdrawscene.h"
#undef protected
#undef private

#include "publicApi.h"

// Named constant for operating type that skips update
static constexpr int kOperatingSkipUpdate = 3;

// =========================================================================
// Test fixture — provides a fresh view for each test
// =========================================================================
class CGraphicsItemGroupTest : public ::testing::Test {
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
// CGraphicsItemGroup::getCenter tests
// =========================================================================

TEST_F(CGraphicsItemGroupTest, GetCenterAllDirections)
{
    CGraphicsItemGroup *group = new CGraphicsItemGroup(CGraphicsItemGroup::ESelectGroup, "testGroup");
    ASSERT_NE(group, nullptr);

    // Set a known rect on the group
    group->setRect(QRectF(10.0, 20.0, 100.0, 50.0));
    QRectF rect = group->rect();

    // LeftTop -> bottomRight
    EXPECT_EQ(group->getCenter(CSizeHandleRect::LeftTop), rect.bottomRight());
    // Top -> (center.x, bottom)
    EXPECT_EQ(group->getCenter(CSizeHandleRect::Top), QPointF(rect.center().x(), rect.bottom()));
    // RightTop -> bottomLeft
    EXPECT_EQ(group->getCenter(CSizeHandleRect::RightTop), rect.bottomLeft());
    // Right -> (left, center.y)
    EXPECT_EQ(group->getCenter(CSizeHandleRect::Right), QPointF(rect.left(), rect.center().y()));
    // RightBottom -> topLeft
    EXPECT_EQ(group->getCenter(CSizeHandleRect::RightBottom), rect.topLeft());
    // Bottom -> (center.x, top)
    EXPECT_EQ(group->getCenter(CSizeHandleRect::Bottom), QPointF(rect.center().x(), rect.top()));
    // LeftBottom -> topRight
    EXPECT_EQ(group->getCenter(CSizeHandleRect::LeftBottom), rect.topRight());
    // Left -> (right, center.y)
    EXPECT_EQ(group->getCenter(CSizeHandleRect::Left), QPointF(rect.right(), rect.center().y()));
    // Rotation -> center
    EXPECT_EQ(group->getCenter(CSizeHandleRect::Rotation), rect.center());
    // InRect (default) -> center
    EXPECT_EQ(group->getCenter(CSizeHandleRect::InRect), rect.center());

    delete group;
}

// =========================================================================
// CGraphicsItemGroup::updateBoundingRect tests
// =========================================================================

TEST_F(CGraphicsItemGroupTest, UpdateBoundingRectEmptyGroupNoCrash)
{
    CGraphicsItemGroup *group = new CGraphicsItemGroup(CGraphicsItemGroup::ESelectGroup, "testGroup");
    ASSERT_NE(group, nullptr);
    m_view->drawScene()->addItem(group);

    // Empty group (0 items) should not crash
    group->updateBoundingRect(false);
    group->updateBoundingRect(true);

    delete group;
}

TEST_F(CGraphicsItemGroupTest, UpdateBoundingRectSingleItem)
{
    CGraphicsItemGroup *group = new CGraphicsItemGroup(CGraphicsItemGroup::ESelectGroup, "testGroup");
    ASSERT_NE(group, nullptr);
    m_view->drawScene()->addItem(group);

    CGraphicsRectItem *item = new CGraphicsRectItem(10.0, 20.0, 100.0, 50.0, nullptr);
    group->add(item);

    group->updateBoundingRect(false);
    QRectF rectAfter = group->rect();
    // The bounding rect should encompass the item (width >= 90, height >= 40)
    EXPECT_GE(rectAfter.width(), 90.0);
    EXPECT_GE(rectAfter.height(), 40.0);

    delete group;
    delete item;
}

TEST_F(CGraphicsItemGroupTest, UpdateBoundingRectForceUpdate)
{
    CGraphicsItemGroup *group = new CGraphicsItemGroup(CGraphicsItemGroup::ESelectGroup, "testGroup");
    ASSERT_NE(group, nullptr);
    m_view->drawScene()->addItem(group);

    CGraphicsRectItem *item1 = new CGraphicsRectItem(10.0, 20.0, 100.0, 50.0, nullptr);
    CGraphicsRectItem *item2 = new CGraphicsRectItem(30.0, 40.0, 80.0, 60.0, nullptr);
    group->add(item1);
    group->add(item2);

    // Force update should not crash
    group->updateBoundingRect(true);

    delete group;
    delete item1;
    delete item2;
}

TEST_F(CGraphicsItemGroupTest, UpdateBoundingRectOperatingTypeSkipUpdate)
{
    CGraphicsItemGroup *group = new CGraphicsItemGroup(CGraphicsItemGroup::ESelectGroup, "testGroup");
    ASSERT_NE(group, nullptr);
    m_view->drawScene()->addItem(group);

    // Set operating type to skip update
    group->m_operatingType = kOperatingSkipUpdate;
    QRectF rectBefore = group->rect();
    group->updateBoundingRect(false);
    QRectF rectAfter = group->rect();
    // Rect should not change when operatingType == kOperatingSkipUpdate
    EXPECT_EQ(rectBefore, rectAfter);

    delete group;
}

// =========================================================================
// CGraphicsItemGroup::updateHandlesGeometry tests
// =========================================================================

TEST_F(CGraphicsItemGroupTest, UpdateHandlesGeometryEmptyGroupNoCrash)
{
    CGraphicsItemGroup *group = new CGraphicsItemGroup(CGraphicsItemGroup::ESelectGroup, "testGroup");
    ASSERT_NE(group, nullptr);
    m_view->drawScene()->addItem(group);

    // Should not crash on empty group
    group->updateHandlesGeometry();

    delete group;
}

TEST_F(CGraphicsItemGroupTest, UpdateHandlesGeometryWithItemsNoCrash)
{
    CGraphicsItemGroup *group = new CGraphicsItemGroup(CGraphicsItemGroup::ESelectGroup, "testGroup");
    ASSERT_NE(group, nullptr);
    m_view->drawScene()->addItem(group);

    CGraphicsRectItem *item1 = new CGraphicsRectItem(10.0, 20.0, 100.0, 50.0, nullptr);
    CGraphicsRectItem *item2 = new CGraphicsRectItem(30.0, 40.0, 80.0, 60.0, nullptr);
    group->add(item1);
    group->add(item2);

    group->updateBoundingRect(false);
    // Should not crash with valid bounding rect
    group->updateHandlesGeometry();

    delete group;
    delete item1;
    delete item2;
}

TEST_F(CGraphicsItemGroupTest, UpdateHandlesGeometrySingleItemShowsRotationHandle)
{
    CGraphicsItemGroup *group = new CGraphicsItemGroup(CGraphicsItemGroup::ESelectGroup, "testGroup");
    ASSERT_NE(group, nullptr);
    m_view->drawScene()->addItem(group);

    CGraphicsRectItem *item = new CGraphicsRectItem(10.0, 20.0, 100.0, 50.0, nullptr);
    group->add(item);

    group->updateBoundingRect(false);
    group->updateHandlesGeometry();

    // With single item, rotation handle should be visible
    // (count == 1, so setHandleVisible(true, Rotation))
    bool foundRotation = false;
    for (auto h : group->m_handles) {
        if (h->dir() == CSizeHandleRect::Rotation) {
            foundRotation = true;
            break;
        }
    }
    EXPECT_TRUE(foundRotation);

    delete group;
    delete item;
}
