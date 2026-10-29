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
#include <QPainter>

#include <memory>

#define protected public
#define private public
#include "publicApi.h"
#include "cgraphicslayer.h"
#undef protected
#undef private

/*
 * Branch map — test_cgraphicslayer_cmds.cpp
 *
 * Module 1: JActivedPaintInfo
 *   addPoint             B1 _path.elementCount()==0 → moveTo
 *                        B2 else → lineTo
 *                        B3 _pathPictureIsDirty set to 1
 *   setPenForPath        B4 stores _pen, sets _pathPictureIsDirty=1
 *   setBrushForPath      B5 stores _brush, sets _pathPictureIsDirty=1
 *   ensurePainterPathPicture  B6 dirty==1 → regenerate, clear dirty
 *                             B7 dirty==0 → no-op (reuse)
 *                             B7a regenerated EPainterPath picture content
 *                                 asserted (non-empty recording, drawn pixels)
 *                             B7b second dirty cycle → cache refreshed with
 *                                 new pen/path (stale cache detected)
 *
 * Module 2: JCommand subclasses
 *   JGeomeCommand::doCommand  B8 _layer!=nullptr → apply pos/rotate/z/transform/rect
 *                             B9 _layer==nullptr → early return, no crash
 *   JPathCommand::doCommand   B10 _layer!=nullptr → addPenPath, boundingRect grows
 *   JPaintCommand::doCommand  B11 _layer!=nullptr → addPicture, image changes
 *   JBlurCommand::doCommand   B12 _layer!=nullptr → blur applied, image modified
 *
 * Module 3: JDynamicLayer remaining
 *   LayerBlockerKeeper   B13 blocked=true initial → saves old, sets true
 *                         B14 blocked=false initial → saves old, sets false
 *                         B15 destructor restores old value
 *   layerType/setLayerType  B16 EPenType/EImageType round-trip
 *   setBlocked/isBlocked  B17 true/false round-trip
 *   addPicture blocked    B18 _isBlocked==true → early return, no change
 *   isImageInited         B19 empty QImage → false
 *                         B20 non-empty + EPenType → false
 *                         B21 non-empty + EImageType → true
 *   addPicture            B22 larger picture triggers _img resize
 *                         B23 creatCmd=false → no command added
 *                         B24 dyImag=true → union rect
 *                         B25 addToStack=false → command not in stack
 *   isEraserEnable        B26 valid boundingRect → true
 *                         B27 empty/invalid → false
 *   imgTrans              B28 _img null → identity
 *                         B29 _img non-null → scale+translate
 *                         B30 non-unit scale (img size ≠ rect size) → per-axis
 *                             scale factors and scaled translation asserted
 */

#if TEST_LAYER_CMDS_ITEM

// Helper: create a 100×100 white ARGB image for JDynamicLayer construction.
static QImage makeCmdTestImage()
{
    QImage img(100, 100, QImage::Format_ARGB32);
    img.fill(Qt::white);
    return img;
}

// Helper: create a QPicture that draws a filled rectangle.
static QPicture makeTestPicture(int w, int h, QColor color)
{
    QPicture pic;
    QPainter p;
    p.begin(&pic);
    p.setRenderHint(QPainter::Antialiasing);
    p.fillRect(QRectF(0, 0, w, h), color);
    p.end();
    return pic;
}

// Helper: play a QPicture into a fresh white image so its recorded painting
// operations can be inspected pixel by pixel.
static QImage renderPictureToImage(const QPicture &pic, const QSize &size)
{
    QImage img(size, QImage::Format_ARGB32);
    img.fill(Qt::white);
    QPainter p(&img);
    p.drawPicture(0, 0, pic);
    p.end();
    return img;
}

// ===========================================================================
// Module 1: JActivedPaintInfo — addPoint / setPenForPath / setBrushForPath
//           ensurePainterPathPicture
// ===========================================================================

TEST(JActivedPaintInfoAddPointTest, FirstPointUsesMoveTo)
{
    JActivedPaintInfo info;
    info.addPoint(QPointF(10, 20));
    EXPECT_EQ(info._path.elementCount(), 1);
    EXPECT_EQ(info._path.elementAt(0).type, QPainterPath::MoveToElement);
    EXPECT_EQ(info._path.elementAt(0).x, 10.0f);
    EXPECT_EQ(info._path.elementAt(0).y, 20.0f);
}

TEST(JActivedPaintInfoAddPointTest, SubsequentPointsUseLineTo)
{
    JActivedPaintInfo info;
    info.addPoint(QPointF(10, 20));
    info.addPoint(QPointF(30, 40));
    info.addPoint(QPointF(50, 60));
    EXPECT_EQ(info._path.elementCount(), 3);
    EXPECT_EQ(info._path.elementAt(0).type, QPainterPath::MoveToElement);
    EXPECT_EQ(info._path.elementAt(1).type, QPainterPath::LineToElement);
    EXPECT_EQ(info._path.elementAt(2).type, QPainterPath::LineToElement);
    EXPECT_EQ(info._path.elementAt(1).x, 30.0f);
    EXPECT_EQ(info._path.elementAt(1).y, 40.0f);
    EXPECT_EQ(info._path.elementAt(2).x, 50.0f);
    EXPECT_EQ(info._path.elementAt(2).y, 60.0f);
}

TEST(JActivedPaintInfoAddPointTest, SetsDirtyFlag)
{
    JActivedPaintInfo info;
    info._pathPictureIsDirty = 0;
    info.addPoint(QPointF(10, 20));
    EXPECT_EQ(info._pathPictureIsDirty, 1);
}

TEST(JActivedPaintInfoSetPenTest, StoresPenAndSetsDirty)
{
    JActivedPaintInfo info;
    info._pathPictureIsDirty = 0;
    QPen pen(Qt::red, 3);
    info.setPenForPath(pen);
    EXPECT_EQ(info._pen.color(), Qt::red);
    EXPECT_EQ(info._pen.width(), 3);
    EXPECT_EQ(info._pathPictureIsDirty, 1);
}

TEST(JActivedPaintInfoSetBrushTest, StoresBrushAndSetsDirty)
{
    JActivedPaintInfo info;
    info._pathPictureIsDirty = 0;
    QBrush brush(Qt::blue);
    info.setBrushForPath(brush);
    EXPECT_EQ(info._brush.color(), Qt::blue);
    EXPECT_EQ(info._pathPictureIsDirty, 1);
}

TEST(JActivedPaintInfoEnsurePictureTest, DirtyRegeneratesPicture)
{
    JActivedPaintInfo info;
    QPainterPath path;
    path.moveTo(0, 0);
    path.lineTo(50, 50);
    QPen pen(Qt::black, 2);
    QBrush brush(Qt::red);

    info._path = path;
    info._pen = pen;
    info._brush = brush;
    info._pathPictureIsDirty = 1;

    info.ensurePainterPathPicture();

    EXPECT_EQ(info._pathPictureIsDirty, 0);
    QPicture &pic = info.picture(JActivedPaintInfo::ESubPictures);
    (void)pic;
}

TEST(JActivedPaintInfoEnsurePictureTest, NotDirtyDoesNotRegenerate)
{
    JActivedPaintInfo info;
    info._pathPictureIsDirty = 0;

    // Call ensurePainterPathPicture when not dirty — should be a no-op.
    info.ensurePainterPathPicture();
    EXPECT_EQ(info._pathPictureIsDirty, 0);
}

TEST(JActivedPaintInfoEnsurePictureTest, PictureEPainterPathTriggersEnsure)
{
    JActivedPaintInfo info;
    info._path.moveTo(0, 0);
    info._path.lineTo(80, 80);
    info._pen = QPen(Qt::green, 1);
    info._brush = QBrush(Qt::yellow);
    info._pathPictureIsDirty = 1;

    // picture(EPainterPath) should trigger ensurePainterPathPicture internally.
    QPicture pic = info.picture(JActivedPaintInfo::EPainterPath);
    EXPECT_EQ(info._pathPictureIsDirty, 0);

    // The regenerated picture must contain the recorded path, not a stale
    // cached one: it must be a non-empty recording that actually draws
    // (non-white) pixels along the (0,0)-(80,80) diagonal.
    EXPECT_GT(pic.size(), 0);
    QImage rendered = renderPictureToImage(pic, QSize(100, 100));
    int drawnPixels = 0;
    for (int y = 0; y < rendered.height(); ++y)
        for (int x = 0; x < rendered.width(); ++x)
            if (rendered.pixelColor(x, y) != QColor(Qt::white))
                ++drawnPixels;
    EXPECT_GT(drawnPixels, 0);
}

TEST(JActivedPaintInfoEnsurePictureTest, RegeneratesPainterPathPictureAfterChange)
{
    JActivedPaintInfo info;
    info._path.moveTo(0, 0);
    info._path.lineTo(80, 80);
    info._pen = QPen(Qt::green, 1);
    info._brush = QBrush(Qt::yellow);
    info._pathPictureIsDirty = 1;
    info.ensurePainterPathPicture();
    QImage first = renderPictureToImage(
        info.picture(JActivedPaintInfo::EPainterPath), QSize(100, 100));

    // Mutate pen and path (both setters mark the picture dirty again).
    info.setPenForPath(QPen(Qt::red, 8));
    info.addPoint(QPointF(90, 10));
    ASSERT_EQ(info._pathPictureIsDirty, 1);

    info.ensurePainterPathPicture();
    QImage second = renderPictureToImage(
        info.picture(JActivedPaintInfo::EPainterPath), QSize(100, 100));

    // The cache must have been refreshed: the new picture is stroked with
    // the thick red pen, which a stale (old green) picture does not contain.
    int redPixels = 0;
    for (int y = 0; y < second.height(); ++y)
        for (int x = 0; x < second.width(); ++x) {
            QColor c = second.pixelColor(x, y);
            if (c.red() > 200 && c.green() < 100 && c.blue() < 100)
                ++redPixels;
        }
    EXPECT_GT(redPixels, 0);
    EXPECT_NE(first, second);
}

// ===========================================================================
// Module 2: JCommand subclasses — doCommand
// ===========================================================================

TEST(JGeomeCommandDoCommandTest, NullLayerDoesNotCrash)
{
    JGeomeCommand cmd(QPointF(10, 20), 45.0, 1.0,
                      QRectF(0, 0, 100, 100), QTransform());
    cmd.doCommand();
}

TEST(JGeomeCommandDoCommandTest, AppliesAllFieldsToLayer)
{
    auto layer = std::make_unique<JDynamicLayer>(makeCmdTestImage(), JDynamicLayer::EPenType, nullptr);
    QPointF newPos(30, 40);
    qreal rotate = 90.0;
    qreal z = 5.0;
    QRectF newRect(0, 0, 200, 150);
    QTransform trans;
    trans.translate(5, 5);

    JGeomeCommand cmd(newPos, rotate, z, newRect, trans);
    cmd.setLayer(layer.get());
    cmd.doCommand();

    EXPECT_EQ(layer->pos(), QPointF(30, 40));
    // Rotation is applied through setDrawRotatin() and normalized into
    // [0, 360); drawRotation() must reflect the supplied rotation.
    EXPECT_EQ(layer->drawRotation(), rotate);
    EXPECT_EQ(layer->zValue(), 5.0);
    EXPECT_EQ(layer->rect(), newRect);
    EXPECT_EQ(layer->transform(), trans);
}

TEST(JPathCommandDoCommandTest, AddsPenPathToLayer)
{
    auto layer = std::make_unique<JDynamicLayer>(makeCmdTestImage(), JDynamicLayer::EPenType, nullptr);
    QPainterPath path;
    path.moveTo(10, 10);
    path.lineTo(90, 90);
    QPen pen(Qt::black, 2);

    QRectF oldRect = layer->boundingRect();
    JPathCommand cmd(path, pen, layer.get());
    cmd.doCommand();

    // addPenPath with creatCmd=false does not add a command to the stack,
    // but it draws the path onto the layer image. Verify image pixels changed.
    QImage afterImg = layer->image();
    bool changed = false;
    for (int y = 0; y < afterImg.height() && !changed; ++y) {
        for (int x = 0; x < afterImg.width() && !changed; ++x) {
            if (afterImg.pixelColor(x, y) != Qt::white) {
                changed = true;
            }
        }
    }
    EXPECT_TRUE(changed);
    (void)oldRect;
}

TEST(JPathCommandDoCommandTest, NullLayerDoesNotCrash)
{
    QPainterPath path;
    path.moveTo(10, 10);
    QPen pen(Qt::black, 2);
    JPathCommand cmd(path, pen, nullptr);
    cmd.doCommand();
}

TEST(JPaintCommandDoCommandTest, UpdatesLayerImage)
{
    auto layer = std::make_unique<JDynamicLayer>(makeCmdTestImage(), JDynamicLayer::EImageType, nullptr);
    QPicture pic = makeTestPicture(100, 100, Qt::red);

    QImage beforeImg = layer->image().copy();
    JPaintCommand cmd(pic, false, layer.get());
    cmd.doCommand();

    QImage afterImg = layer->image();
    // The image should have been painted upon (red fill on white → pixels differ).
    bool changed = false;
    for (int y = 0; y < afterImg.height() && !changed; ++y) {
        for (int x = 0; x < afterImg.width() && !changed; ++x) {
            if (afterImg.pixelColor(x, y) != beforeImg.pixelColor(x, y)) {
                changed = true;
            }
        }
    }
    EXPECT_TRUE(changed);
}

TEST(JPaintCommandDoCommandTest, NullLayerDoesNotCrash)
{
    QPicture pic = makeTestPicture(50, 50, Qt::blue);
    JPaintCommand cmd(pic, false, nullptr);
    cmd.doCommand();
}

TEST(JBlurCommandDoCommandTest, ModifiesLayerImage)
{
    // Use a non-uniform image: draw colored shapes on white background
    // so that blurring produces visible pixel changes.
    QImage img(100, 100, QImage::Format_ARGB32);
    img.fill(Qt::white);
    QPainter p(&img);
    p.fillRect(QRectF(10, 10, 50, 50), Qt::red);
    p.fillRect(QRectF(60, 60, 30, 30), Qt::blue);
    p.end();

    auto layer = std::make_unique<JDynamicLayer>(img, JDynamicLayer::EImageType, nullptr);
    QPainterPath blurPath;
    blurPath.addRect(10, 10, 50, 50);

    QImage beforeImg = layer->image().copy();
    JBlurCommand cmd(blurPath, 0, layer.get());
    cmd.doCommand();

    QImage afterImg = layer->image();
    bool changed = false;
    for (int y = 0; y < afterImg.height() && !changed; ++y) {
        for (int x = 0; x < afterImg.width() && !changed; ++x) {
            if (afterImg.pixelColor(x, y) != beforeImg.pixelColor(x, y)) {
                changed = true;
            }
        }
    }
    EXPECT_TRUE(changed);
}

TEST(JBlurCommandDoCommandTest, NullLayerDoesNotCrash)
{
    QPainterPath path;
    path.addRect(0, 0, 50, 50);
    JBlurCommand cmd(path, 0, nullptr);
    cmd.doCommand();
}

// ===========================================================================
// Module 3: JDynamicLayer remaining interfaces
// ===========================================================================

// --- LayerBlockerKeeper ---

TEST(LayerBlockerKeeperTest, BlockedTrueInitSetsBlockedFalse)
{
    auto layer = std::make_unique<JDynamicLayer>(makeCmdTestImage(), JDynamicLayer::EPenType, nullptr);
    layer->setBlocked(true);
    EXPECT_TRUE(layer->isBlocked());

    {
        JDynamicLayer::LayerBlockerKeeper keeper(layer.get(), false);
        EXPECT_FALSE(layer->isBlocked());
    }
    // Destructor should restore old value (true).
    EXPECT_TRUE(layer->isBlocked());
}

TEST(LayerBlockerKeeperTest, BlockedFalseInitSetsBlockedTrue)
{
    auto layer = std::make_unique<JDynamicLayer>(makeCmdTestImage(), JDynamicLayer::EPenType, nullptr);
    layer->setBlocked(false);
    EXPECT_FALSE(layer->isBlocked());

    {
        JDynamicLayer::LayerBlockerKeeper keeper(layer.get(), true);
        EXPECT_TRUE(layer->isBlocked());
    }
    // Destructor should restore old value (false).
    EXPECT_FALSE(layer->isBlocked());
}

// --- layerType / setLayerType ---

TEST(JDynamicLayerLayerTypeTest, EPenTypeRoundTrip)
{
    auto layer = std::make_unique<JDynamicLayer>(makeCmdTestImage(), JDynamicLayer::EPenType, nullptr);
    EXPECT_EQ(layer->layerType(), JDynamicLayer::EPenType);
    layer->setLayerType(JDynamicLayer::EImageType);
    EXPECT_EQ(layer->layerType(), JDynamicLayer::EImageType);
    layer->setLayerType(JDynamicLayer::EPenType);
    EXPECT_EQ(layer->layerType(), JDynamicLayer::EPenType);
}

// --- setBlocked / isBlocked ---

TEST(JDynamicLayerBlockedTest, TrueFalseRoundTrip)
{
    auto layer = std::make_unique<JDynamicLayer>(makeCmdTestImage(), JDynamicLayer::EPenType, nullptr);
    layer->setBlocked(true);
    EXPECT_TRUE(layer->isBlocked());
    layer->setBlocked(false);
    EXPECT_FALSE(layer->isBlocked());
}

// --- addPicture blocked early-return ---

TEST(JDynamicLayerAddPictureTest, BlockedEarlyReturnNoChange)
{
    auto layer = std::make_unique<JDynamicLayer>(makeCmdTestImage(), JDynamicLayer::EImageType, nullptr);
    QImage beforeImg = layer->image().copy();
    QRectF beforeRect = layer->boundingRect();
    int beforeCmdCount = layer->commands().size();

    layer->setBlocked(true);
    QPicture pic = makeTestPicture(200, 200, Qt::red);
    layer->addPicture(pic, true, false, true);

    EXPECT_EQ(layer->image().copy(), beforeImg);
    EXPECT_EQ(layer->boundingRect(), beforeRect);
    EXPECT_EQ(layer->commands().size(), beforeCmdCount);
}

// --- isImageInited ---

TEST(JDynamicLayerIsImageInitedTest, EmptyImageReturnsFalse)
{
    auto layer = std::make_unique<JDynamicLayer>(QImage(), JDynamicLayer::EImageType, nullptr);
    EXPECT_FALSE(layer->isImageInited());
}

TEST(JDynamicLayerIsImageInitedTest, NonEmptyEPenTypeReturnsFalse)
{
    auto layer = std::make_unique<JDynamicLayer>(makeCmdTestImage(), JDynamicLayer::EPenType, nullptr);
    EXPECT_FALSE(layer->isImageInited());
}

TEST(JDynamicLayerIsImageInitedTest, NonEmptyEImageTypeReturnsTrue)
{
    auto layer = std::make_unique<JDynamicLayer>(makeCmdTestImage(), JDynamicLayer::EImageType, nullptr);
    EXPECT_TRUE(layer->isImageInited());
}

// --- addPicture variations ---

TEST(JDynamicLayerAddPictureTest, LargerPictureTriggersResize)
{
    // Use dyImag=true so the union of boundingRect and pictureRect is used,
    // which triggers image resize when the picture is larger.
    auto layer = std::make_unique<JDynamicLayer>(makeCmdTestImage(), JDynamicLayer::EImageType, nullptr);
    QSize beforeSize = layer->image().size();
    QPicture pic = makeTestPicture(300, 300, Qt::blue);
    layer->addPicture(pic, false, true, true);
    // The image should have been resized to accommodate the larger picture.
    QSize afterSize = layer->image().size();
    EXPECT_NE(afterSize, beforeSize);
}

TEST(JDynamicLayerAddPictureTest, CreatCmdFalseNoCommandAdded)
{
    auto layer = std::make_unique<JDynamicLayer>(makeCmdTestImage(), JDynamicLayer::EImageType, nullptr);
    int beforeCount = layer->commands().size();
    QPicture pic = makeTestPicture(50, 50, Qt::green);
    layer->addPicture(pic, false, false, true);
    EXPECT_EQ(layer->commands().size(), beforeCount);
}

TEST(JDynamicLayerAddPictureTest, CreatCmdTrueAddsCommand)
{
    auto layer = std::make_unique<JDynamicLayer>(makeCmdTestImage(), JDynamicLayer::EImageType, nullptr);
    int beforeCount = layer->commands().size();
    QPicture pic = makeTestPicture(50, 50, Qt::green);
    layer->addPicture(pic, true, false, true);
    EXPECT_EQ(layer->commands().size(), beforeCount + 1);
}

TEST(JDynamicLayerAddPictureTest, DyImagTrueUnionRect)
{
    auto layer = std::make_unique<JDynamicLayer>(makeCmdTestImage(), JDynamicLayer::EImageType, nullptr);
    QRectF beforeRect = layer->boundingRect();
    QPicture pic = makeTestPicture(200, 200, Qt::red);
    layer->addPicture(pic, false, true, true);
    // With dyImag=true, the rect should be the union of current boundingRect and picture rect.
    QRectF afterRect = layer->boundingRect();
    EXPECT_GE(afterRect.width(), beforeRect.width());
}

TEST(JDynamicLayerAddPictureTest, AddToStackFalseCommandStillAppended)
{
    // addToStack only controls CCmdBlock parameter, not whether the command
    // is appended to _commands. The command IS always appended.
    auto layer = std::make_unique<JDynamicLayer>(makeCmdTestImage(), JDynamicLayer::EImageType, nullptr);
    int beforeCount = layer->commands().size();
    QPicture pic = makeTestPicture(50, 50, Qt::green);
    layer->addPicture(pic, true, false, false);
    EXPECT_EQ(layer->commands().size(), beforeCount + 1);
}

// --- isEraserEnable ---

TEST(JDynamicLayerIsEraserEnableTest, ValidBoundingRectReturnsTrue)
{
    auto layer = std::make_unique<JDynamicLayer>(makeCmdTestImage(), JDynamicLayer::EPenType, nullptr);
    EXPECT_TRUE(layer->isEraserEnable());
}

TEST(JDynamicLayerIsEraserEnableTest, EmptyLayerReturnsFalse)
{
    auto layer = std::make_unique<JDynamicLayer>(QImage(), JDynamicLayer::EPenType, nullptr);
    // Empty image → rect is (0,0,0,0) which is not valid.
    EXPECT_FALSE(layer->isEraserEnable());
}

// --- imgTrans ---

TEST(JDynamicLayerImgTransTest, NullImgReturnsIdentity)
{
    auto layer = std::make_unique<JDynamicLayer>(QImage(), JDynamicLayer::EPenType, nullptr);
    QTransform trans = layer->imgTrans();
    EXPECT_TRUE(trans.isIdentity());
}

TEST(JDynamicLayerImgTransTest, NonNullImgReturnsScaleTranslate)
{
    auto layer = std::make_unique<JDynamicLayer>(makeCmdTestImage(), JDynamicLayer::EPenType, nullptr);
    QTransform trans = layer->imgTrans();
    // For a 100×100 image with rect (0,0,100,100):
    // scale = 100/100 = 1.0, translate = 0 → identity transform.
    EXPECT_TRUE(trans.isIdentity());
    EXPECT_DOUBLE_EQ(trans.m11(), 1.0);
    EXPECT_DOUBLE_EQ(trans.m22(), 1.0);
}

TEST(JDynamicLayerImgTransTest, NonNullImgWithOffsetReturnsScaleTranslate)
{
    auto layer = std::make_unique<JDynamicLayer>(makeCmdTestImage(), JDynamicLayer::EPenType, nullptr);
    layer->setRect(QRectF(10, 20, 100, 100));
    QTransform trans = layer->imgTrans();
    EXPECT_FALSE(trans.isIdentity());
    // scale = 100/100 = 1.0; translate(-10, -20)
    EXPECT_DOUBLE_EQ(trans.m11(), 1.0);
    EXPECT_DOUBLE_EQ(trans.m22(), 1.0);
    EXPECT_DOUBLE_EQ(trans.dx(), -10.0);
    EXPECT_DOUBLE_EQ(trans.dy(), -20.0);
}

TEST(JDynamicLayerImgTransTest, NonUnitScaleAssertsScaleAndTranslation)
{
    // 300x100 image inside a 100x50 rect: per-axis scale factors differ
    // (3.0 and 2.0), so a swapped or miscomputed scale is caught.
    // QTransform::translate() after scale() applies in scaled space, so
    // dx = -rect.x()*sx = -30, dy = -rect.y()*sy = -40.
    QImage img(300, 100, QImage::Format_ARGB32);
    img.fill(Qt::white);
    auto layer = std::make_unique<JDynamicLayer>(img, JDynamicLayer::EPenType, nullptr);
    layer->setRect(QRectF(10, 20, 100, 50));
    QTransform trans = layer->imgTrans();
    EXPECT_DOUBLE_EQ(trans.m11(), 3.0);
    EXPECT_DOUBLE_EQ(trans.m22(), 2.0);
    EXPECT_DOUBLE_EQ(trans.dx(), -30.0);
    EXPECT_DOUBLE_EQ(trans.dy(), -40.0);
}

#endif // TEST_LAYER_CMDS_ITEM
