// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for PageScene attribution and undo-record methods (issue V-6027).
 *
 * Target methods:
 *   - updateAttribution() / ensureAttribution()
 *   - recordItemsInfoToCmd(items, varFor, clearInfo, shoudNotSelectItems)
 *   - recordSecenInfoToCmd(exptype, varFor, items)
 *   - renderToImage(bgColor, desImageSize)
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
#include "frame/cundoredocommand.h"
#undef protected
#undef private

#include <QImage>
#include <QColor>
#include <QSize>

// =========================================================================
// updateAttribution / ensureAttribution
// =========================================================================

TEST(PageSceneAttrib, UpdateAttribution_SetsDirtyFlag)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    scene->attributeDirty = false;
    scene->updateAttribution();
    EXPECT_TRUE(scene->attributeDirty);
}

TEST(PageSceneAttrib, EnsureAttribution_ClearsDirtyFlag)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    scene->attributeDirty = true;
    scene->ensureAttribution();
    EXPECT_FALSE(scene->attributeDirty);
}

TEST(PageSceneAttrib, EnsureAttribution_NotDirty_NoChange)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    scene->attributeDirty = false;
    scene->ensureAttribution();
    EXPECT_FALSE(scene->attributeDirty);
}

// =========================================================================
// recordItemsInfoToCmd
// =========================================================================

TEST(PageSceneAttrib, RecordItemsInfoToCmd_UndoVar_NoCrash)
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

    scene->recordItemsInfoToCmd(bzItems, UndoVar, true, QList<CGraphicsItem *>());
    SUCCEED();
}

TEST(PageSceneAttrib, RecordItemsInfoToCmd_RedoVar_NoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(100, 100), QPoint(200, 200), false);

    auto bzItems = scene->getBzItems();
    ASSERT_GE(bzItems.count(), 1);

    scene->recordItemsInfoToCmd(bzItems, RedoVar, true, QList<CGraphicsItem *>());
    SUCCEED();
}

TEST(PageSceneAttrib, RecordItemsInfoToCmd_EmptyList_NoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    QList<CGraphicsItem *> emptyList;
    scene->recordItemsInfoToCmd(emptyList, UndoVar, true, QList<CGraphicsItem *>());
    SUCCEED();
}

// =========================================================================
// recordSecenInfoToCmd
// =========================================================================

TEST(PageSceneAttrib, RecordSecenInfoToCmd_EGroupChanged_NoCrash)
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

    scene->recordSecenInfoToCmd(CSceneUndoRedoCommand::EGroupChanged, UndoVar, bzItems);
    SUCCEED();
}

TEST(PageSceneAttrib, RecordSecenInfoToCmd_RedoVar_NoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(100, 100), QPoint(200, 200), false);

    auto bzItems = scene->getBzItems();
    ASSERT_GE(bzItems.count(), 1);

    scene->recordSecenInfoToCmd(CSceneUndoRedoCommand::EGroupChanged, RedoVar, bzItems);
    SUCCEED();
}

// =========================================================================
// renderToImage
// =========================================================================

TEST(PageSceneAttrib, RenderToImage_DefaultArgs_ReturnsNonEmptyImage)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(100, 100), QPoint(200, 200), false);

    QImage image = scene->renderToImage();
    EXPECT_FALSE(image.isNull());
    EXPECT_GT(image.width(), 0);
    EXPECT_GT(image.height(), 0);
}

TEST(PageSceneAttrib, RenderToImage_WithBgColor_ReturnsNonEmptyImage)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(100, 100), QPoint(200, 200), false);

    QImage image = scene->renderToImage(Qt::white);
    EXPECT_FALSE(image.isNull());
    EXPECT_GT(image.width(), 0);
    EXPECT_GT(image.height(), 0);
}

TEST(PageSceneAttrib, RenderToImage_WithCustomSize_ReturnsCorrectSize)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    drawApp->setCurrentTool(rectangle);
    createItemByMouse(view, false, QPoint(100, 100), QPoint(200, 200), false);

    QSize customSize(200, 200);
    QImage image = scene->renderToImage(Qt::white, customSize);
    EXPECT_FALSE(image.isNull());
    EXPECT_EQ(image.size(), customSize);
}
