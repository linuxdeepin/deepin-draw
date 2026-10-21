// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for PageScene background and reset methods (issue V-6027).
 *
 * Target methods:
 *   - bgColor() / setBgColor(color)
 *   - resetSceneBackgroundBrush()
 *   - resetScene()
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#define protected public
#define private public
#include "drawshape/cdrawscene.h"
#include "drawshape/drawItems/bzItems/cgraphicsitem.h"
#include "drawshape/drawItems/bzItems/cgraphicsrectitem.h"
#include "drawshape/globaldefine.h"
#include "publicApi.h"
#undef protected
#undef private

#include <QColor>
#include <QGuiApplication>
#include <DGuiApplicationHelper>

DWIDGET_USE_NAMESPACE

// =========================================================================
// bgColor / setBgColor
// =========================================================================

TEST(PageSceneBgReset, SetBgColor_GetReturnsSameColor)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    QColor testColor(100, 150, 200);
    scene->setBgColor(testColor);
    EXPECT_EQ(scene->bgColor(), testColor);
}

TEST(PageSceneBgReset, SetBgColor_TransparentColor)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    scene->setBgColor(Qt::transparent);
    EXPECT_EQ(scene->bgColor(), Qt::transparent);
}

TEST(PageSceneBgReset, SetBgColor_SolidColor)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    scene->setBgColor(Qt::red);
    EXPECT_EQ(scene->bgColor(), Qt::red);
}

// =========================================================================
// resetSceneBackgroundBrush
// =========================================================================

TEST(PageSceneBgReset, ResetSceneBackgroundBrush_LightTheme)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    DGuiApplicationHelper::instance()->setPaletteType(DGuiApplicationHelper::LightType);
    scene->resetSceneBackgroundBrush();
    EXPECT_EQ(scene->bgColor(), Qt::white);
}

TEST(PageSceneBgReset, ResetSceneBackgroundBrush_DarkTheme)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    DGuiApplicationHelper::instance()->setPaletteType(DGuiApplicationHelper::DarkType);
    scene->resetSceneBackgroundBrush();
    EXPECT_EQ(scene->bgColor(), Qt::white);
}

// =========================================================================
// resetScene
// =========================================================================

TEST(PageSceneBgReset, ResetScene_ClearsAllItems)
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
    ASSERT_GT(scene->getBzItems().count(), 0);

    scene->resetScene();
    EXPECT_EQ(scene->getBzItems().count(), 0);
}

TEST(PageSceneBgReset, ResetScene_SelectionGroupEmpty)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(100, 100), QPoint(200, 200), false);
    auto bzItems = scene->getBzItems();
    ASSERT_GT(bzItems.count(), 0);
    scene->selectItem(bzItems.first());
    ASSERT_GT(scene->selectGroup()->items().count(), 0);

    scene->resetScene();
    EXPECT_EQ(scene->selectGroup()->items().count(), 0);
}

TEST(PageSceneBgReset, ResetScene_RecreatesSelectionGroup)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    scene->resetScene();
    EXPECT_NE(scene->selectGroup(), nullptr);
}
