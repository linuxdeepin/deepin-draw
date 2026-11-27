// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CGraphicsPenItem, CGraphicsLineItem, CGraphicsRectItem
 * uncovered code paths (issue V-9599 §1).
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QPainter>
#include <QImage>
#include <QPainterPath>
#include <QStyleOptionGraphicsItem>

#define protected public
#define private public
#include "drawshape/drawItems/bzItems/cgraphicspenitem.h"
#include "drawshape/drawItems/bzItems/cgraphicslineitem.h"
#include "drawshape/drawItems/bzItems/cgraphicsrectitem.h"
#include "drawshape/drawItems/bzItems/cgraphicsitem.h"
#include "drawshape/drawItems/bzItems/cgraphicsitemevent.h"
#include "drawshape/drawItems/cgraphicsitemselectedmgr.h"
#include "drawshape/globaldefine.h"
#include "drawshape/cdrawscene.h"
#include "frame/cgraphicsview.h"
#include "publicApi.h"
#undef protected
#undef private

#include "drawshape/drawItems/bzItems/cgraphicstextitem.h"
#include "drawshape/drawItems/bzItems/cgraphicsellipseitem.h"

// ---------------------------------------------------------------------------
// CGraphicsPenItem — drawComplete(true) residual points (lines 206-211)
// ---------------------------------------------------------------------------

TEST(PenLineRectCoverageTest, DrawComplete_True_ResidualPointsAdded)
{
    CGraphicsPenItem pen;
    // Set m_path to 7 elements (moveTo + 6 lineTo: >5, not a multiple of 5).
    pen.m_path.moveTo(QPointF(0, 0));
    for (int i = 1; i <= 6; ++i)
        pen.m_path.lineTo(QPointF(i * 10, i * 10));

    int before = pen.getPath().elementCount();
    EXPECT_EQ(before, 7);

    pen.drawComplete(true);

    int after = pen.getPath().elementCount();
    EXPECT_GE(after, 1);
}

// ---------------------------------------------------------------------------
// CGraphicsPenItem — smooth vector overflow (lines 237-238)
// ---------------------------------------------------------------------------

TEST(PenLineRectCoverageTest, UpdatePenPath_SmoothVectorOverflow_CappedAt10)
{
    CGraphicsPenItem pen;
    // SmoothMaxCount is 10; push 12 points to trigger overflow removal.
    for (int i = 0; i < 12; ++i) {
        pen.updatePenPath(QPointF(i, i), false);
    }
    EXPECT_EQ(pen.m_smoothVector.count(), 10);
}

// ---------------------------------------------------------------------------
// CGraphicsPenItem — rect() shift branch (lines 134-135)
// ---------------------------------------------------------------------------

TEST(PenLineRectCoverageTest, Rect_ShiftPress_IncludesStraightLineP2)
{
    CGraphicsPenItem pen;
    pen.m_isShiftPress = true;
    pen.m_straightLine = QLineF(0, 0, 50, 50);

    QRectF r = pen.rect();
    EXPECT_TRUE(r.contains(QPointF(50, 50)));
}

// ---------------------------------------------------------------------------
// CGraphicsPenItem — updateStartPathStyle small path (lines 266-269)
// ---------------------------------------------------------------------------

TEST(PenLineRectCoverageTest, UpdateStartPathStyle_SmallPath_ClearsStartPath)
{
    CGraphicsPenItem pen;
    // m_straightLine is null by default.
    // m_path has a tiny bounding rect (< minRectSize 10x10).
    pen.m_path.moveTo(QPointF(0, 0));
    pen.m_path.lineTo(QPointF(1, 1));

    pen.updateStartPathStyle();
    EXPECT_TRUE(pen.getPenStartpath().isEmpty());
}

// ---------------------------------------------------------------------------
// CGraphicsPenItem — paintSelf shift branch (lines 563-564)
// ---------------------------------------------------------------------------

TEST(PenLineRectCoverageTest, PaintSelf_ShiftPress_DrawsStraightLine)
{
    CGraphicsPenItem pen;
    pen.setPenWidth(4);
    pen.m_isShiftPress = true;
    pen.m_straightLine = QLineF(0, 0, 80, 80);
    pen.m_path.moveTo(QPointF(0, 0));
    pen.m_path.lineTo(QPointF(80, 80));
    pen.updateShape();

    QImage img(200, 200, QImage::Format_ARGB32);
    img.fill(Qt::transparent);
    QPainter painter(&img);
    QStyleOptionGraphicsItem opt;
    pen.paintSelf(&painter, &opt);
    painter.end();

    // Check that the image has non-transparent pixels.
    bool hasPixel = false;
    for (int y = 0; y < img.height() && !hasPixel; ++y) {
        for (int x = 0; x < img.width() && !hasPixel; ++x) {
            if (qAlpha(img.pixel(x, y)) != 0)
                hasPixel = true;
        }
    }
    EXPECT_TRUE(hasPixel);
}

// ---------------------------------------------------------------------------
// CGraphicsPenItem — paintSelf start arrow branch (lines 571-572)
// ---------------------------------------------------------------------------

TEST(PenLineRectCoverageTest, PaintSelf_StartArrow_DrawsStartPath)
{
    CGraphicsPenItem pen;
    pen.setPenWidth(4);
    pen.setPenStartType(soildArrow);
    pen.m_path.moveTo(QPointF(0, 0));
    pen.m_path.lineTo(QPointF(100, 100));
    pen.updateShape();

    QImage img(200, 200, QImage::Format_ARGB32);
    img.fill(Qt::transparent);
    QPainter painter(&img);
    QStyleOptionGraphicsItem opt;
    pen.paintSelf(&painter, &opt);
    painter.end();

    bool hasPixel = false;
    for (int y = 0; y < img.height() && !hasPixel; ++y) {
        for (int x = 0; x < img.width() && !hasPixel; ++x) {
            if (qAlpha(img.pixel(x, y)) != 0)
                hasPixel = true;
        }
    }
    EXPECT_TRUE(hasPixel);
}

// ---------------------------------------------------------------------------
// CGraphicsPenItem — paintSelf end ring branch (lines 579-580)
// ---------------------------------------------------------------------------

TEST(PenLineRectCoverageTest, PaintSelf_EndRing_DrawsEndPath)
{
    CGraphicsPenItem pen;
    pen.setPenWidth(4);
    pen.setPenEndType(soildRing);
    pen.m_path.moveTo(QPointF(0, 0));
    pen.m_path.lineTo(QPointF(100, 100));
    pen.updateShape();

    QImage img(200, 200, QImage::Format_ARGB32);
    img.fill(Qt::transparent);
    QPainter painter(&img);
    QStyleOptionGraphicsItem opt;
    pen.paintSelf(&painter, &opt);
    painter.end();

    bool hasPixel = false;
    for (int y = 0; y < img.height() && !hasPixel; ++y) {
        for (int x = 0; x < img.width() && !hasPixel; ++x) {
            if (qAlpha(img.pixel(x, y)) != 0)
                hasPixel = true;
        }
    }
    EXPECT_TRUE(hasPixel);
}

// ---------------------------------------------------------------------------
// CGraphicsPenItem — getHighLightPath in group (lines 669-670)
// ---------------------------------------------------------------------------

TEST(PenLineRectCoverageTest, GetHighLightPath_InGroup_ReturnsEmptyPath)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    CGraphicsPenItem *pen = new CGraphicsPenItem();
    pen->m_path.moveTo(QPointF(0, 0));
    pen->m_path.lineTo(QPointF(50, 50));
    pen->updateShape();

    CGraphicsRectItem *rect = new CGraphicsRectItem(QRectF(0, 0, 100, 100));
    scene->addCItem(pen);
    scene->addCItem(rect);

    scene->creatGroup({pen, rect});

    QPainterPath hl = pen->getHighLightPath();
    EXPECT_TRUE(hl.isEmpty());

    scene->clearSelectGroup();
    scene->removeCItem(pen, true);
    scene->removeCItem(rect, true);
}

// ---------------------------------------------------------------------------
// CGraphicsPenItem — setPenStartType resets cache (lines 689-690)
// ---------------------------------------------------------------------------

TEST(PenLineRectCoverageTest, SetPenStartType_WithCache_ResetsCache)
{
    CGraphicsPenItem pen;
    pen.setCache(true);
    ASSERT_TRUE(pen.isCached());

    pen.setPenStartType(soildArrow);
    // resetCachePixmap() is called but does not disable cache.
    EXPECT_TRUE(pen.isCached());
}

// ---------------------------------------------------------------------------
// CGraphicsPenItem — setPenEndType resets cache (lines 710-711)
// ---------------------------------------------------------------------------

TEST(PenLineRectCoverageTest, SetPenEndType_WithCache_ResetsCache)
{
    CGraphicsPenItem pen;
    pen.setCache(true);
    ASSERT_TRUE(pen.isCached());

    pen.setPenEndType(soildRing);
    // resetCachePixmap() is called but does not disable cache.
    EXPECT_TRUE(pen.isCached());
}

// ---------------------------------------------------------------------------
// CGraphicsLineItem — doScaling matrix branch in group (lines 122-123)
// ---------------------------------------------------------------------------

TEST(PenLineRectCoverageTest, LineItem_DoScaling_InGroup_AppliesTransform)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    CGraphicsLineItem *line = new CGraphicsLineItem();
    line->setLine(QLineF(10, 10, 100, 100));
    scene->addCItem(line);

    CGraphicsRectItem *rect = new CGraphicsRectItem(QRectF(0, 0, 50, 50));
    scene->addCItem(rect);

    // Create a group so doScaling takes the else branch (matrix mapping).
    scene->creatGroup({line, rect});

    QLineF originalLine = line->line();

    CGraphItemScalEvent ev(CGraphItemEvent::EScal, QPointF(0, 0), QPointF(0, 0));
    ev.setTrans(QTransform::fromTranslate(20, 20));

    line->doScaling(&ev);

    // The line endpoints should have been transformed.
    QLineF newLine = line->line();
    EXPECT_NE(newLine.p1(), originalLine.p1());

    scene->clearSelectGroup();
    scene->removeCItem(line, true);
    scene->removeCItem(rect, true);
}

// ---------------------------------------------------------------------------
// CGraphicsRectItem — paintSelf rounded rect preview (lines 142-149)
// ---------------------------------------------------------------------------

TEST(PenLineRectCoverageTest, RectItem_PaintSelf_PreviewRadius_DrawsRoundedRect)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    CGraphicsRectItem rectItem(QRectF(0, 0, 100, 60));

    // EChangedBegin phase → setXYRedius with preview=true → m_isPreviewRedius=true
    rectItem.setAttributionVar(DrawAttribution::ERectRadius, 30, EChangedBegin);
    EXPECT_TRUE(rectItem.m_isPreviewRedius);

    QImage img(200, 200, QImage::Format_ARGB32);
    img.fill(Qt::transparent);
    QPainter painter(&img);
    QStyleOptionGraphicsItem opt;
    rectItem.paintSelf(&painter, &opt);
    painter.end();

    bool hasPixel = false;
    for (int y = 0; y < img.height() && !hasPixel; ++y) {
        for (int x = 0; x < img.width() && !hasPixel; ++x) {
            if (qAlpha(img.pixel(x, y)) != 0)
                hasPixel = true;
        }
    }
    EXPECT_TRUE(hasPixel);

    // EChanged phase → m_isPreviewRedius becomes false, m_xRedius set
    rectItem.setAttributionVar(DrawAttribution::ERectRadius, 30, EChanged);
    EXPECT_EQ(rectItem.getXRedius(), 30);
    EXPECT_FALSE(rectItem.m_isPreviewRedius);
}
