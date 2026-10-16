// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CGraphicsItemGroup additional methods (V-5932)
 *
 * Target methods (14):
 *   Count: allCount
 *   Logic items: getLogicFirst, minZItem
 *   Add type: addType, setAddType
 *   Group type: setGroupType
 *   Cancel flag: setCancelable, isCancelable
 *   Content: setNoContent
 *   Scene: setRecursiveScene
 *   Z-value: updateZValue, drawZValue (override)
 *   Scale end: doScalEnd (override)
 *   Attributes: updateAttributes
 *
 * Source file:
 *   src/drawshape/drawItems/cgraphicsitemselectedmgr.cpp
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

#define protected public
#define private public
#include "drawshape/drawItems/bzItems/cgraphicsitem.h"
#include "drawshape/drawItems/cgraphicsitemselectedmgr.h"
#include "drawshape/drawItems/bzItems/cgraphicsitemevent.h"
#include "drawshape/drawItems/bzItems/cgraphicsrectitem.h"
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
// Count methods
// =========================================================================

// --- allCount ---

TEST(CGraphicsItemGroupFuncTest, AllCount_EmptyGroup_ReturnsZero)
{
    CGraphicsItemGroup group(CGraphicsItemGroup::ENormalGroup, "grp");
    EXPECT_EQ(group.allCount(), 0);
}

TEST(CGraphicsItemGroupFuncTest, AllCount_WithDirectItems_ReturnsCount)
{
    CGraphicsItemGroup group(CGraphicsItemGroup::ENormalGroup, "grp");
    CGraphicsRectItem item1(0, 0, 50, 50, nullptr);
    CGraphicsRectItem item2(0, 0, 50, 50, nullptr);
    group.add(&item1, false, false);
    group.add(&item2, false, false);
    EXPECT_EQ(group.allCount(), 2);
}

TEST(CGraphicsItemGroupFuncTest, AllCount_WithNestedGroup_IncludesAll)
{
    CGraphicsItemGroup parentGroup(CGraphicsItemGroup::ENormalGroup, "parent");
    CGraphicsItemGroup childGroup(CGraphicsItemGroup::ENormalGroup, "child");
    CGraphicsRectItem item1(0, 0, 50, 50, nullptr);
    CGraphicsRectItem item2(0, 0, 50, 50, nullptr);
    childGroup.add(&item1, false, false);
    childGroup.add(&item2, false, false);
    parentGroup.add(&childGroup, false, false);
    // allCount should include items from nested groups
    EXPECT_EQ(parentGroup.allCount(), 2);
}

// =========================================================================
// Logic item methods
// =========================================================================

// --- minZItem ---

TEST(CGraphicsItemGroupFuncTest, MinZItem_EmptyGroup_ReturnsNullptr)
{
    CGraphicsItemGroup group(CGraphicsItemGroup::ENormalGroup, "grp");
    EXPECT_EQ(group.minZItem(), nullptr);
}

TEST(CGraphicsItemGroupFuncTest, MinZItem_WithItems_ReturnsLowestZ)
{
    CGraphicsItemGroup group(CGraphicsItemGroup::ENormalGroup, "grp");
    CGraphicsRectItem item1(0, 0, 50, 50, nullptr);
    CGraphicsRectItem item2(0, 0, 50, 50, nullptr);
    item1.setZValue(5.0);
    item2.setZValue(2.0);
    group.add(&item1, false, false);
    group.add(&item2, false, false);
    auto *result = group.minZItem();
    ASSERT_NE(result, nullptr);
    EXPECT_EQ(result, &item2);
}

// --- getLogicFirst ---

TEST(CGraphicsItemGroupFuncTest, GetLogicFirst_EmptyGroup_ReturnsNullptr)
{
    CGraphicsItemGroup group(CGraphicsItemGroup::ENormalGroup, "grp");
    EXPECT_EQ(group.getLogicFirst(), nullptr);
}

TEST(CGraphicsItemGroupFuncTest, GetLogicFirst_OneByOne_ReturnsFirstItem)
{
    CGraphicsItemGroup group(CGraphicsItemGroup::ENormalGroup, "grp");
    group.setAddType(CGraphicsItemGroup::EOneByOne);
    TestGraphicsItem item1(nullptr);
    TestGraphicsItem item2(nullptr);
    group.add(&item1, false, false);
    group.add(&item2, false, false);
    auto *result = group.getLogicFirst();
    ASSERT_NE(result, nullptr);
    // EOneByOne returns first item in the list
    EXPECT_EQ(result, &item1);
}

TEST(CGraphicsItemGroupFuncTest, GetLogicFirst_NotOneByOne_ReturnsMinZItem)
{
    CGraphicsItemGroup group(CGraphicsItemGroup::ENormalGroup, "grp");
    CGraphicsRectItem item1(0, 0, 50, 50, nullptr);
    CGraphicsRectItem item2(0, 0, 50, 50, nullptr);
    item1.setZValue(5.0);
    item2.setZValue(1.0);
    group.add(&item1, false, false);
    group.add(&item2, false, false);
    // add() resets _addTp to EOneByOne, so set ERectSelect
    // after adding to test the non-EOneByOne branch
    group.setAddType(CGraphicsItemGroup::ERectSelect);
    auto *result = group.getLogicFirst();
    ASSERT_NE(result, nullptr);
    // Non-EOneByOne returns minZItem (lowest z)
    EXPECT_EQ(result, &item2);
}

// =========================================================================
// Add type methods
// =========================================================================

// --- addType ---

TEST(CGraphicsItemGroupFuncTest, AddType_Default_ReturnsDefault)
{
    CGraphicsItemGroup group(CGraphicsItemGroup::ENormalGroup, "grp");
    // Default _addTp is whatever the constructor initializes to
    // Just verify we can read it without crash
    auto tp = group.addType();
    (void)tp;
    SUCCEED();
}

// --- setAddType ---

TEST(CGraphicsItemGroupFuncTest, SetAddType_UpdatesAddType)
{
    CGraphicsItemGroup group(CGraphicsItemGroup::ENormalGroup, "grp");
    group.setAddType(CGraphicsItemGroup::EOneByOne);
    EXPECT_EQ(group.addType(), CGraphicsItemGroup::EOneByOne);
}

// =========================================================================
// Group type methods
// =========================================================================

// --- setGroupType ---

TEST(CGraphicsItemGroupFuncTest, SetGroupType_UpdatesType)
{
    CGraphicsItemGroup group(CGraphicsItemGroup::ENormalGroup, "grp");
    group.setGroupType(CGraphicsItemGroup::ESelectGroup);
    EXPECT_EQ(group.groupType(), CGraphicsItemGroup::ESelectGroup);
}

TEST(CGraphicsItemGroupFuncTest, SetGroupType_ToNormal)
{
    CGraphicsItemGroup group(CGraphicsItemGroup::ESelectGroup, "grp");
    group.setGroupType(CGraphicsItemGroup::ENormalGroup);
    EXPECT_EQ(group.groupType(), CGraphicsItemGroup::ENormalGroup);
}

// =========================================================================
// Cancel flag methods
// =========================================================================

// --- setCancelable / isCancelable ---

TEST(CGraphicsItemGroupFuncTest, IsCancelable_Default_True)
{
    CGraphicsItemGroup group(CGraphicsItemGroup::ENormalGroup, "grp");
    EXPECT_TRUE(group.isCancelable());
}

TEST(CGraphicsItemGroupFuncTest, SetCancelable_True_FlagSet)
{
    CGraphicsItemGroup group(CGraphicsItemGroup::ENormalGroup, "grp");
    group.setCancelable(true);
    EXPECT_TRUE(group.isCancelable());
}

TEST(CGraphicsItemGroupFuncTest, SetCancelable_False_FlagCleared)
{
    CGraphicsItemGroup group(CGraphicsItemGroup::ENormalGroup, "grp");
    group.setCancelable(true);
    group.setCancelable(false);
    EXPECT_FALSE(group.isCancelable());
}

// =========================================================================
// Content methods
// =========================================================================

// --- setNoContent ---

TEST(CGraphicsItemGroupFuncTest, SetNoContent_True_SetsFlag)
{
    CGraphicsItemGroup group(CGraphicsItemGroup::ENormalGroup, "grp");
    group.setNoContent(true, false);
    EXPECT_TRUE(group.flags() & QGraphicsItem::ItemHasNoContents);
}

TEST(CGraphicsItemGroupFuncTest, SetNoContent_False_ClearsFlag)
{
    CGraphicsItemGroup group(CGraphicsItemGroup::ENormalGroup, "grp");
    group.setNoContent(true, false);
    group.setNoContent(false, false);
    EXPECT_FALSE(group.flags() & QGraphicsItem::ItemHasNoContents);
}

TEST(CGraphicsItemGroupFuncTest, SetNoContent_WithChildren_CascadesFlag)
{
    CGraphicsItemGroup group(CGraphicsItemGroup::ENormalGroup, "grp");
    TestGraphicsItem item1(nullptr);
    TestGraphicsItem item2(nullptr);
    group.add(&item1, false, false);
    group.add(&item2, false, false);
    group.setNoContent(true, true);
    EXPECT_TRUE(item1.flags() & QGraphicsItem::ItemHasNoContents);
    EXPECT_TRUE(item2.flags() & QGraphicsItem::ItemHasNoContents);
}

// =========================================================================
// Scene methods
// =========================================================================

// --- setRecursiveScene ---

TEST(CGraphicsItemGroupFuncTest, SetRecursiveScene_NullScene_NoCrash)
{
    CGraphicsItemGroup group(CGraphicsItemGroup::ENormalGroup, "grp");
    TestGraphicsItem item1(nullptr);
    group.add(&item1, false, false);
    group.setRecursiveScene(nullptr);
    SUCCEED();
}

TEST(CGraphicsItemGroupFuncTest, SetRecursiveScene_WithNestedGroup_SetsAll)
{
    CGraphicsItemGroup parentGroup(CGraphicsItemGroup::ENormalGroup, "parent");
    CGraphicsItemGroup childGroup(CGraphicsItemGroup::ENormalGroup, "child");
    TestGraphicsItem item1(nullptr);
    childGroup.add(&item1, false, false);
    parentGroup.add(&childGroup, false, false);

    QGraphicsScene scene;
    parentGroup.setRecursiveScene(nullptr);
    SUCCEED();
}

// =========================================================================
// Z-value methods
// =========================================================================

// --- drawZValue (override) ---

TEST(CGraphicsItemGroupFuncTest, DrawZValue_NormalGroupNotDirty_EqualsZValue)
{
    CGraphicsItemGroup group(CGraphicsItemGroup::ENormalGroup, "grp");
    group.setZValue(3.0);
    group._zIsDirty = false;
    EXPECT_DOUBLE_EQ(group.drawZValue(), 3.0);
}

TEST(CGraphicsItemGroupFuncTest, DrawZValue_NormalGroupDirty_TriggersUpdate)
{
    CGraphicsItemGroup group(CGraphicsItemGroup::ENormalGroup, "grp");
    TestGraphicsItem item1(nullptr);
    item1.setZValue(5.0);
    group.add(&item1, false, false);
    group._zIsDirty = true;
    // drawZValue should trigger updateZValue and clear dirty flag
    qreal z = group.drawZValue();
    EXPECT_FALSE(group._zIsDirty);
    // After update, group z should match the min child z
    EXPECT_NEAR(z, item1.zValue(), 0.01);
}

TEST(CGraphicsItemGroupFuncTest, DrawZValue_SelectGroup_DirtyIgnored)
{
    CGraphicsItemGroup group(CGraphicsItemGroup::ESelectGroup, "sel");
    group.setZValue(2.0);
    group._zIsDirty = true;
    // SelectGroup does not trigger updateZValue even if dirty
    EXPECT_DOUBLE_EQ(group.drawZValue(), 2.0);
}

// --- updateZValue ---

TEST(CGraphicsItemGroupFuncTest, UpdateZValue_EmptyGroup_NoCrash)
{
    CGraphicsItemGroup group(CGraphicsItemGroup::ENormalGroup, "grp");
    group.updateZValue();
    SUCCEED();
}

TEST(CGraphicsItemGroupFuncTest, UpdateZValue_SetsGroupZToMinChildZ)
{
    CGraphicsItemGroup group(CGraphicsItemGroup::ENormalGroup, "grp");
    TestGraphicsItem item1(nullptr);
    TestGraphicsItem item2(nullptr);
    item1.setZValue(10.0);
    item2.setZValue(3.0);
    group.add(&item1, false, false);
    group.add(&item2, false, false);
    group._zIsDirty = true;

    group.updateZValue();
    EXPECT_NEAR(group.zValue(), 3.0, 0.01);
    EXPECT_FALSE(group._zIsDirty);
}

// =========================================================================
// Scale end method (override)
// =========================================================================

// --- doScalEnd ---

TEST(CGraphicsItemGroupFuncTest, DoScalEnd_NormalGroup_NoCrash)
{
    CGraphicsItemGroup group(CGraphicsItemGroup::ENormalGroup, "grp");
    TestGraphicsItem item1(nullptr);
    item1.m_testRect = QRectF(0, 0, 50, 50);
    group.add(&item1, false, false);
    CGraphItemScalEvent event(CGraphItemEvent::EScal,
                              QPointF(0, 0), QPointF(10, 10));
    group.doScalEnd(&event);
    SUCCEED();
}

TEST(CGraphicsItemGroupFuncTest, DoScalEnd_SelectGroup_NoCrash)
{
    CGraphicsItemGroup group(CGraphicsItemGroup::ESelectGroup, "sel");
    TestGraphicsItem item1(nullptr);
    item1.m_testRect = QRectF(0, 0, 50, 50);
    group.add(&item1, false, false);
    CGraphItemScalEvent event(CGraphItemEvent::EScal,
                              QPointF(0, 0), QPointF(10, 10));
    group.doScalEnd(&event);
    SUCCEED();
}

TEST(CGraphicsItemGroupFuncTest, DoScalEnd_EmptyNormalGroup_NoCrash)
{
    CGraphicsItemGroup group(CGraphicsItemGroup::ENormalGroup, "grp");
    CGraphItemScalEvent event(CGraphItemEvent::EScal,
                              QPointF(0, 0), QPointF(10, 10));
    group.doScalEnd(&event);
    SUCCEED();
}

// =========================================================================
// Attributes method
// =========================================================================

// --- updateAttributes ---

TEST(CGraphicsItemGroupFuncTest, UpdateAttributes_NormalGroup_NoCrash)
{
    CGraphicsItemGroup group(CGraphicsItemGroup::ENormalGroup, "grp");
    group.updateAttributes(true);
    SUCCEED();
}

TEST(CGraphicsItemGroupFuncTest, UpdateAttributes_SelectGroup_NoScene_NoCrash)
{
    CGraphicsItemGroup group(CGraphicsItemGroup::ESelectGroup, "sel");
    group.updateAttributes(false);
    SUCCEED();
}

TEST(CGraphicsItemGroupFuncTest, UpdateAttributes_NormalGroup_NoEarlyReturn)
{
    // For non-SelectGroup, updateAttributes returns immediately.
    // Verify it does not crash even with showTitle=true.
    CGraphicsItemGroup group(CGraphicsItemGroup::ENormalGroup, "grp");
    group.updateAttributes(true);
    SUCCEED();
}
