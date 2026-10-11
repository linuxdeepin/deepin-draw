// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CShapeMimeData methods (issue V-5575)
 *
 * Source: src/drawshape/cshapemimedata.cpp
 *
 * Target methods:
 *   CShapeMimeData::hasFormat, itemsTreeInfo
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#define protected public
#define private public
#include "drawshape/cshapemimedata.h"
#include "drawshape/sitemdata.h"
#undef protected
#undef private

// =========================================================================
// CShapeMimeData::hasFormat
// =========================================================================

TEST(CShapeMimeDataHasFormatTest, DrawItemsFormatReturnsTrue)
{
    CGroupBzItemsTreeInfo treeInfo;
    CShapeMimeData mimeData(treeInfo);
    EXPECT_TRUE(mimeData.hasFormat(QStringLiteral("drawItems")));
}

TEST(CShapeMimeDataHasFormatTest, OtherFormatReturnsFalse)
{
    CGroupBzItemsTreeInfo treeInfo;
    CShapeMimeData mimeData(treeInfo);
    EXPECT_FALSE(mimeData.hasFormat(QStringLiteral("text/plain")));
}

TEST(CShapeMimeDataHasFormatTest, EmptyStringReturnsFalse)
{
    CGroupBzItemsTreeInfo treeInfo;
    CShapeMimeData mimeData(treeInfo);
    EXPECT_FALSE(mimeData.hasFormat(QStringLiteral("")));
}

TEST(CShapeMimeDataHasFormatTest, ApplicationJsonReturnsFalse)
{
    CGroupBzItemsTreeInfo treeInfo;
    CShapeMimeData mimeData(treeInfo);
    EXPECT_FALSE(mimeData.hasFormat(QStringLiteral("application/json")));
}

// =========================================================================
// CShapeMimeData::itemsTreeInfo
// =========================================================================

namespace {

// Builds a tree with non-default values in the group data, a rect item and a
// nested child group holding a line item, so comparisons are meaningful.
CGroupBzItemsTreeInfo makeTreeInfo()
{
    CGroupBzItemsTreeInfo treeInfo;
    treeInfo.data.data.pGroup->groupType = 3;
    treeInfo.data.data.pGroup->name = QStringLiteral("root-group");
    treeInfo.data.data.pGroup->isCancelAble = false;

    CGraphicsUnit rectUnit;
    rectUnit.head.dataType = RectType;
    rectUnit.head.pos = QPointF(1.5, 2.5);
    rectUnit.head.rotate = 45.0;
    rectUnit.head.zValue = 7.0;
    rectUnit.head.rect = QRectF(0, 0, 30, 20);
    rectUnit.data.pRect = new SGraphicsRectUnitData{QPointF(1, 2), QPointF(31, 22), 4, 6};
    treeInfo.bzItems.append(rectUnit);

    CGroupBzItemsTreeInfo childInfo;
    childInfo.data.data.pGroup->groupType = 5;
    CGraphicsUnit lineUnit;
    lineUnit.head.dataType = LineType;
    lineUnit.data.pLine = new SGraphicsLineUnitData{QPointF(0, 0), QPointF(9, 9), soildArrow, noneLine};
    childInfo.bzItems.append(lineUnit);
    treeInfo.childGroups.append(childInfo);

    return treeInfo;
}

// Compares every field of `actual` against the non-default `expected` tree.
// Must be called while the CShapeMimeData that produced `actual` is still
// alive: its destructor releases the (shared) bzItems data.
void expectTreeInfoEquals(const CGroupBzItemsTreeInfo &actual,
                          const CGroupBzItemsTreeInfo &expected)
{
    ASSERT_EQ(actual.bzItems.count(), 1);
    const CGraphicsUnit &rect = actual.bzItems[0];
    EXPECT_EQ(rect.head.dataType, static_cast<int>(RectType));
    EXPECT_EQ(rect.head.pos, QPointF(1.5, 2.5));
    EXPECT_DOUBLE_EQ(rect.head.rotate, 45.0);
    EXPECT_DOUBLE_EQ(rect.head.zValue, 7.0);
    EXPECT_EQ(rect.head.rect, QRectF(0, 0, 30, 20));
    ASSERT_NE(rect.data.pRect, nullptr);
    EXPECT_EQ(expected.bzItems[0].data.pRect, rect.data.pRect);
    EXPECT_EQ(rect.data.pRect->topLeft, QPointF(1, 2));
    EXPECT_EQ(rect.data.pRect->bottomRight, QPointF(31, 22));
    EXPECT_EQ(rect.data.pRect->xRedius, 4);
    EXPECT_EQ(rect.data.pRect->yRedius, 6);

    ASSERT_NE(actual.data.data.pGroup, nullptr);
    EXPECT_EQ(actual.data.data.pGroup->groupType, 3);
    EXPECT_EQ(actual.data.data.pGroup->name, QStringLiteral("root-group"));
    EXPECT_FALSE(actual.data.data.pGroup->isCancelAble);

    ASSERT_EQ(actual.childGroups.count(), 1);
    const CGroupBzItemsTreeInfo &child = actual.childGroups[0];
    ASSERT_NE(child.data.data.pGroup, nullptr);
    EXPECT_EQ(child.data.data.pGroup->groupType, 5);
    ASSERT_EQ(child.bzItems.count(), 1);
    const CGraphicsUnit &line = child.bzItems[0];
    EXPECT_EQ(line.head.dataType, static_cast<int>(LineType));
    ASSERT_NE(line.data.pLine, nullptr);
    EXPECT_EQ(line.data.pLine->point1, QPointF(0, 0));
    EXPECT_EQ(line.data.pLine->point2, QPointF(9, 9));
    EXPECT_EQ(line.data.pLine->start_type, soildArrow);
    EXPECT_EQ(line.data.pLine->end_type, noneLine);
}

} // namespace

TEST(CShapeMimeDataItemsTreeInfoTest, ReturnsConstructedTreeInfo)
{
    CGroupBzItemsTreeInfo treeInfo = makeTreeInfo();
    CShapeMimeData mimeData(treeInfo);
    // The returned treeInfo must preserve the non-default data passed in.
    expectTreeInfoEquals(mimeData.itemsTreeInfo(), treeInfo);
}

TEST(CShapeMimeDataItemsTreeInfoTest, ReturnsSameObjectAfterConstruction)
{
    CGroupBzItemsTreeInfo treeInfo = makeTreeInfo();
    CShapeMimeData mimeData(treeInfo);
    // Both calls must return the same underlying data as what was passed in.
    expectTreeInfoEquals(mimeData.itemsTreeInfo(), treeInfo);
    expectTreeInfoEquals(mimeData.itemsTreeInfo(), treeInfo);
}
