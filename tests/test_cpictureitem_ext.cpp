// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CPictureItem zero-coverage methods (V-8216)
 *
 * Target methods:
 *   - attributions()
 *   - operatingBegin(CGraphItemEvent*)
 *   - operatingEnd(CGraphItemEvent*)
 *   - paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*)
 *
 * Source: src/drawshape/drawItems/bzItems/cpictureitem.cpp
 */

#include <gtest/gtest.h>

#include <QPixmap>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QImage>

#define protected public
#define private public
#include "drawshape/drawItems/bzItems/cpictureitem.h"
#include "drawshape/drawItems/bzItems/cgraphicsitemevent.h"
#include "drawshape/drawItems/bzItems/cgraphicsrectitem.h"
#undef protected
#undef private

#include "test_helpers.h"

// =========================================================================
// operatingBegin / operatingEnd
// =========================================================================

TEST(CPictureItemExt, OperatingBegin_ResetTransform)
{
    QPixmap pix(10, 10);
    pix.fill(Qt::red);
    CPictureItem item(QRectF(0, 0, 100, 80), pix);

    // operatingBegin resets _trans to the identity transform.
    item._trans = QTransform::fromScale(2.0, 3.0);
    ASSERT_FALSE(item._trans.isIdentity());

    CGraphItemEvent event;
    item.operatingBegin(&event);
    EXPECT_TRUE(item._trans.isIdentity());
}

TEST(CPictureItemExt, OperatingEnd_EventType3_RequiresSceneContext)
{
    // event type 3 triggers updateBlurPixmap which calls
    // drawScene()->pageContext()->defaultAttri(...); requires full scene context.
    GTEST_SKIP() << "operatingEnd with type 3 requires scene/page context for blur";
}

TEST(CPictureItemExt, OperatingEnd_OtherEventType_KeepsTransform)
{
    QPixmap pix(10, 10);
    pix.fill(Qt::red);
    CPictureItem item(QRectF(0, 0, 100, 80), pix);

    // operatingEnd with a non-blur event type must not reset _trans —
    // only operatingBegin does that.
    item._trans = QTransform::fromScale(2.0, 3.0);

    CGraphItemEvent event;
    event.setToolEventType(1);
    item.operatingEnd(&event);
    EXPECT_FALSE(item._trans.isIdentity());
}

TEST(CPictureItemExt, OperatingBeginThenEnd_TransformStaysIdentity)
{
    QPixmap pix(10, 10);
    pix.fill(Qt::red);
    CPictureItem item(QRectF(0, 0, 100, 80), pix);

    CGraphItemEvent event;
    item.operatingBegin(&event);
    EXPECT_TRUE(item._trans.isIdentity());
    item.operatingEnd(&event);
    EXPECT_TRUE(item._trans.isIdentity());
}

// =========================================================================
// paint
// =========================================================================

TEST(CPictureItemExt, Paint_DrawsPixmapContent)
{
    QPixmap pix(10, 10);
    pix.fill(Qt::red);
    CPictureItem item(QRectF(0, 0, 100, 80), pix);

    QImage image(120, 100, QImage::Format_ARGB32);
    image.fill(Qt::white);
    QPainter painter(&image);
    QStyleOptionGraphicsItem option;
    item.paint(&painter, &option, nullptr);
    painter.end();

    // paintSelf draws m_pixmap into the item rect — the rendered image
    // must contain the red pixmap, not stay blank white.
    EXPECT_EQ(image.pixelColor(50, 50), QColor(Qt::red));
}

// =========================================================================
// attributions — requires scene context via harness
// =========================================================================

TEST(CPictureItemExt, Attributions_WithScene_ReturnsAttriList)
{
    ensurePageExists();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    QPixmap pix(10, 10);
    pix.fill(Qt::red);
    auto *item = new CPictureItem(QRectF(0, 0, 100, 80), pix);
    scene->addItem(item);
    auto *selGroup = scene->selectGroup();
    ASSERT_NE(selGroup, nullptr);
    selGroup->add(item, false, false);

    auto result = item->attributions();
    EXPECT_FALSE(result.isEmpty());

    scene->clearSelectGroup();
    delete item;
}
