// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CGraphicsProxyWidget (issue V-7767)
 *
 * Source: src/drawshape/drawItems/cgraphicsproxywidget.cpp
 *
 * Target methods:
 *   - Constructor (sets IBeam cursor)
 *   - parentDrawItem() (dynamic_cast to CGraphicsItem)
 *   - paint() (delegates to QGraphicsProxyWidget::paint)
 *   - itemChange() (ItemFlagsHaveChanged handling)
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QApplication>
#include <QGraphicsScene>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QCursor>

#define protected public
#define private public
#include "drawshape/drawItems/cgraphicsproxywidget.h"
#include "drawshape/drawItems/bzItems/cgraphicsitem.h"
#include "drawshape/drawItems/bzItems/cgraphicstextitem.h"
#include "drawshape/globaldefine.h"
#include "publicApi.h"
#undef protected
#undef private

// =========================================================================
// Helper: create a CGraphicsProxyWidget with a parent CGraphicsItem

static PageScene *getTestScene()
{
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    auto *page = board->currentPage();
    return page ? page->scene() : nullptr;
}
// =========================================================================
static CGraphicsProxyWidget *createProxyWithParent(CGraphicsItem *parent)
{
    return new CGraphicsProxyWidget(parent);
}

// =========================================================================
// Constructor — sets IBeam cursor
// =========================================================================

TEST(CGraphicsProxyWidgetConstructor, SetsIBeamCursor)
{
    QGraphicsScene scene;
    CGraphicsProxyWidget *proxy = new CGraphicsProxyWidget();
    scene.addItem(proxy);

    EXPECT_EQ(proxy->cursor().shape(), Qt::IBeamCursor);

    delete proxy;
}

TEST(CGraphicsProxyWidgetConstructor, AcceptsNullParent)
{
    CGraphicsProxyWidget *proxy = new CGraphicsProxyWidget(nullptr);
    ASSERT_NE(proxy, nullptr);
    EXPECT_EQ(proxy->cursor().shape(), Qt::IBeamCursor);
    EXPECT_EQ(proxy->parentItem(), nullptr);

    delete proxy;
}

// =========================================================================
// parentDrawItem — returns parent CGraphicsItem or nullptr
// =========================================================================

TEST(CGraphicsProxyWidgetParentDrawItem, ReturnsNullWhenNoParent)
{
    QGraphicsScene scene;
    CGraphicsProxyWidget *proxy = new CGraphicsProxyWidget();
    scene.addItem(proxy);

    // No parent → should return nullptr
    EXPECT_EQ(proxy->parentDrawItem(), nullptr);

    delete proxy;
}

TEST(CGraphicsProxyWidgetParentDrawItem, ReturnsParentWhenParentIsCGraphicsItem)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    // Create a CGraphicsTextItem as parent
    CGraphicsTextItem *textItem = new CGraphicsTextItem();
    scene->addCItem(textItem);

    // Create proxy with textItem as parent
    CGraphicsProxyWidget *proxy = new CGraphicsProxyWidget(textItem);

    CGraphicsItem *parent = proxy->parentDrawItem();
    EXPECT_NE(parent, nullptr);
    EXPECT_EQ(parent, textItem);

    delete proxy;
    delete textItem;
}

// =========================================================================
// paint — does not crash with offscreen rendering
// =========================================================================

TEST(CGraphicsProxyWidgetPaint, DoesNotCrashOffscreen)
{
    QGraphicsScene scene;
    CGraphicsProxyWidget *proxy = new CGraphicsProxyWidget();
    scene.addItem(proxy);

    QImage image(200, 200, QImage::Format_ARGB32);
    image.fill(Qt::white);
    QPainter painter(&image);
    QStyleOptionGraphicsItem option;
    option.rect = QRect(0, 0, 200, 200);

    // Should not crash
    proxy->paint(&painter, &option, nullptr);

    delete proxy;
}

// =========================================================================
// itemChange — flag change is safe (no crash without parent/scene)
// =========================================================================

TEST(CGraphicsProxyWidgetItemChange, FlagChangeSafeWithoutParent)
{
    QGraphicsScene scene;
    CGraphicsProxyWidget *proxy = new CGraphicsProxyWidget();
    scene.addItem(proxy);

    // ItemFlagsHaveChanged with no parentDrawItem → should not crash
    // (parentDrawItem returns nullptr, the inner if-guard skips)
    proxy->setFlag(QGraphicsItem::ItemIsMovable, true);

    // If we reach here without crash, the test passes
    SUCCEED();

    delete proxy;
}
