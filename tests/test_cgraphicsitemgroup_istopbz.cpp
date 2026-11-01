// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CGraphicsItemGroup::isTopBzGroup
 * (issue V-7262)
 *
 * Source: src/drawshape/drawItems/cgraphicsitemselectedmgr.cpp
 *
 * Target method:
 *   bool isTopBzGroup() const  —  returns bzGroup() == nullptr
 *
 * When the group has no parent group (bzGroup() returns nullptr),
 * isTopBzGroup returns true. When it has a parent group, returns false.
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QPointF>
#include <QRectF>
#include <QPainterPath>

#define protected public
#define private public
#include "drawshape/drawItems/bzItems/cgraphicsitem.h"
#include "drawshape/drawItems/cgraphicsitemselectedmgr.h"
#include "drawshape/drawItems/bzItems/cgraphicsitemevent.h"
#include "drawshape/drawItems/bzItems/cgraphicsrectitem.h"
#include "drawshape/sitemdata.h"
#include "drawshape/globaldefine.h"
#include "cdrawparamsigleton.h"
#include "cdrawscene.h"
#undef protected
#undef private

// =========================================================================
// isTopBzGroup — no parent group (bzGroup == nullptr)
// =========================================================================

TEST(CGraphicsItemGroupIsTopBzTest, NoParentGroupReturnsTrue)
{
    CGraphicsItemGroup group(CGraphicsItemGroup::ENormalGroup, "testGroup");

    // No parent group set → bzGroup() returns nullptr → isTopBzGroup returns true
    EXPECT_TRUE(group.isTopBzGroup());
}

TEST(CGraphicsItemGroupIsTopBzTest, SelectGroupNoParentReturnsTrue)
{
    CGraphicsItemGroup group(CGraphicsItemGroup::ESelectGroup, "selectGroup");

    EXPECT_TRUE(group.isTopBzGroup());
}

// =========================================================================
// isTopBzGroup — with parent group (bzGroup != nullptr)
// =========================================================================

TEST(CGraphicsItemGroupIsTopBzTest, WithParentGroupReturnsFalse)
{
    CGraphicsItemGroup parentGroup(CGraphicsItemGroup::ENormalGroup, "parent");
    CGraphicsItemGroup childGroup(CGraphicsItemGroup::ENormalGroup, "child");

    // Set the parent group on the child
    childGroup.setBzGroup(&parentGroup);

    // Now childGroup.bzGroup() returns &parentGroup → isTopBzGroup returns false
    EXPECT_FALSE(childGroup.isTopBzGroup());
}

TEST(CGraphicsItemGroupIsTopBzTest, ParentGroupItselfIsTopBz)
{
    CGraphicsItemGroup parentGroup(CGraphicsItemGroup::ENormalGroup, "parent");
    CGraphicsItemGroup childGroup(CGraphicsItemGroup::ENormalGroup, "child");

    childGroup.setBzGroup(&parentGroup);

    // The parent group has no parent of its own → isTopBzGroup returns true
    EXPECT_TRUE(parentGroup.isTopBzGroup());
}

// =========================================================================
// isTopBzGroup — after removing parent group
// =========================================================================

TEST(CGraphicsItemGroupIsTopBzTest, AfterClearingParentReturnsTrue)
{
    CGraphicsItemGroup parentGroup(CGraphicsItemGroup::ENormalGroup, "parent");
    CGraphicsItemGroup childGroup(CGraphicsItemGroup::ENormalGroup, "child");

    childGroup.setBzGroup(&parentGroup);
    EXPECT_FALSE(childGroup.isTopBzGroup());

    // Clear the parent group
    childGroup.setBzGroup(nullptr);
    EXPECT_TRUE(childGroup.isTopBzGroup());
}

// =========================================================================
// isTopBzGroup — with items but no parent group
// =========================================================================

TEST(CGraphicsItemGroupIsTopBzTest, WithItemsNoParentReturnsTrue)
{
    CGraphicsItemGroup group(CGraphicsItemGroup::ENormalGroup, "grp");
    CGraphicsRectItem item1(0, 0, 50, 50, nullptr);
    CGraphicsRectItem item2(0, 0, 50, 50, nullptr);
    group.add(&item1, false, false);
    group.add(&item2, false, false);

    EXPECT_TRUE(group.isTopBzGroup());
}
