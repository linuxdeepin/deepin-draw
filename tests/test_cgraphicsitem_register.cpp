// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CPictureItem factory registration and
 * CGraphicsItemGroup::rasterToSelfLayer (issue V-8626)
 *
 * Existing test_cgraphicsitem_misc2.cpp covers CGraphicsItem::rasterToSelfLayer
 * basic case, but the group version was skipped. These tests cover:
 *   - CPictureItemRegisterNameSpace::creatInstance() (via factory)
 *   - CGraphicsItemGroup::rasterToSelfLayer(bool) with child items
 *
 * Source:
 *   src/drawshape/drawItems/bzItems/cpictureitem.cpp
 *   src/drawshape/drawItems/cgraphicsitemselectedmgr.cpp
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QRectF>
#include <QImage>

#define protected public
#define private public
#include "drawshape/drawItems/bzItems/cpictureitem.h"
#include "drawshape/drawItems/bzItems/cgraphicsrectitem.h"
#include "drawshape/drawItems/bzItems/cgraphicsitem.h"
#include "drawshape/drawItems/cgraphicsitemselectedmgr.h"
#include "drawshape/drawItems/cgraphicslayer.h"
#include "service/dyncreatobject.h"
#include "drawshape/globaldefine.h"
#undef protected
#undef private

// ===========================================================================
// CPictureItemRegisterNameSpace::creatInstance (via ClassObjectFactory)
// ===========================================================================

TEST(CPictureItemRegisterTest, FactoryCreatesPictureItem)
{
    // REGISTITEMCLASS in cgraphicsitem.cpp registers CPictureItem at static init
    auto *obj = ClassObjectFactory::creatObject<CPictureItem>("CPictureItem");
    ASSERT_NE(obj, nullptr);
    EXPECT_GE(obj->type(), 0);
    delete obj;
}

TEST(CPictureItemRegisterTest, FactoryReturnsNullForUnknownName)
{
    auto *obj = ClassObjectFactory::creatObject<CPictureItem>("NonExistentPicture");
    EXPECT_EQ(obj, nullptr);
}

// ===========================================================================
// CGraphicsItemGroup::rasterToSelfLayer
// ===========================================================================

TEST(CGraphicsItemGroupRasterTest, RasterToSelfLayerWithChildItemsNoCrash)
{
    // Create a group with a child rect item that has a valid layer
    CGraphicsItemGroup group;

    CGraphicsRectItem *rectItem = new CGraphicsRectItem(QRectF(0, 0, 50, 50));

    CGraphicsLayer layer;
    layer.setRect(QRectF(0, 0, 100, 100));
    rectItem->setLayer(&layer);

    group.add(rectItem);

    // Call rasterToSelfLayer with deleteSelf=false to avoid scene removal
    group.rasterToSelfLayer(false);

    // The group should not crash and items should still be accessible
    EXPECT_FALSE(group.getBzItems(true).isEmpty());
}

TEST(CGraphicsItemGroupRasterTest, RasterToSelfLayerEmptyGroupNoCrash)
{
    CGraphicsItemGroup group;

    // An empty group should not crash
    group.rasterToSelfLayer(false);
    SUCCEED();
}

TEST(CGraphicsItemGroupRasterTest, RasterToSelfLayerMultipleChildren)
{
    CGraphicsItemGroup group;

    CGraphicsRectItem *rect1 = new CGraphicsRectItem(QRectF(0, 0, 30, 30));
    CGraphicsRectItem *rect2 = new CGraphicsRectItem(QRectF(40, 40, 30, 30));

    CGraphicsLayer layer;
    layer.setRect(QRectF(0, 0, 100, 100));
    rect1->setLayer(&layer);
    rect2->setLayer(&layer);

    group.add(rect1);
    group.add(rect2);

    group.rasterToSelfLayer(false);

    EXPECT_EQ(group.getBzItems(true).size(), 2);
}
