// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CGraphicsLayer and JDynamicLayer uncovered paths
 * (issue V-9599 §2).
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QPainter>
#include <QImage>
#include <QPainterPath>
#include <QPen>

#define protected public
#define private public
#include "drawshape/drawItems/cgraphicslayer.h"
#include "drawshape/drawItems/bzItems/cgraphicsitem.h"
#include "drawshape/globaldefine.h"
#include "drawshape/cdrawscene.h"
#include "frame/cgraphicsview.h"
#include "publicApi.h"
#undef protected
#undef private

// ---------------------------------------------------------------------------
// CGraphicsLayer::setRect — no intersection (lines 58-61)
// ---------------------------------------------------------------------------

TEST(CGraphicsLayerCoverageTest, SetRect_NoIntersection_CreatesNewImage)
{
    CGraphicsLayer layer;
    layer.setRect(QRectF(0, 0, 100, 100));
    EXPECT_EQ(layer.layerImage().size(), QSize(100, 100));

    // Set a non-intersecting rect → hits the "no intersection" branch.
    layer.setRect(QRectF(500, 500, 50, 50));
    EXPECT_EQ(layer.layerImage().size(), QSize(50, 50));
}

// ---------------------------------------------------------------------------
// JDyLayerCmdBase::creatCmd(2) and creatCmd(3) (lines 125-127)
// ---------------------------------------------------------------------------

TEST(CGraphicsLayerCoverageTest, CreatCmd_Type2_ReturnsPathCommand)
{
    auto *cmd = JDyLayerCmdBase::creatCmd(2);
    EXPECT_NE(cmd, nullptr);
    EXPECT_EQ(cmd->cmdType(), 2);
    // JDyLayerCmdBase has no virtual destructor; delete through derived type.
    delete static_cast<JPathCommand *>(cmd);
}

TEST(CGraphicsLayerCoverageTest, CreatCmd_Type3_ReturnsPaintCommand)
{
    auto *cmd = JDyLayerCmdBase::creatCmd(3);
    EXPECT_NE(cmd, nullptr);
    EXPECT_EQ(cmd->cmdType(), 3);
    // JDyLayerCmdBase has no virtual destructor; delete through derived type.
    delete static_cast<JPaintCommand *>(cmd);
}

// ---------------------------------------------------------------------------
// JDynamicLayer::setAttributionVar(EImageAdaptScene) (lines 328-330)
// ---------------------------------------------------------------------------

TEST(CGraphicsLayerCoverageTest, SetAttributionVar_ImageAdaptScene_NoCrash)
{
    JDynamicLayer layer(QImage(50, 50, QImage::Format_ARGB32));
    layer.setAttributionVar(DrawAttribution::EImageAdaptScene, QVariant(), EChangedFinished);
    SUCCEED();
}

// ---------------------------------------------------------------------------
// JDynamicLayer::addPenPath — blocked returns early (line 353)
// ---------------------------------------------------------------------------

TEST(CGraphicsLayerCoverageTest, AddPenPath_Blocked_ReturnsImmediately)
{
    JDynamicLayer layer;
    layer.setBlocked(true);
    ASSERT_TRUE(layer.isBlocked());

    QPainterPath path;
    path.addEllipse(0, 0, 40, 40);
    layer.addPenPath(path, QPen(Qt::red, 4), 0, true);

    // Image should still be null because addPenPath returned early.
    EXPECT_TRUE(layer.image().isNull());
}

// ---------------------------------------------------------------------------
// JDynamicLayer::addPenPath — type=0 with empty _img creates image (lines 368-371, 394-396)
// ---------------------------------------------------------------------------

TEST(CGraphicsLayerCoverageTest, AddPenPath_Type0_EmptyImg_CreatesImageAndCommand)
{
    JDynamicLayer layer;
    ASSERT_FALSE(layer.isBlocked());
    ASSERT_TRUE(layer.image().isNull());

    QPainterPath path;
    path.addEllipse(0, 0, 40, 40);
    layer.addPenPath(path, QPen(Qt::red, 4), 0, true);

    // Image should now be non-null (created at 368-371).
    EXPECT_FALSE(layer.image().isNull());
    // A JPathCommand should have been appended (394-396).
    EXPECT_FALSE(layer.commands().isEmpty());
}

// ---------------------------------------------------------------------------
// JDynamicLayer::addPenPath — type=0 second call triggers resize (lines 378-380)
// ---------------------------------------------------------------------------

TEST(CGraphicsLayerCoverageTest, AddPenPath_Type0_SecondCall_TriggersResize)
{
    JDynamicLayer layer;

    // First call — creates the image.
    QPainterPath path1;
    path1.addEllipse(0, 0, 40, 40);
    layer.addPenPath(path1, QPen(Qt::red, 4), 0, true);
    EXPECT_FALSE(layer.image().isNull());

    QSize firstSize = layer.image().size();

    // Second call with a path extending beyond the current bounding rect
    // → triggers image resize (378-380).
    QPainterPath path2;
    path2.moveTo(0, 0);
    path2.lineTo(100, 60);
    layer.addPenPath(path2, QPen(Qt::blue, 4), 0, true);

    // The image should have been resized to accommodate the new path.
    EXPECT_GE(layer.image().width(), firstSize.width());
    EXPECT_FALSE(layer.image().isNull());
}
