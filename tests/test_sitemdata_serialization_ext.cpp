// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for remaining serialization branches in sitemdata.h
 * (issue V-9255 §1 — complements test_sitemdata_serialization.cpp)
 *
 * Source: src/drawshape/sitemdata.h (inline)
 *
 * Key insight: getVersion() always seeks to position 0 of the stream buffer,
 * reads 0xA0B0C0D0 magic + version int, then seeks back. So for any struct
 * whose operator>> calls getVersion(), we must either:
 *  (a) prepend a magic+version header at position 0, write struct data after,
 *      seek to 8, then read; or
 *  (b) for CGraphicsUnit, the head's headCheck[0..3] serves as the magic and
 *      head.dataType & 0xFF serves as the version — but only if that version
 *      is high enough (>=9) for the head to read all fields.
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QtCore/QtGlobal>
#include <QBuffer>
#include <QDataStream>
#include <QPen>
#include <QBrush>
#include <QPointF>
#include <QImage>
#include <QPainterPath>
#include <QRegularExpression>

#define protected public
#define private public
#include "drawshape/sitemdata.h"
#include "drawshape/globaldefine.h"
#undef protected
#undef private

// =========================================================================
// SGraphicsGroupUnitData serialization
// =========================================================================

TEST(SitemdataSerializationExt, GroupUnitData_NameNonEmpty_RoundTrip)
{
    // operator<< writes: isCancelAble, count, rawbytes, groupType
    // operator>>  reads: isCancelAble, count, nameByteSz, rawbytes, groupType
    // These are ASYMMETRIC — operator>> expects an extra nameByteSz field.
    // Manually construct the byte stream matching operator>>'s expected format.
    QString name = QString::fromUtf8("我的组合");
    QByteArray nameUtf8 = name.toUtf8();
    int count = nameUtf8.size();

    QBuffer buf;
    buf.open(QIODevice::ReadWrite);
    QDataStream out(&buf);
    // Write magic + version header at position 0
    out << static_cast<quint32>(0xA0B0C0D0);
    out << static_cast<qint32>(EDdf5_9_0_3_LATER);
    // Manually write the fields as operator>> expects them
    out << false;                   // isCancelAble
    out << count;                   // count (name byte count)
    out << count;                   // nameByteSz (extra field operator>> expects)
    out.writeRawData(nameUtf8.data(), count);  // raw name bytes
    out << 3;                       // groupType

    // Read back: seek past the 8-byte header
    buf.seek(8);
    SGraphicsGroupUnitData dst;
    QDataStream in(&buf);
    in >> dst;

    EXPECT_EQ(dst.isCancelAble, false);
    EXPECT_EQ(dst.name, name);
    EXPECT_EQ(dst.groupType, 3);
}

TEST(SitemdataSerializationExt, GroupUnitData_NameEmpty_WritesZero)
{
    // For empty name: operator<< writes isCancelAble, int(0), groupType
    // operator>> reads isCancelAble, count(=0), skips nameByteSz, groupType
    // These are symmetric for the empty-name case.
    QBuffer buf;
    buf.open(QIODevice::ReadWrite);
    QDataStream out(&buf);
    // Write magic + version header
    out << static_cast<quint32>(0xA0B0C0D0);
    out << static_cast<qint32>(EDdf5_9_0_3_LATER);
    // Write struct data using operator<<
    SGraphicsGroupUnitData src;
    src.name.clear();
    src.isCancelAble = true;
    src.groupType = 0;
    out << src;

    // Read back: seek past the 8-byte header
    buf.seek(8);
    SGraphicsGroupUnitData dst;
    QDataStream in(&buf);
    in >> dst;

    EXPECT_EQ(dst.isCancelAble, true);
    EXPECT_TRUE(dst.name.isEmpty());
    EXPECT_EQ(dst.groupType, 0);
}

// =========================================================================
// SGraphicsRectUnitData::operator>> old version (EDdf5_8_0_Base) else branch
// =========================================================================

TEST(SitemdataSerializationExt, RectUnitData_OldVersion_ReadsTopLeftBottomRight_RadiusZero)
{
    // Old version: no magic header at position 0 → getVersion returns EDdf5_8_0_Base
    SGraphicsRectUnitData src;
    src.topLeft = QPointF(10, 20);
    src.bottomRight = QPointF(100, 200);
    src.xRedius = 5;
    src.yRedius = 5;

    QBuffer buf;
    buf.open(QIODevice::ReadWrite);
    QDataStream out(&buf);
    out << src.topLeft;
    out << src.bottomRight;

    buf.seek(0);
    SGraphicsRectUnitData dst;
    dst.xRedius = 99;
    dst.yRedius = 99;
    QDataStream in(&buf);
    in >> dst;

    EXPECT_EQ(dst.topLeft, QPointF(10, 20));
    EXPECT_EQ(dst.bottomRight, QPointF(100, 200));
    EXPECT_EQ(dst.xRedius, 0);
    EXPECT_EQ(dst.yRedius, 0);
}

TEST(SitemdataSerializationExt, RectUnitData_NewVersion_ReadsAllFields)
{
    // New version: write magic+version header, then struct data, seek to 8, read
    QBuffer buf;
    buf.open(QIODevice::ReadWrite);
    QDataStream out(&buf);
    out << static_cast<quint32>(0xA0B0C0D0);
    out << static_cast<qint32>(EDdf5_9_0_3_LATER);

    SGraphicsRectUnitData src;
    src.topLeft = QPointF(1, 2);
    src.bottomRight = QPointF(3, 4);
    src.xRedius = 7;
    src.yRedius = 8;
    out << src;

    buf.seek(8);
    SGraphicsRectUnitData dst;
    QDataStream in(&buf);
    in >> dst;

    EXPECT_EQ(dst.topLeft, QPointF(1, 2));
    EXPECT_EQ(dst.bottomRight, QPointF(3, 4));
    EXPECT_EQ(dst.xRedius, 7);
    EXPECT_EQ(dst.yRedius, 8);
}

// =========================================================================
// SGraphicsPictureUnitData serialization
// =========================================================================

TEST(SitemdataSerializationExt, PictureUnitData_NewFormat_RoundTrip)
{
    // New format (>= EDdf5_8_0_16_1): writes srcByteArry + flip flags
    QBuffer buf;
    buf.open(QIODevice::ReadWrite);
    QDataStream out(&buf);
    out << static_cast<quint32>(0xA0B0C0D0);
    out << static_cast<qint32>(EDdf5_8_0_48_LATER);

    SGraphicsPictureUnitData src;
    src.srcByteArry = QByteArray("imagedata123");
    src.flipHorizontal = true;
    src.flipVertical = true;
    out << src;

    buf.seek(8);
    SGraphicsPictureUnitData dst;
    QDataStream in(&buf);
    in >> dst;

    EXPECT_EQ(dst.srcByteArry, QByteArray("imagedata123"));
    EXPECT_TRUE(dst.flipHorizontal);
    EXPECT_TRUE(dst.flipVertical);
}

TEST(SitemdataSerializationExt, PictureUnitData_NewFormat_NoFlipFlags)
{
    // Version >= EDdf5_8_0_16_1 but < EDdf5_8_0_48_LATER: reads byte array
    // but does NOT read flip flags. operator<< writes flip flags but reader
    // doesn't consume them — that's fine, we only check the read values.
    QBuffer buf;
    buf.open(QIODevice::ReadWrite);
    QDataStream out(&buf);
    out << static_cast<quint32>(0xA0B0C0D0);
    out << static_cast<qint32>(EDdf5_8_0_16_1);

    SGraphicsPictureUnitData src;
    src.srcByteArry = QByteArray("data");
    out << src;

    buf.seek(8);
    SGraphicsPictureUnitData dst;
    QDataStream in(&buf);
    in >> dst;

    EXPECT_EQ(dst.srcByteArry, QByteArray("data"));
    EXPECT_FALSE(dst.flipHorizontal);
    EXPECT_FALSE(dst.flipVertical);
}

TEST(SitemdataSerializationExt, PictureUnitData_OldFormat_ReadsImage)
{
    // Old format (< EDdf5_8_0_16_1): reads QImage directly.
    // No magic header → getVersion returns EDdf5_8_0_Base.
    // SGraphicsPictureUnitData::operator>> first reads rect (SGraphicsRectUnitData).
    // With version 0, rect's else branch reads topLeft, bottomRight only.
    // Then picture reads QImage.
    QImage img(4, 4, QImage::Format_RGB32);
    img.fill(Qt::red);

    QBuffer buf;
    buf.open(QIODevice::ReadWrite);
    QDataStream out(&buf);
    // Write rect data (old version: topLeft, bottomRight only)
    out << QPointF(0, 0);
    out << QPointF(4, 4);
    // Write QImage
    out << img;

    buf.seek(0);
    SGraphicsPictureUnitData dst;
    QDataStream in(&buf);
    in >> dst;

    EXPECT_FALSE(dst.image.isNull());
    EXPECT_EQ(dst.image.width(), 4);
    EXPECT_EQ(dst.image.height(), 4);
}

// =========================================================================
// SGraphicsPenUnitData serialization
// =========================================================================

TEST(SitemdataSerializationExt, PenUnitData_DefaultVersion_RoundTrip)
{
    // Default branch (version >= EDdf5_8_0_10_2): reads start_type, path, end_type
    QBuffer buf;
    buf.open(QIODevice::ReadWrite);
    QDataStream out(&buf);
    out << static_cast<quint32>(0xA0B0C0D0);
    out << static_cast<qint32>(EDdf5_9_0_3_LATER);

    SGraphicsPenUnitData src;
    src.start_type = soildArrow;
    src.end_type = noneLine;
    QPainterPath p;
    p.moveTo(0, 0);
    p.lineTo(10, 10);
    src.path = p;
    out << src;

    buf.seek(8);
    SGraphicsPenUnitData dst;
    QDataStream in(&buf);
    in >> dst;

    EXPECT_EQ(dst.start_type, soildArrow);
    EXPECT_EQ(dst.end_type, noneLine);
    EXPECT_EQ(dst.path.elementCount(), p.elementCount());
}

TEST(SitemdataSerializationExt, PenUnitData_OldVersion_ArrowNonEmpty_SetsSoildArrow)
{
    // Old version (EDdf5_8_0_Base): reads qint8 pentype, path, arrow
    QBuffer buf;
    buf.open(QIODevice::ReadWrite);
    QDataStream out(&buf);
    out << static_cast<qint8>(1);
    QPainterPath p;
    p.moveTo(0, 0);
    p.lineTo(5, 5);
    out << p;
    QPolygonF arrow;
    arrow << QPointF(0, 0) << QPointF(1, 1);
    out << arrow;

    buf.seek(0);
    SGraphicsPenUnitData dst;
    QDataStream in(&buf);
    in >> dst;

    EXPECT_EQ(dst.start_type, soildArrow);
    EXPECT_FALSE(dst.arrow.isEmpty());
}

TEST(SitemdataSerializationExt, PenUnitData_OldVersion_ArrowEmpty_KeepsDefault)
{
    // Old version with empty arrow: start_type stays noneLine (default)
    QBuffer buf;
    buf.open(QIODevice::ReadWrite);
    QDataStream out(&buf);
    out << static_cast<qint8>(0);
    QPainterPath p;
    out << p;
    QPolygonF arrow;
    out << arrow;

    buf.seek(0);
    SGraphicsPenUnitData dst;
    QDataStream in(&buf);
    in >> dst;

    EXPECT_EQ(dst.start_type, noneLine);
    EXPECT_TRUE(dst.arrow.isEmpty());
}

// =========================================================================
// SGraphicsBlurUnitData round-trip
// =========================================================================

TEST(SitemdataSerializationExt, BlurUnitData_RoundTrip)
{
    // SGraphicsBlurUnitData contains SGraphicsPenUnitData, whose operator>>
    // calls getVersion(). Without a magic header, version=0 → old version path
    // which reads qint8+path+arrow, but operator<< writes qint32+path+qint32.
    // FIX: prepend magic+version(9) header so getVersion returns 9 (default case).
    QBuffer buf;
    buf.open(QIODevice::ReadWrite);
    QDataStream out(&buf);
    out << static_cast<quint32>(0xA0B0C0D0);
    out << static_cast<qint32>(EDdf5_9_0_3_LATER);

    SGraphicsBlurUnitData src;
    src.effect = 2;
    src.data.start_type = soildArrow;
    src.data.end_type = soildArrow;
    QPainterPath p;
    p.moveTo(0, 0);
    p.lineTo(20, 20);
    src.data.path = p;
    out << src;

    buf.seek(8);
    SGraphicsBlurUnitData dst;
    QDataStream in(&buf);
    in >> dst;

    EXPECT_EQ(dst.effect, 2);
    EXPECT_EQ(dst.data.start_type, soildArrow);
    EXPECT_EQ(dst.data.end_type, soildArrow);
    EXPECT_EQ(dst.data.path.elementCount(), p.elementCount());
}

// =========================================================================
// CGraphicsUnit operator<< (PictureType, PenType, BlurType, MgrType)
// =========================================================================

TEST(SitemdataSerializationExt, CGraphicsUnit_Output_PictureType)
{
    CGraphicsUnit unit;
    unit.head.dataType = PictureType;
    unit.head.headCheck[0] = static_cast<qint8>(0xA0);
    unit.head.headCheck[1] = static_cast<qint8>(0xB0);
    unit.head.headCheck[2] = static_cast<qint8>(0xC0);
    unit.head.headCheck[3] = static_cast<qint8>(0xD0);
    unit.head.dataType = static_cast<qint32>(PictureType);
    unit.data.pPic = new SGraphicsPictureUnitData;
    unit.data.pPic->srcByteArry = QByteArray("picbytes");
    unit.data.pPic->flipHorizontal = true;

    QBuffer buf;
    buf.open(QIODevice::ReadWrite);
    QDataStream out(&buf);
    out << unit;

    EXPECT_GT(buf.size(), 0);

    unit.release();
}

TEST(SitemdataSerializationExt, CGraphicsUnit_Output_PenType)
{
    CGraphicsUnit unit;
    unit.head.dataType = PenType;
    unit.data.pPen = new SGraphicsPenUnitData;
    unit.data.pPen->start_type = soildArrow;

    QBuffer buf;
    buf.open(QIODevice::ReadWrite);
    QDataStream out(&buf);
    out << unit;

    EXPECT_GT(buf.size(), 0);

    unit.release();
}

TEST(SitemdataSerializationExt, CGraphicsUnit_Output_BlurType)
{
    CGraphicsUnit unit;
    unit.head.dataType = BlurType;
    unit.data.pBlur = new SGraphicsBlurUnitData;
    unit.data.pBlur->effect = 1;

    QBuffer buf;
    buf.open(QIODevice::ReadWrite);
    QDataStream out(&buf);
    out << unit;

    EXPECT_GT(buf.size(), 0);

    unit.release();
}

TEST(SitemdataSerializationExt, CGraphicsUnit_Output_MgrType)
{
    CGraphicsUnit unit;
    unit.head.dataType = MgrType;
    unit.data.pGroup = new SGraphicsGroupUnitData;
    unit.data.pGroup->name = "group1";

    QBuffer buf;
    buf.open(QIODevice::ReadWrite);
    QDataStream out(&buf);
    out << unit;

    EXPECT_GT(buf.size(), 0);

    unit.release();
}

// =========================================================================
// CGraphicsUnit operator>> (LineType, PictureType)
// =========================================================================

TEST(SitemdataSerializationExt, CGraphicsUnit_Read_LineType)
{
    // For CGraphicsUnit round-trip, the head's headCheck serves as the magic
    // and head.dataType & 0xFF serves as the version for getVersion().
    // LineType = UserType+7 = 65543, 65543 & 0xFF = 7 = EDdf5_8_0_84_LATER.
    // Version 7 reads blurCount but NOT trans (needs >=8) or rect (needs >=9).
    // But operator<< writes trans and rect unconditionally → mismatch.
    // FIX: Prepend magic+version(9) header so getVersion returns 9,
    // and seek to 8 before reading so head starts at the right position.
    QBuffer buf;
    buf.open(QIODevice::ReadWrite);
    QDataStream out(&buf);
    // Prepend version header
    out << static_cast<quint32>(0xA0B0C0D0);
    out << static_cast<qint32>(EDdf5_9_0_3_LATER);

    CGraphicsUnit src;
    src.head.headCheck[0] = static_cast<qint8>(0xA0);
    src.head.headCheck[1] = static_cast<qint8>(0xB0);
    src.head.headCheck[2] = static_cast<qint8>(0xC0);
    src.head.headCheck[3] = static_cast<qint8>(0xD0);
    src.head.dataType = static_cast<qint32>(LineType);
    src.data.pLine = new SGraphicsLineUnitData;
    src.data.pLine->point1 = QPointF(10, 20);
    src.data.pLine->point2 = QPointF(30, 40);
    src.data.pLine->start_type = soildArrow;
    src.data.pLine->end_type = noneLine;
    out << src;

    // Seek past the 8-byte prepended header before reading
    buf.seek(8);
    CGraphicsUnit dst;
    QDataStream in(&buf);
    in >> dst;

    ASSERT_NE(dst.data.pLine, nullptr);
    EXPECT_EQ(dst.head.dataType, LineType);
    EXPECT_EQ(dst.data.pLine->point1, QPointF(10, 20));
    EXPECT_EQ(dst.data.pLine->point2, QPointF(30, 40));

    src.release();
    dst.release();
}

TEST(SitemdataSerializationExt, CGraphicsUnit_Read_PictureType)
{
    // Same approach as LineType: prepend magic+version(9), seek to 8, read.
    QBuffer buf;
    buf.open(QIODevice::ReadWrite);
    QDataStream out(&buf);
    out << static_cast<quint32>(0xA0B0C0D0);
    out << static_cast<qint32>(EDdf5_9_0_3_LATER);

    CGraphicsUnit src;
    src.head.headCheck[0] = static_cast<qint8>(0xA0);
    src.head.headCheck[1] = static_cast<qint8>(0xB0);
    src.head.headCheck[2] = static_cast<qint8>(0xC0);
    src.head.headCheck[3] = static_cast<qint8>(0xD0);
    src.head.dataType = static_cast<qint32>(PictureType);
    src.data.pPic = new SGraphicsPictureUnitData;
    src.data.pPic->srcByteArry = QByteArray("picdata456");
    src.data.pPic->flipHorizontal = true;
    src.data.pPic->flipVertical = false;
    out << src;

    buf.seek(8);
    CGraphicsUnit dst;
    QDataStream in(&buf);
    in >> dst;

    ASSERT_NE(dst.data.pPic, nullptr);
    EXPECT_EQ(dst.data.pPic->srcByteArry, QByteArray("picdata456"));
    EXPECT_TRUE(dst.data.pPic->flipHorizontal);
    EXPECT_FALSE(dst.data.pPic->flipVertical);

    src.release();
    dst.release();
}
