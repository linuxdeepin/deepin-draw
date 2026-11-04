// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for sitemdata.h serialization structs
 *
 * Source: src/drawshape/sitemdata.h
 *
 * Target: SGraphicsUnitHead, SGraphicsLineUnitData,
 *         CGraphicsUnit::deepCopy/release, CBzGroupTree
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QBuffer>
#include <QDataStream>

#define protected public
#define private public
#include "drawshape/sitemdata.h"
#include "drawshape/globaldefine.h"
#undef protected
#undef private

// ddf 文件头布局与 CGraphics::operator<< 完全一致。
// 各结构体的反序列化都会调用 getVersion() 回到流开头读取版本号，
// 因此每个用例都必须先写入该文件头，再写入待测的数据体。
static void writeDdfHeader(QDataStream &out)
{
    out << static_cast<quint32>(0xA0B0C0D0);
    out << int(EDdfCurVersion);
    out << qint64(0);
    out << QRectF();
}

// 把 header + 数据体序列化为字节数组
static QByteArray serializePayload(const std::function<void(QDataStream &)> &payload)
{
    QBuffer buf;
    buf.open(QIODevice::ReadWrite);
    QDataStream out(&buf);
    writeDdfHeader(out);
    payload(out);
    return buf.data();
}

// 持有缓冲区与读取流；consumeHeader 消耗掉文件头，模拟真实 ddf 加载顺序
struct DdfReader
{
    QBuffer buf;
    QDataStream stream{&buf};

    explicit DdfReader(const QByteArray &data)
    {
        buf.open(QIODevice::ReadWrite);
        buf.write(data);
        buf.seek(0);
    }

    void consumeHeader()
    {
        CGraphics g;
        stream >> g;
    }
};

// =========================================================================
// SGraphicsUnitHead
// =========================================================================


TEST(SGraphicsUnitHeadTest, RoundTrip_WithoutBlur_PreservesFields)
{
    SGraphicsUnitHead head;
    head.headCheck[0] = 'H';
    head.dataType = RectType;
    head.dataLength = 128;
    head.pen = QPen(QColor(10, 20, 30), 3);
    head.brush = QBrush(QColor(40, 50, 60));
    head.pos = QPointF(11, 22);
    head.rotate = 33.5;
    head.zValue = 2.5;
    head.trans = QTransform(1, 0, 0, 1, 5, 6);
    head.rect = QRectF(0, 0, 100, 80);

    QByteArray data = serializePayload([&](QDataStream &out) { out << head; });
    DdfReader reader(data);
    reader.consumeHeader();

    SGraphicsUnitHead loaded;
    reader.stream >> loaded;

    EXPECT_EQ(loaded.headCheck[0], 'H');
    EXPECT_EQ(loaded.dataType, RectType);
    EXPECT_EQ(loaded.dataLength, qint64(128));
    EXPECT_EQ(loaded.pen.color(), QColor(10, 20, 30));
    EXPECT_EQ(loaded.brush.color(), QColor(40, 50, 60));
    EXPECT_EQ(loaded.pos, QPointF(11, 22));
    EXPECT_DOUBLE_EQ(loaded.rotate, 33.5);
    EXPECT_DOUBLE_EQ(loaded.zValue, 2.5);
    EXPECT_EQ(loaded.blurCount, 0);
    EXPECT_EQ(loaded.trans.dx(), 5);
    EXPECT_EQ(loaded.rect, QRectF(0, 0, 100, 80));
}

TEST(SGraphicsUnitHeadTest, RoundTrip_WithBlurs_PreservesBlurInfos)
{
    SGraphicsUnitHead head;
    head.dataType = BlurType;
    head.blurCount = 2;

    SBlurInfo b1;
    b1.blurPath.addRect(0, 0, 5, 5);
    b1.blurEfTp = BlurEffect;
    SBlurInfo b2;
    b2.blurPath.addRect(1, 1, 6, 6);
    b2.blurEfTp = MasicoEffect;
    head.blurInfos << b1 << b2;

    QByteArray data = serializePayload([&](QDataStream &out) { out << head; });
    DdfReader reader(data);
    reader.consumeHeader();

    SGraphicsUnitHead loaded;
    reader.stream >> loaded;

    ASSERT_EQ(loaded.blurCount, 2);
    ASSERT_EQ(loaded.blurInfos.size(), 2);
    EXPECT_EQ(loaded.blurInfos[0].blurEfTp, BlurEffect);
    EXPECT_EQ(loaded.blurInfos[1].blurEfTp, MasicoEffect);
    EXPECT_EQ(loaded.blurInfos[1].blurPath.boundingRect(), QRectF(1, 1, 6, 6));
}

// =========================================================================
// SGraphicsLineUnitData
// =========================================================================


TEST(SGraphicsLineUnitDataTest, RoundTrip_PreservesPointsAndArrowTypes)
{
    SGraphicsLineUnitData line;
    line.point1 = QPointF(7, 8);
    line.point2 = QPointF(70, 80);
    line.start_type = normalArrow;
    line.end_type = soildRing;

    QByteArray data = serializePayload([&](QDataStream &out) { out << line; });
    DdfReader reader(data);
    reader.consumeHeader();

    SGraphicsLineUnitData loaded;
    reader.stream >> loaded;

    EXPECT_EQ(loaded.point1, QPointF(7, 8));
    EXPECT_EQ(loaded.point2, QPointF(70, 80));
    EXPECT_EQ(loaded.start_type, normalArrow);
    EXPECT_EQ(loaded.end_type, soildRing);
}


// =========================================================================
// CGraphicsUnit::deepCopy / release 与 CBzGroupTree
// =========================================================================

TEST(CGraphicsUnitTest, DeepCopy_Line_ClonesData)
{
    CGraphicsUnit src;
    src.head.dataType = LineType;
    src.data.pLine = new SGraphicsLineUnitData;
    src.data.pLine->point1 = QPointF(1, 1);
    src.data.pLine->point2 = QPointF(9, 9);
    src.data.pLine->start_type = normalArrow;

    CGraphicsUnit des;
    CGraphicsUnit::deepCopy(des, src);

    ASSERT_NE(des.data.pLine, nullptr);
    EXPECT_NE(des.data.pLine, src.data.pLine);
    EXPECT_EQ(des.data.pLine->point2, QPointF(9, 9));
    EXPECT_EQ(des.data.pLine->start_type, normalArrow);

    des.release();
    src.release();
}


TEST(CBzGroupTreeTest, CopyConstructor_PerformsDeepCopy)
{
    CBzGroupTree<CGraphicsUnit> tree;
    ASSERT_NE(tree.data.data.pGroup, nullptr);
    EXPECT_EQ(tree.data.head.dataType, MgrType);

    tree.setGroupType(4);
    tree.data.data.pGroup->name = QStringLiteral("图层组");

    CBzGroupTree<CGraphicsUnit> copied(tree);

    ASSERT_NE(copied.data.data.pGroup, nullptr);
    EXPECT_NE(copied.data.data.pGroup, tree.data.data.pGroup);
    EXPECT_EQ(copied.groupType(), 4);
    EXPECT_EQ(copied.data.data.pGroup->name, QStringLiteral("图层组"));
}
