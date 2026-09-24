// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QImage>
#include <QPainterPath>
#include <QPen>
#include <QBrush>
#include <QPicture>
#include <QTransform>
#include <QRectF>
#include <QPointF>
#include <QVariant>
#include <QBuffer>
#include <QDataStream>

#include <memory>

#define protected public
#define private public
#include "publicApi.h"
#include "cgraphicslayer.h"
#undef protected
#undef private

/*
 * CGraphicsLayer / JDynamicLayer / JActivedPaintInfo / JGeomeCommand
 * unit tests (V-5185)
 *
 * The methods listed in the issue span four classes defined in
 * cgraphicslayer.cpp:
 *   CGraphicsLayer      — rect, removeCItem
 *   JDynamicLayer       — boundingRect, contains, clear, image, type,
 *                         setAttributionVar, addPenPath
 *   JActivedPaintInfo   — picture, painter, beginSubPicture,
 *                         endSubPicture, addSubPicture, getPathPicture
 *   JDyLayerCmdBase     — creatCmd (tested via JGeomeCommand)
 *   JGeomeCommand       — constructor, cmdType, serialization
 *
 * All objects are constructed directly (no application context needed).
 */

// Helper: create a 100×100 white ARGB image for JDynamicLayer construction.
static QImage makeTestImage()
{
    QImage img(100, 100, QImage::Format_ARGB32);
    img.fill(Qt::white);
    return img;
}

// ===========================================================================
// CGraphicsLayer
// ===========================================================================

// ---------------------------------------------------------------------------
// rect / setRect
// ---------------------------------------------------------------------------

TEST(CGraphicsLayerRectTest, SetRectAndGetRect)
{
    auto layer = std::make_unique<CGraphicsLayer>();
    layer->setRect(QRectF(10, 20, 100, 50));
    EXPECT_EQ(layer->rect(), QRectF(10, 20, 100, 50));
}

// ---------------------------------------------------------------------------
// removeCItem
// ---------------------------------------------------------------------------

TEST(CGraphicsLayerRemoveCItemTest, RemoveNullItemDoesNotCrash)
{
    auto layer = std::make_unique<CGraphicsLayer>();
    // Removing a null pointer should be a safe no-op.
    layer->removeCItem(nullptr);
}

// ===========================================================================
// JDynamicLayer
// ===========================================================================

// ---------------------------------------------------------------------------
// boundingRect
// ---------------------------------------------------------------------------

// Shared fixture: a JDynamicLayer backed by a 100x100 test image.
class JDynamicLayerTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        layer = std::make_unique<JDynamicLayer>(makeTestImage(), JDynamicLayer::EPenType, nullptr);
    }

    std::unique_ptr<JDynamicLayer> layer;
};

TEST_F(JDynamicLayerTest, BoundingRectMatchesImageSize)
{
    // The constructor initializes the layer rect to the image dimensions.
    EXPECT_EQ(layer->boundingRect(), QRectF(0, 0, 100, 100));
}

// ---------------------------------------------------------------------------
// contains
// ---------------------------------------------------------------------------

TEST_F(JDynamicLayerTest, ContainsPointInsideAndOutside)
{
    EXPECT_TRUE(layer->contains(QPointF(50, 50)));
    EXPECT_FALSE(layer->contains(QPointF(-100, -100)));
}

// ---------------------------------------------------------------------------
// clear
// ---------------------------------------------------------------------------

TEST_F(JDynamicLayerTest, ClearDoesNotCrash)
{
    layer->clear();
}

// ---------------------------------------------------------------------------
// image
// ---------------------------------------------------------------------------

TEST_F(JDynamicLayerTest, ReturnsValidImage)
{
    QImage result = layer->image();
    // The returned image should match the input dimensions.
    EXPECT_FALSE(result.isNull());
    EXPECT_EQ(result.size(), QSize(100, 100));
}

// ---------------------------------------------------------------------------
// type
// ---------------------------------------------------------------------------

TEST_F(JDynamicLayerTest, ReturnsNonNegativeType)
{
    EXPECT_GE(layer->type(), 0);
}

// ---------------------------------------------------------------------------
// setAttributionVar
// ---------------------------------------------------------------------------

TEST_F(JDynamicLayerTest, SetAttributionVarDoesNotCrash)
{
    layer->setAttributionVar(0, QVariant(42), 0);
}

// ---------------------------------------------------------------------------
// addPenPath
// ---------------------------------------------------------------------------

TEST_F(JDynamicLayerTest, AddPenPathDoesNotCrash)
{
    QPainterPath path;
    path.moveTo(10, 10);
    path.lineTo(90, 90);
    QPen pen(Qt::black, 2);

    layer->addPenPath(path, pen, 0, false);
}

// ===========================================================================
// JActivedPaintInfo
// ===========================================================================

// ---------------------------------------------------------------------------
// picture
// ---------------------------------------------------------------------------

TEST(JActivedPaintInfoPictureTest, ReturnsPictureWithoutCrash)
{
    JActivedPaintInfo info;
    (void)info.picture();
}

// ---------------------------------------------------------------------------
// painter
// ---------------------------------------------------------------------------

TEST(JActivedPaintInfoPainterTest, ReturnsPainterWithoutCrash)
{
    JActivedPaintInfo info;
    (void)info.painter();
}

// ---------------------------------------------------------------------------
// beginSubPicture
// ---------------------------------------------------------------------------

TEST(JActivedPaintInfoBeginSubPictureTest, DoesNotCrash)
{
    JActivedPaintInfo info;
    info.beginSubPicture();
}

// ---------------------------------------------------------------------------
// endSubPicture
// ---------------------------------------------------------------------------

TEST(JActivedPaintInfoEndSubPictureTest, DoesNotCrashAfterBegin)
{
    JActivedPaintInfo info;
    info.beginSubPicture();
    info.endSubPicture();
}

// ---------------------------------------------------------------------------
// addSubPicture
// ---------------------------------------------------------------------------

TEST(JActivedPaintInfoAddSubPictureTest, DoesNotCrash)
{
    JActivedPaintInfo info;
    QPicture pic;
    info.addSubPicture(pic);
}

// ---------------------------------------------------------------------------
// getPathPicture
// ---------------------------------------------------------------------------

TEST(JActivedPaintInfoGetPathPictureTest, ReturnsPictureWithoutCrash)
{
    JActivedPaintInfo info;
    QPainterPath path;
    path.moveTo(0, 0);
    path.lineTo(100, 100);
    QPen pen(Qt::black, 2);
    QBrush brush(Qt::red);
    (void)info.getPathPicture(path, pen, brush);
}

// ===========================================================================
// JGeomeCommand / JDyLayerCmdBase
// ===========================================================================

// ---------------------------------------------------------------------------
// JGeomeCommand constructor
// ---------------------------------------------------------------------------

TEST(JGeomeCommandConstructorTest, StoresAllParameters)
{
    JGeomeCommand cmd(QPointF(10, 20), 45.0, 1.0,
                      QRectF(0, 0, 100, 100), QTransform());
    EXPECT_EQ(cmd._pos, QPointF(10, 20));
    EXPECT_DOUBLE_EQ(cmd._rotate, 45.0);
    EXPECT_DOUBLE_EQ(cmd._z, 1.0);
    EXPECT_EQ(cmd._rect, QRectF(0, 0, 100, 100));
    EXPECT_EQ(cmd._trans, QTransform());
}

// ---------------------------------------------------------------------------
// JGeomeCommand::cmdType
// ---------------------------------------------------------------------------

TEST(JGeomeCommandCmdTypeTest, ReturnsGeometryCommandType)
{
    JGeomeCommand cmd(QPointF(10, 20), 45.0, 1.0,
                      QRectF(0, 0, 100, 100), QTransform());
    EXPECT_EQ(cmd.cmdType(), 1);
}

// ---------------------------------------------------------------------------
// JDyLayerCmdBase::creatCmd (tested via JGeomeCommand)
// ---------------------------------------------------------------------------

TEST(JDyLayerCmdBaseCreatCmdTest, CreatCmdDoesNotCrash)
{
    JGeomeCommand cmd(QPointF(10, 20), 45.0, 1.0,
                      QRectF(0, 0, 100, 100), QTransform());
    (void)cmd.creatCmd(0);
}

// ---------------------------------------------------------------------------
// JGeomeCommand serialization / deserialization round-trip
// ---------------------------------------------------------------------------

TEST(JGeomeCommandSerializationTest, RoundTripPreservesAllFields)
{
    JGeomeCommand src(QPointF(10, 20), 45.0, 1.0,
                      QRectF(0, 0, 100, 100), QTransform());
    JGeomeCommand dst(QPointF(0, 0), 0.0, 0.0,
                      QRectF(0, 0, 10, 10), QTransform());

    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    QDataStream out(&buffer);
    src.serialization(out);
    buffer.close();

    buffer.open(QIODevice::ReadOnly);
    QDataStream in(&buffer);
    dst.deserialization(in);
    buffer.close();

    // All five serialized fields must survive the round-trip unchanged.
    EXPECT_EQ(dst._pos, QPointF(10, 20));
    EXPECT_DOUBLE_EQ(dst._rotate, 45.0);
    EXPECT_DOUBLE_EQ(dst._z, 1.0);
    EXPECT_EQ(dst._rect, QRectF(0, 0, 100, 100));
    EXPECT_EQ(dst._trans, QTransform());
}
