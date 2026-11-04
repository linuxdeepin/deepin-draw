// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CGraphicsProxyWidget
 *
 * Source: src/drawshape/drawItems/cgraphicsproxywidget.cpp
 *
 * Target methods:
 *   CGraphicsProxyWidget()          — IBeam 光标
 *   parentDrawItem()                — 父图元转换
 *   paint()                         — 离屏渲染不崩溃
 *   itemChange()                    — 标志变化安全
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QGraphicsScene>
#include <QImage>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QTextEdit>

#define protected public
#define private public
#include "drawshape/drawItems/cgraphicsproxywidget.h"
#include "drawshape/drawItems/bzItems/cgraphicsitem.h"
#undef protected
#undef private

// CGraphicsItem 为抽象基类（纯虚 rect()），测试需要一个具体子类
namespace {
class TestDrawItem : public CGraphicsItem
{
public:
    explicit TestDrawItem(QGraphicsItem *parent = nullptr)
        : CGraphicsItem(parent) {}
    QRectF rect() const override { return QRectF(0, 0, 100, 100); }
};
}   // namespace

// =========================================================================
// 构造函数
// =========================================================================

TEST(CGraphicsProxyWidgetCtorTest, SetsIBeamCursor)
{
    CGraphicsProxyWidget proxy;
    EXPECT_EQ(proxy.cursor().shape(), Qt::IBeamCursor);
}

TEST(CGraphicsProxyWidgetParentTest, NoParent_ReturnsNull)
{
    CGraphicsProxyWidget proxy;
    EXPECT_EQ(proxy.parentDrawItem(), nullptr);
}

TEST(CGraphicsProxyWidgetParentTest, DrawItemParent_ReturnsCastPointer)
{
    QGraphicsScene scene;
    TestDrawItem parent;
    scene.addItem(&parent);

    CGraphicsProxyWidget proxy(&parent);
    EXPECT_EQ(proxy.parentDrawItem(), &parent);
}

// =========================================================================
// paint
// =========================================================================

TEST(CGraphicsProxyWidgetPaintTest, WithEmbeddedWidget_RendersNoCrash)
{
    CGraphicsProxyWidget proxy;
    auto *edit = new QTextEdit;
    edit->resize(80, 60);
    edit->setPlainText(QStringLiteral("画板"));
    proxy.setWidget(edit);

    QImage img(120, 120, QImage::Format_ARGB32_Premultiplied);
    img.fill(Qt::white);
    QPainter painter(&img);
    QStyleOptionGraphicsItem option;
    option.rect = QRect(0, 0, 120, 120);
    option.exposedRect = QRectF(0, 0, 120, 120);
    proxy.paint(&painter, &option, nullptr);
    painter.end();

    EXPECT_FALSE(img.isNull());
}

// =========================================================================
// itemChange
// =========================================================================

TEST(CGraphicsProxyWidgetItemChangeTest, FlagsChanged_NoParentItem_NoCrash)
{
    QGraphicsScene scene;
    CGraphicsProxyWidget proxy;
    scene.addItem(&proxy);

    // 触发 ItemFlagsHaveChanged；父图元为空时应直接跳过场景联动逻辑
    proxy.setFlag(QGraphicsItem::ItemHasNoContents, true);

    EXPECT_TRUE(proxy.flags() & QGraphicsItem::ItemHasNoContents);
}
