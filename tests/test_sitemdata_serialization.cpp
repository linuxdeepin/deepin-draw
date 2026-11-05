// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for ddf serialization structures in sitemdata.h (issue V-7767)
 *
 * Target structures:
 *   - SGraphicsUnitHead serialization round-trip (with/without blur info)
 *   - SGraphicsLineUnitData point coords & arrow type round-trip
 *   - CGraphicsUnit::deepCopy for TextType (complements existing RectType/EllipseType/LineType tests)
 *   - CBzGroupTree copy constructor deep copy
 *
 * Source: src/drawshape/sitemdata.h
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QtCore/QtGlobal>
#include <QPen>
#include <QBrush>
#include <QPointF>
#include <QBuffer>
#include <QDataStream>
#include <QPainterPath>
#include <QTransform>

#define protected public
#define private public
#include "drawshape/sitemdata.h"
#include "drawshape/globaldefine.h"
#undef protected
#undef private

// =========================================================================
// SGraphicsUnitHead serialization — full round-trip with blur info
// Uses magic header so getVersion() returns EDdf5_9_0_3_LATER and all
// fields (blurCount, blurInfos, trans, rect) are read back.
// =========================================================================

TEST(SGraphicsUnitHeadSerialization, FullRoundTripWithBlurInfo)
{
    SGraphicsUnitHead head;
    // Magic header so getVersion() recognises a recent ddf version
    head.headCheck[0] = static_cast<qint8>(0xA0);
    head.headCheck[1] = static_cast<qint8>(0xB0);
    head.headCheck[2] = static_cast<qint8>(0xC0);
    head.headCheck[3] = static_cast<qint8>(0xD0);
    // dataType value doubles as verVar: 9 & 0xff = EDdf5_9_0_3_LATER
    head.dataType = static_cast<qint32>(EDdf5_9_0_3_LATER);
    head.dataLength = 256;
    head.pen = QPen(Qt::red, 3, Qt::DashLine);
    head.brush = QBrush(Qt::blue, Qt::CrossPattern);
    head.pos = QPointF(10.5, 20.5);
    head.rotate = 30.0;
    head.zValue = 5.0;
    head.trans = QTransform::fromScale(2.0, 3.0);
    head.rect = QRectF(0, 0, 100, 200);
    head.blurCount = 1;
    SBlurInfo blurInfo;
    blurInfo.blurPath.addRect(0, 0, 50, 50);
    blurInfo.blurEfTp = BlurEffect;
    head.blurInfos.append(blurInfo);

    QBuffer buffer;
    buffer.open(QIODevice::ReadWrite);
    QDataStream stream(&buffer);
    stream << head;

    buffer.seek(0);
    SGraphicsUnitHead head2;
    stream >> head2;

    EXPECT_EQ(head2.headCheck[0], head.headCheck[0]);
    EXPECT_EQ(head2.headCheck[1], head.headCheck[1]);
    EXPECT_EQ(head2.headCheck[2], head.headCheck[2]);
    EXPECT_EQ(head2.headCheck[3], head.headCheck[3]);
    EXPECT_EQ(head2.dataType, head.dataType);
    EXPECT_EQ(head2.dataLength, head.dataLength);
    EXPECT_EQ(head2.pen.color(), head.pen.color());
    EXPECT_EQ(head2.pen.width(), head.pen.width());
    EXPECT_EQ(head2.brush.color(), head.brush.color());
    EXPECT_EQ(head2.pos, head.pos);
    EXPECT_DOUBLE_EQ(head2.rotate, head.rotate);
    EXPECT_DOUBLE_EQ(head2.zValue, head.zValue);
    EXPECT_EQ(head2.blurCount, head.blurCount);
    ASSERT_EQ(head2.blurInfos.size(), 1);
    EXPECT_EQ(head2.blurInfos[0].blurEfTp, BlurEffect);
    EXPECT_EQ(head2.trans, head.trans);
    EXPECT_EQ(head2.rect, head.rect);
}

// =========================================================================
// SGraphicsUnitHead serialization — basic round-trip without blur info
// No magic header → getVersion() returns EDdf5_8_0_Base → only basic
// fields (headCheck, dataType, dataLength, pen, brush, pos, rotate, zValue)
// are read back.
// =========================================================================

TEST(SGraphicsUnitHeadSerialization, BasicRoundTripWithoutBlurInfo)
{
    SGraphicsUnitHead head;
    head.headCheck[0] = 0;
    head.headCheck[1] = 0;
    head.headCheck[2] = 0;
    head.headCheck[3] = 0;
    head.dataType = RectType;
    head.dataLength = 64;
    head.pen = QPen(Qt::green, 1);
    head.brush = QBrush(Qt::yellow);
    head.pos = QPointF(5, 5);
    head.rotate = 90.0;
    head.zValue = 2.0;

    QBuffer buffer;
    buffer.open(QIODevice::ReadWrite);
    QDataStream stream(&buffer);
    stream << head;

    buffer.seek(0);
    SGraphicsUnitHead head2;
    stream >> head2;

    EXPECT_EQ(head2.dataType, RectType);
    EXPECT_EQ(head2.dataLength, 64);
    EXPECT_EQ(head2.pen.color(), Qt::green);
    EXPECT_EQ(head2.brush.color(), Qt::yellow);
    EXPECT_EQ(head2.pos, QPointF(5, 5));
    EXPECT_DOUBLE_EQ(head2.rotate, 90.0);
    EXPECT_DOUBLE_EQ(head2.zValue, 2.0);
    // Without magic header, blurCount/trans/rect are not read (defaults)
    EXPECT_EQ(head2.blurCount, 0);
    EXPECT_TRUE(head2.blurInfos.isEmpty());
}

// =========================================================================
// SGraphicsLineUnitData serialization — full round-trip with arrow types
// Prepends magic header + version to the buffer so getVersion() returns
// a recent version and all fields (point1, point2, start_type, end_type)
// are read back.
// =========================================================================

TEST(SGraphicsLineUnitDataSerialization, FullRoundTripWithArrowTypes)
{
    QBuffer buffer;
    buffer.open(QIODevice::ReadWrite);
    QDataStream stream(&buffer);

    // Write magic header + version so getVersion returns a recent version
    quint32 magic = 0xA0B0C0D0;
    stream << magic;
    int version = static_cast<int>(EDdf5_9_0_3_LATER);
    stream << version;

    // Write line data
    SGraphicsLineUnitData lineData;
    lineData.point1 = QPointF(10, 20);
    lineData.point2 = QPointF(100, 200);
    lineData.start_type = normalArrow;
    lineData.end_type = soildArrow;
    stream << lineData;

    // Read back: seek past the 8-byte header so getVersion sees it
    buffer.seek(8);
    SGraphicsLineUnitData lineData2;
    stream >> lineData2;

    EXPECT_EQ(lineData2.point1, QPointF(10, 20));
    EXPECT_EQ(lineData2.point2, QPointF(100, 200));
    EXPECT_EQ(lineData2.start_type, normalArrow);
    EXPECT_EQ(lineData2.end_type, soildArrow);
}

// =========================================================================
// SGraphicsLineUnitData serialization — basic round-trip (points only)
// No magic header → getVersion() returns EDdf5_8_0_Base → only point1
// and point2 are read back.
// =========================================================================

TEST(SGraphicsLineUnitDataSerialization, BasicRoundTripPointsOnly)
{
    QBuffer buffer;
    buffer.open(QIODevice::ReadWrite);
    QDataStream stream(&buffer);

    // No magic header — getVersion will return EDdf5_8_0_Base
    SGraphicsLineUnitData lineData;
    lineData.point1 = QPointF(1, 2);
    lineData.point2 = QPointF(3, 4);
    lineData.start_type = soildRing;
    lineData.end_type = normalRing;
    stream << lineData;

    buffer.seek(0);
    SGraphicsLineUnitData lineData2;
    stream >> lineData2;

    EXPECT_EQ(lineData2.point1, QPointF(1, 2));
    EXPECT_EQ(lineData2.point2, QPointF(3, 4));
    // Arrow types not read in Base version (defaults remain)
    EXPECT_EQ(lineData2.start_type, noneLine);
    EXPECT_EQ(lineData2.end_type, noneLine);
}

// =========================================================================
// CGraphicsUnit::deepCopy — TextType (complements existing tests that
// cover RectType, EllipseType, LineType)
// =========================================================================

TEST(CGraphicsUnitDeepCopyTextType, CopiesTextDataAndHead)
{
    CGraphicsUnit source;
    source.head.dataType = TextType;
    source.head.pos = QPointF(50, 50);
    source.head.rotate = 15.0;
    source.data.pText = new SGraphicsTextUnitData;
    source.data.pText->rect.topLeft = QPointF(0, 0);
    source.data.pText->rect.bottomRight = QPointF(200, 100);
    source.data.pText->content = QStringLiteral("Hello, deepin-draw!");
    source.data.pText->font = QFont("Arial", 14);
    source.data.pText->manResizeFlag = true;

    CGraphicsUnit dest;
    CGraphicsUnit::deepCopy(dest, source);

    ASSERT_NE(dest.data.pText, nullptr);
    EXPECT_EQ(dest.head.dataType, TextType);
    EXPECT_EQ(dest.head.pos, QPointF(50, 50));
    EXPECT_DOUBLE_EQ(dest.head.rotate, 15.0);
    EXPECT_EQ(dest.data.pText->content, QStringLiteral("Hello, deepin-draw!"));
    EXPECT_EQ(dest.data.pText->font.family(), QString("Arial"));
    EXPECT_TRUE(dest.data.pText->manResizeFlag);

    // Ensure deep copy — modifying dest should not affect source
    dest.data.pText->content = QStringLiteral("Modified");
    EXPECT_NE(source.data.pText->content, dest.data.pText->content);

    source.release();
    dest.release();
}

// =========================================================================
// CBzGroupTree copy constructor — deep copy of CGraphicsUnit data
// =========================================================================

TEST(CBzGroupTreeCopyConstructor, DeepCopiesGroupData)
{
    CBzGroupTree<int> original;
    original.setGroupType(42);
    original.bzItems.append(1);
    original.bzItems.append(2);

    // Child group with values distinct from the parent's, plus a nested
    // grandchild so a copy that loses nested levels is detectable.
    CBzGroupTree<int> child;
    child.setGroupType(7);
    child.bzItems.append(30);
    child.bzItems.append(40);
    CBzGroupTree<int> grandchild;
    grandchild.setGroupType(8);
    grandchild.bzItems.append(50);
    child.childGroups.append(grandchild);
    original.childGroups.append(child);

    // Copy construct
    CBzGroupTree<int> copy(original);

    // Verify data was deep-copied
    ASSERT_NE(copy.data.data.pGroup, nullptr);
    EXPECT_EQ(copy.data.head.dataType, MgrType);
    EXPECT_EQ(copy.data.data.pGroup->groupType, 42);
    EXPECT_EQ(copy.bzItems.size(), 2);
    EXPECT_EQ(copy.bzItems[0], 1);
    EXPECT_EQ(copy.bzItems[1], 2);

    // The copied child group must carry its own distinct data and the
    // nested grandchild level must survive the copy.
    ASSERT_EQ(copy.childGroups.size(), 1);
    EXPECT_EQ(copy.childGroups.at(0).groupType(), 7);
    ASSERT_EQ(copy.childGroups.at(0).bzItems.size(), 2);
    EXPECT_EQ(copy.childGroups.at(0).bzItems.at(0), 30);
    EXPECT_EQ(copy.childGroups.at(0).bzItems.at(1), 40);
    ASSERT_EQ(copy.childGroups.at(0).childGroups.size(), 1);
    EXPECT_EQ(copy.childGroups.at(0).childGroups.at(0).groupType(), 8);
    ASSERT_EQ(copy.childGroups.at(0).childGroups.at(0).bzItems.size(), 1);
    EXPECT_EQ(copy.childGroups.at(0).childGroups.at(0).bzItems.at(0), 50);

    // Verify deep copy — pointers must differ
    EXPECT_NE(copy.data.data.pGroup, original.data.data.pGroup);

    // Mutate the copy's child subtree; the original subtree must be
    // unaffected and the copy must own its own group data (no aliasing).
    copy.childGroups[0].setGroupType(99);
    copy.childGroups[0].bzItems[0] = 300;
    copy.childGroups[0].bzItems.append(999);
    copy.childGroups[0].childGroups[0].setGroupType(88);

    EXPECT_EQ(copy.childGroups.at(0).groupType(), 99);
    ASSERT_EQ(copy.childGroups.at(0).bzItems.size(), 3);
    EXPECT_EQ(copy.childGroups.at(0).bzItems.at(0), 300);
    EXPECT_EQ(copy.childGroups.at(0).childGroups.at(0).groupType(), 88);
    EXPECT_NE(copy.childGroups.at(0).data.data.pGroup,
              original.childGroups.at(0).data.data.pGroup);
    EXPECT_NE(copy.childGroups.at(0).childGroups.at(0).data.data.pGroup,
              original.childGroups.at(0).childGroups.at(0).data.data.pGroup);
    EXPECT_EQ(original.childGroups.at(0).groupType(), 7);
    ASSERT_EQ(original.childGroups.at(0).bzItems.size(), 2);
    EXPECT_EQ(original.childGroups.at(0).bzItems.at(0), 30);
    EXPECT_EQ(original.childGroups.at(0).bzItems.at(1), 40);
    EXPECT_EQ(original.childGroups.at(0).childGroups.at(0).groupType(), 8);

    // Modify copy's data, original should be unaffected
    copy.setGroupType(55);
    EXPECT_EQ(original.data.data.pGroup->groupType, 42);
    EXPECT_EQ(copy.data.data.pGroup->groupType, 55);
}

// =========================================================================
// CBzGroupTree assignment operator — deep copy of CGraphicsUnit data
// =========================================================================

TEST(CBzGroupTreeAssignmentOperator, DeepCopiesGroupData)
{
    CBzGroupTree<int> original;
    original.setGroupType(7);
    original.bzItems.append(10);

    // Child group with values distinct from the parent's, plus a nested
    // grandchild so an assignment that loses nested levels is detectable.
    CBzGroupTree<int> child;
    child.setGroupType(21);
    child.bzItems.append(60);
    CBzGroupTree<int> grandchild;
    grandchild.setGroupType(22);
    grandchild.bzItems.append(70);
    child.childGroups.append(grandchild);
    original.childGroups.append(child);

    CBzGroupTree<int> assigned;
    assigned = original;

    ASSERT_NE(assigned.data.data.pGroup, nullptr);
    EXPECT_EQ(assigned.data.head.dataType, MgrType);
    EXPECT_EQ(assigned.data.data.pGroup->groupType, 7);
    EXPECT_EQ(assigned.bzItems.size(), 1);
    EXPECT_EQ(assigned.bzItems[0], 10);

    // The assigned child subtree must carry its distinct data and the
    // nested grandchild level must survive the assignment.
    ASSERT_EQ(assigned.childGroups.size(), 1);
    EXPECT_EQ(assigned.childGroups.at(0).groupType(), 21);
    ASSERT_EQ(assigned.childGroups.at(0).bzItems.size(), 1);
    EXPECT_EQ(assigned.childGroups.at(0).bzItems.at(0), 60);
    ASSERT_EQ(assigned.childGroups.at(0).childGroups.size(), 1);
    EXPECT_EQ(assigned.childGroups.at(0).childGroups.at(0).groupType(), 22);
    ASSERT_EQ(assigned.childGroups.at(0).childGroups.at(0).bzItems.size(), 1);
    EXPECT_EQ(assigned.childGroups.at(0).childGroups.at(0).bzItems.at(0), 70);

    // Deep copy: different pointers
    EXPECT_NE(assigned.data.data.pGroup, original.data.data.pGroup);

    // Mutate the assigned child subtree; the original subtree must be
    // unaffected and the assigned tree must own its own group data.
    assigned.childGroups[0].setGroupType(33);
    assigned.childGroups[0].bzItems[0] = 600;
    assigned.childGroups[0].childGroups[0].setGroupType(44);

    EXPECT_EQ(assigned.childGroups.at(0).groupType(), 33);
    EXPECT_EQ(assigned.childGroups.at(0).bzItems.at(0), 600);
    EXPECT_EQ(assigned.childGroups.at(0).childGroups.at(0).groupType(), 44);
    EXPECT_NE(assigned.childGroups.at(0).data.data.pGroup,
              original.childGroups.at(0).data.data.pGroup);
    EXPECT_NE(assigned.childGroups.at(0).childGroups.at(0).data.data.pGroup,
              original.childGroups.at(0).childGroups.at(0).data.data.pGroup);
    EXPECT_EQ(original.childGroups.at(0).groupType(), 21);
    EXPECT_EQ(original.childGroups.at(0).bzItems.at(0), 60);
    EXPECT_EQ(original.childGroups.at(0).childGroups.at(0).groupType(), 22);

    // Modify assigned, original unchanged
    assigned.setGroupType(100);
    EXPECT_EQ(original.data.data.pGroup->groupType, 7);
    EXPECT_EQ(assigned.data.data.pGroup->groupType, 100);
}
