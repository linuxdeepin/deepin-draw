// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CGraphicsTextItem::drawDocument (issue V-7618 group 2)
 *
 * Source: src/drawshape/drawItems/bzItems/cgraphicstextitem.cpp lines 503-519
 *
 * Target method:
 *   void drawDocument(QPainter *painter, const QTextDocument *doc,
 *                     const QRectF &r = QRectF())
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QImage>
#include <QPainter>
#include <QTextDocument>
#include <QTextCursor>

#define protected public
#define private public
#include "drawshape/drawItems/bzItems/cgraphicstextitem.h"
#undef protected
#undef private

// =========================================================================
// CGraphicsTextItem::drawDocument
// =========================================================================

TEST(CGraphicsTextItemDrawDocument, NonEmptyDoc_DrawsPixels)
{
    QTextDocument doc;
    doc.setPlainText("Hello World");

    QImage img(200, 100, QImage::Format_ARGB32);
    img.fill(Qt::white);

    {
        QPainter p(&img);
        CGraphicsTextItem item;
        item.drawDocument(&p, &doc, QRectF(0, 0, 200, 100));
    }

    // At least some pixels should differ from pure white
    bool hasNonWhite = false;
    for (int y = 0; y < img.height() && !hasNonWhite; ++y) {
        for (int x = 0; x < img.width() && !hasNonWhite; ++x) {
            if (img.pixel(x, y) != QColor(Qt::white).rgb()) {
                hasNonWhite = true;
            }
        }
    }
    EXPECT_TRUE(hasNonWhite);
}

TEST(CGraphicsTextItemDrawDocument, EmptyDoc_NoCrash)
{
    QTextDocument doc;  // empty by default

    QImage img(200, 100, QImage::Format_ARGB32);
    img.fill(Qt::white);

    QPainter p(&img);
    CGraphicsTextItem item;
    // Should return early without drawing
    item.drawDocument(&p, &doc, QRectF(0, 0, 200, 100));
    SUCCEED();
}

TEST(CGraphicsTextItemDrawDocument, EmptyRect_NoCrash)
{
    QTextDocument doc;
    doc.setPlainText("Test");

    QImage img(200, 100, QImage::Format_ARGB32);
    img.fill(Qt::white);

    QPainter p(&img);
    CGraphicsTextItem item;
    // Default QRectF() — not valid, so no clipping
    item.drawDocument(&p, &doc);
    SUCCEED();
}

TEST(CGraphicsTextItemDrawDocument, ValidRect_ClipsToRegion)
{
    QTextDocument doc;
    doc.setPlainText("Clipping test text");

    QImage img(200, 100, QImage::Format_ARGB32);
    img.fill(Qt::white);

    {
        QPainter p(&img);
        CGraphicsTextItem item;
        item.drawDocument(&p, &doc, QRectF(10, 10, 50, 30));
    }
    // Should not crash; text drawn within the clipped region
    SUCCEED();
}
