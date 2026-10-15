// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <QImage>
#include <QPainter>
#include <QPen>
#include <QRectF>
#include <memory>

#define protected public
#define private public
#include "cgraphicsview.h"
#include "csizehandlerect.h"
#include "drawshape/drawItems/bzItems/cgraphicscutitem.h"
#include "publicApi.h"
#undef protected
#undef private

/*
 * Branch list (from MCP source snippets):
 * B1: constructor - no initHandle -> nodes empty.
 * B2: initHandle - eight resize directions -> nodes populated.
 * B3: updateHandlesGeometry - handles loop -> handle geometry refreshed.
 * B4: getHighLightPath - always returns an empty path.
 * B5: showControlRects - true -> SelectionHandleActive.
 * B6: showControlRects - false -> SelectionHandleOff.
 * B7: free-mode flag - setter mirrors getter and refreshes controls.
 * B8: resizeCutSize - all eight resize directions bounded by min sizes.
 * B9: getWHRadio - free mode -> -1 even when a view exists.
 * B10: setRatioType/getRatioType - preset and original ratios.
 * B11: original-rect methods - set and restore stored rects.
 * B12: drawTrisectorRect - draws internal thirds.
 * B13: drawFourConner - small area uses pen rectangle.
 * B14: drawFourConner - large area uses corner pixmap path.
 */

class CGraphicsCutItemUnitTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // Arrange
        createNewViewByShortcutKey();
        view = getCurView();
        ASSERT_NE(view, nullptr);
        ASSERT_NE(view->drawScene(), nullptr);

        item = new CGraphicsCutItem(QRectF(0, 0, 100, 80));
        view->drawScene()->addItem(item);
        item->initHandle();
    }

    void TearDown() override
    {
        if (item != nullptr) {
            if (item->scene() != nullptr)
                item->scene()->removeItem(item);
            delete item;
            item = nullptr;
        }
    }

    PageView *view = nullptr;
    CGraphicsCutItem *item = nullptr;
};

TEST(CGraphicsCutItemConstructorTest, ConstructorStoresRectWithoutHandles)
{
    // Arrange
    const QRectF initialRect(10, 20, 70, 50);
    CGraphicsCutItem item(initialRect);

    // Act
    const auto handles = item.nodes();

    // Assert
    EXPECT_EQ(item.originalRect(), initialRect);
    EXPECT_TRUE(handles.isEmpty());
    EXPECT_FALSE(item.isFreeMode());
}

TEST_F(CGraphicsCutItemUnitTest, InitHandle_Creates_ResizeNodes)
{
    // Arrange
    ASSERT_NE(item, nullptr);

    // Act
    const auto handles = item->nodes();

    // Assert
    EXPECT_EQ(handles.size(), 8);
    EXPECT_NE(handles.first(), nullptr);
    EXPECT_NE(handles.last(), nullptr);
}

TEST_F(CGraphicsCutItemUnitTest, Highlight_Path_IsEmpty)
{
    // Arrange
    ASSERT_NE(item, nullptr);

    // Act
    const QPainterPath path = item->getHighLightPath();

    // Assert
    EXPECT_TRUE(path.isEmpty());
    EXPECT_EQ(path.boundingRect(), QRectF());
}

TEST_F(CGraphicsCutItemUnitTest, ShowControl_States_UpdatesNodes)
{
    // Arrange
    ASSERT_NE(item, nullptr);

    // Act
    item->showControlRects(true);

    // Assert
    EXPECT_EQ(item->nodes().first()->m_state, SelectionHandleActive);

    // Act
    item->showControlRects(false);

    // Assert
    EXPECT_EQ(item->nodes().first()->m_state, SelectionHandleOff);
}

TEST_F(CGraphicsCutItemUnitTest, FreeMode_Flag_UpdatesControlState)
{
    // Arrange
    ASSERT_NE(item, nullptr);

    // Act
    item->setIsFreeMode(true);

    // Assert
    EXPECT_TRUE(item->isFreeMode());
    EXPECT_EQ(item->nodes().first()->m_state, SelectionHandleActive);

    // Act
    item->setIsFreeMode(false);

    // Assert
    EXPECT_FALSE(item->isFreeMode());
    EXPECT_EQ(item->nodes().first()->m_state, SelectionHandleActive);
}

TEST_F(CGraphicsCutItemUnitTest, UpdateHandles_Geometry_RefreshesNodes)
{
    // Arrange
    ASSERT_NE(item, nullptr);
    const QRectF geometry(10, 10, 90, 70);
    item->setRect(geometry);
    CSizeHandleRect *rightHandle = item->nodes().at(CSizeHandleRect::Right);

    // Act
    item->updateHandlesGeometry();

    // Assert
    EXPECT_EQ(item->rect(), geometry);
    EXPECT_EQ(rightHandle->dir(), CSizeHandleRect::Right);
    EXPECT_TRUE(rightHandle->boundingRect().isValid());
}

TEST_F(CGraphicsCutItemUnitTest, ResizeCut_EightDirs_KeepsLimits)
{
    // Arrange
    ASSERT_NE(item, nullptr);
    const CSizeHandleRect::EDirection directions[] = {
        CSizeHandleRect::LeftTop, CSizeHandleRect::Top,
        CSizeHandleRect::RightTop, CSizeHandleRect::Right,
        CSizeHandleRect::RightBottom, CSizeHandleRect::Bottom,
        CSizeHandleRect::LeftBottom, CSizeHandleRect::Left
    };
    item->setIsFreeMode(true);
    item->setRect(QRectF(0, 0, 100, 80));

    for (CSizeHandleRect::EDirection direction : directions) {
        // Act
        item->resizeCutSize(direction, QPointF(50, 50), QPointF(60, 70), nullptr);
        const QRectF resizedRect = item->rect();

        // Assert
        EXPECT_EQ(item->isFreeMode(), true);
        EXPECT_GE(resizedRect.width(), 10.0);
        EXPECT_GE(resizedRect.height(), 10.0);
        EXPECT_LE(resizedRect.width(), 10000.0);
        EXPECT_LE(resizedRect.height(), 10000.0);
        item->setRect(QRectF(0, 0, 100, 80));
        item->updateHandlesGeometry();
    }
}

TEST_F(CGraphicsCutItemUnitTest, FreeMode_Radio_ReturnsInvalid)
{
    // Arrange
    ASSERT_NE(item, nullptr);
    item->setIsFreeMode(true);

    // Act
    const qreal ratio = item->getWHRadio();

    // Assert
    EXPECT_EQ(ratio, -1.0);
    EXPECT_TRUE(item->isFreeMode());
}

TEST_F(CGraphicsCutItemUnitTest, RatioType_PresetOriginal_IsStored)
{
    // Arrange
    ASSERT_NE(item, nullptr);
    const QRectF originalRect(0, 0, 60, 90);
    item->setOriginalInitRect(originalRect);

    // Act
    item->setRatioType(cut_1_1);

    // Assert
    EXPECT_EQ(item->getRatioType(), cut_1_1);
    EXPECT_FALSE(item->isFreeMode());

    // Act
    item->setRatioType(cut_original);

    // Assert
    EXPECT_EQ(item->getRatioType(), cut_original);
    EXPECT_FALSE(item->isFreeMode());
}

TEST_F(CGraphicsCutItemUnitTest, OriginalRect_Setters_KeepGeometry)
{
    // Arrange
    ASSERT_NE(item, nullptr);
    const QRectF original(3, 4, 40, 30);
    const QRectF originalInit(5, 6, 60, 45);

    // Act
    item->setOriginalRect(original);
    item->setOriginalInitRect(originalInit);

    // Assert
    EXPECT_EQ(item->originalRect(), original);
    EXPECT_EQ(item->m_originalInitRect, originalInit);
}

TEST_F(CGraphicsCutItemUnitTest, DrawTrisector_Painter_DoesNotCrash)
{
    // Arrange
    ASSERT_NE(item, nullptr);
    QImage image(100, 100, QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    QPainter painter(&image);

    // Act
    item->drawTrisectorRect(&painter);
    painter.end();

    // Assert
    EXPECT_FALSE(image.isNull());
    EXPECT_EQ(painter.isActive(), false);
    EXPECT_EQ(image.format(), QImage::Format_ARGB32);
}

TEST_F(CGraphicsCutItemUnitTest, DrawFourConner_SmallArea_UsesRectangle)
{
    // Arrange
    ASSERT_NE(item, nullptr);
    item->setRect(QRectF(0, 0, 20, 20));
    QImage image(100, 100, QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    QPainter painter(&image);
    const QPen originalPen = painter.pen();

    // Act
    item->drawFourConner(&painter);

    // Assert
    EXPECT_EQ(painter.pen().color().name().toUpper(), QStringLiteral("#EDEDED"));
    EXPECT_EQ(image.isNull(), false);
    EXPECT_NE(painter.pen(), originalPen);
    painter.end();
}

TEST_F(CGraphicsCutItemUnitTest, DrawFourConner_LargeArea_UsesPixmaps)
{
    // Arrange
    ASSERT_NE(item, nullptr);
    QImage image(160, 160, QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    QPainter painter(&image);

    // Act
    item->drawFourConner(&painter);
    painter.end();

    // Assert
    EXPECT_EQ(image.isNull(), false);
    EXPECT_EQ(painter.isActive(), false);
    EXPECT_TRUE(image.hasAlphaChannel());
}
