// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for PageScene layer and z-value methods (issue V-6027).
 *
 * Target methods:
 *   - insertLayer(pLayer, index)
 *   - setCurrentLayer(pLayer)
 *   - blockAssignZValue(b)
 *   - getMaxZValue()
 *   - blockSelectionStyle(b)
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#define protected public
#define private public
#include "drawshape/cdrawscene.h"
#include "drawshape/drawItems/bzItems/cgraphicsitem.h"
#include "drawshape/drawItems/bzItems/cgraphicsrectitem.h"
#include "drawshape/drawItems/cgraphicslayer.h"
#include "drawshape/globaldefine.h"
#include "publicApi.h"
#undef protected
#undef private

// =========================================================================
// insertLayer
// =========================================================================

TEST(PageSceneLayer, InsertLayer_Nullptr_ReturnsImmediately)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    int beforeCount = scene->m_layers.count();
    scene->insertLayer(nullptr, 0);
    EXPECT_EQ(scene->m_layers.count(), beforeCount);
}

TEST(PageSceneLayer, InsertLayer_DuplicateNotAdded)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    int beforeCount = scene->m_layers.count();
    if (beforeCount > 0) {
        CGraphicsLayer *existing = scene->m_layers.first();
        scene->insertLayer(existing, 0);
        EXPECT_EQ(scene->m_layers.count(), beforeCount);
    }
}

TEST(PageSceneLayer, InsertLayer_AtIndex0)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    int beforeCount = scene->m_layers.count();
    CGraphicsLayer *newLayer = new CGraphicsLayer();
    scene->insertLayer(newLayer, 0);
    ASSERT_EQ(scene->m_layers.count(), beforeCount + 1);
    EXPECT_EQ(scene->m_layers.first(), newLayer);
}

// =========================================================================
// setCurrentLayer
// =========================================================================

TEST(PageSceneLayer, SetCurrentLayer_SwitchesCurrent)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    CGraphicsLayer *newLayer = new CGraphicsLayer();
    scene->insertLayer(newLayer, 0);
    scene->setCurrentLayer(newLayer);
    EXPECT_EQ(scene->m_currentLayer, newLayer);
}

TEST(PageSceneLayer, SetCurrentLayer_Nullptr)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    scene->setCurrentLayer(nullptr);
    EXPECT_EQ(scene->m_currentLayer, nullptr);
}

// =========================================================================
// blockAssignZValue
// =========================================================================

TEST(PageSceneLayer, BlockAssignZValue_EnableThenDisable)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    scene->blockAssignZValue(true);
    EXPECT_TRUE(scene->blockZAssign);

    scene->blockAssignZValue(false);
    EXPECT_FALSE(scene->blockZAssign);
}

// =========================================================================
// getMaxZValue
// =========================================================================

TEST(PageSceneLayer, GetMaxZValue_EmptyScene_ReturnsNegativeOne)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    scene->resetScene();
    EXPECT_EQ(scene->getBzItems().count(), 0);
    // getMaxZValue returns -1 when bzItems is empty
    qreal result = scene->getMaxZValue();
    EXPECT_EQ(result, -1.0);
}

TEST(PageSceneLayer, GetMaxZValue_WithItems_ReturnsMaxZ)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(100, 100), QPoint(200, 200), false);
    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(300, 300), QPoint(400, 400), false);

    auto bzItems = scene->getBzItems();
    ASSERT_GE(bzItems.count(), 2);

    qreal maxZ = scene->getMaxZValue();
    // bzItems are sorted descending by z, so first has max z
    EXPECT_FLOAT_EQ(maxZ, bzItems.first()->zValue());
}

// =========================================================================
// blockSelectionStyle
// =========================================================================

TEST(PageSceneLayer, BlockSelectionStyle_EnableNoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    // Should not crash when enabling
    scene->blockSelectionStyle(true);
    SUCCEED();
}

TEST(PageSceneLayer, BlockSelectionStyle_DisableNoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    scene->blockSelectionStyle(true);
    scene->blockSelectionStyle(false);
    SUCCEED();
}
