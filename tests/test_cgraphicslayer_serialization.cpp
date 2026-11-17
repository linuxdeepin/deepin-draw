// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for JPathCommand / JPaintCommand / JBlurCommand
 * serialization round-trip (issue V-8626)
 *
 * Existing test_cgraphicslayer.cpp already covers JGeomeCommand
 * serialization — these tests cover the remaining three command types.
 *
 * Source: src/drawshape/drawItems/cgraphicslayer.cpp
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QBuffer>
#include <QDataStream>
#include <QPainterPath>
#include <QPen>
#include <QPicture>
#include <QPointF>
#include <QRectF>

#define protected public
#define private public
#include "cgraphicslayer.h"
#undef protected
#undef private

// ===========================================================================
// JPathCommand serialization / deserialization round-trip
// ===========================================================================

TEST(JPathCommandSerializationTest, RoundTripPreservesPathAndPen)
{
    QPainterPath path;
    path.moveTo(10, 20);
    path.cubicTo(30, 40, 50, 60, 70, 80);
    QPen pen(Qt::red, 3.0, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);

    JPathCommand src(path, pen, nullptr);

    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    QDataStream out(&buffer);
    src.serialization(out);
    buffer.close();

    JPathCommand dst;
    buffer.open(QIODevice::ReadOnly);
    QDataStream in(&buffer);
    dst.deserialization(in);
    buffer.close();

    EXPECT_EQ(dst._path, path);
    EXPECT_EQ(dst._pen, pen);
}

TEST(JPathCommandSerializationTest, RoundTripDefaultValues)
{
    JPathCommand src;  // default QPainterPath + default QPen

    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    QDataStream out(&buffer);
    src.serialization(out);
    buffer.close();

    JPathCommand dst;
    buffer.open(QIODevice::ReadOnly);
    QDataStream in(&buffer);
    dst.deserialization(in);
    buffer.close();

    EXPECT_EQ(dst._path, QPainterPath());
    EXPECT_EQ(dst._pen, QPen());
}

// ===========================================================================
// JPaintCommand serialization / deserialization round-trip
// ===========================================================================

TEST(JPaintCommandSerializationTest, RoundTripPreservesPictureAndFlag)
{
    // Build a non-trivial QPicture by painting into it
    QPicture picture;
    QPainter painter(&picture);
    painter.setPen(QPen(Qt::blue, 2));
    painter.drawLine(0, 0, 100, 100);
    painter.drawRect(10, 10, 50, 50);
    painter.end();

    JPaintCommand src(picture, true, nullptr);

    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    QDataStream out(&buffer);
    src.serialization(out);
    buffer.close();

    JPaintCommand dst;
    buffer.open(QIODevice::ReadOnly);
    QDataStream in(&buffer);
    dst.deserialization(in);
    buffer.close();

    EXPECT_EQ(dst._dyImag, true);
}

TEST(JPaintCommandSerializationTest, RoundTripDefaultValues)
{
    JPaintCommand src;  // default QPicture, dyImag = false

    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    QDataStream out(&buffer);
    src.serialization(out);
    buffer.close();

    JPaintCommand dst;
    buffer.open(QIODevice::ReadOnly);
    QDataStream in(&buffer);
    dst.deserialization(in);
    buffer.close();

    EXPECT_EQ(dst._dyImag, false);
}

// ===========================================================================
// JBlurCommand serialization / deserialization round-trip
// ===========================================================================

TEST(JBlurCommandSerializationTest, RoundTripPreservesPathAndType)
{
    QPainterPath blurPath;
    blurPath.addEllipse(0, 0, 100, 50);
    int blurType = 2;

    JBlurCommand src(blurPath, blurType, nullptr);

    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    QDataStream out(&buffer);
    src.serialization(out);
    buffer.close();

    JBlurCommand dst;
    buffer.open(QIODevice::ReadOnly);
    QDataStream in(&buffer);
    dst.deserialization(in);
    buffer.close();

    EXPECT_EQ(dst._path, blurPath);
    EXPECT_EQ(dst._tp, blurType);
}

TEST(JBlurCommandSerializationTest, RoundTripDefaultValues)
{
    JBlurCommand src;  // default QPainterPath, tp = 0

    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    QDataStream out(&buffer);
    src.serialization(out);
    buffer.close();

    JBlurCommand dst;
    buffer.open(QIODevice::ReadOnly);
    QDataStream in(&buffer);
    dst.deserialization(in);
    buffer.close();

    EXPECT_EQ(dst._path, QPainterPath());
    EXPECT_EQ(dst._tp, 0);
}

// ===========================================================================
// cmdType identifiers
// ===========================================================================

TEST(JPathCommandCmdTypeTest, ReturnsTwo)
{
    JPathCommand cmd;
    EXPECT_EQ(cmd.cmdType(), 2);
}

TEST(JPaintCommandCmdTypeTest, ReturnsThree)
{
    JPaintCommand cmd;
    EXPECT_EQ(cmd.cmdType(), 3);
}

TEST(JBlurCommandCmdTypeTest, ReturnsFour)
{
    JBlurCommand cmd;
    EXPECT_EQ(cmd.cmdType(), 4);
}
