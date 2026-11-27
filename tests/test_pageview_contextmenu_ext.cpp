// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for PageView context menu and text-align menu (issue V-9581 §1).
 *
 * Source: src/frame/cgraphicsview.cpp
 *
 * Target lines:
 *   795-797: contextMenuEvent — shield branch where m_isShowContext=false
 *            or position outside sceneRect → early return (no popup shown)
 *   1619-1621: setTextAlignMenuActionStatus — else branch (autoAdjustSize=false)
 *            → all three text-align actions enabled
 *
 * Construction:
 *   - For contextMenuEvent: set m_isShowContext=false, send QContextMenuEvent;
 *     assert no active popup widget is created.
 *   - For setTextAlignMenuActionStatus: create a real CGraphicsTextItem,
 *     setAutoAdjustSize(false), call the method, assert actions enabled;
 *     then setAutoAdjustSize(true) and assert actions disabled.
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QApplication>
#include <QContextMenuEvent>

#define protected public
#define private public
#include "publicApi.h"
#include "drawshape/globaldefine.h"
#include "drawshape/drawItems/bzItems/cgraphicstextitem.h"
#undef protected
#undef private

// ---------------------------------------------------------------------------
// contextMenuEvent — shield branch (m_isShowContext = false → early return)
// ---------------------------------------------------------------------------

TEST(PageViewContextMenuExtTest, ContextMenuEvent_NotShown_WhenIsShowContextFalse)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    // Set the current tool to selection so CHECK_CURRENTTOOL_RETURN passes.
    drawApp->setCurrentTool(selection);

    // Disable context menu display.
    view->m_isShowContext = false;

    // Send a context menu event to the viewport.
    QPoint pos(50, 50);
    QPoint globalPos = view->viewport()->mapToGlobal(pos);
    QContextMenuEvent ev(QContextMenuEvent::Mouse, pos, globalPos);
    QApplication::sendEvent(view, &ev);
    qApp->processEvents();

    // No popup widget should have been created.
    EXPECT_EQ(qApp->activePopupWidget(), nullptr);

    // Restore.
    view->m_isShowContext = true;
}

TEST(PageViewContextMenuExtTest, ContextMenuEvent_NotShown_WhenPosOutsideSceneRect)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    drawApp->setCurrentTool(selection);
    view->m_isShowContext = true;

    // Send a context menu event far outside the scene rect.
    QPoint pos(5000, 5000);
    QPoint globalPos = view->viewport()->mapToGlobal(pos);
    QContextMenuEvent ev(QContextMenuEvent::Mouse, pos, globalPos);
    QApplication::sendEvent(view, &ev);
    qApp->processEvents();

    EXPECT_EQ(qApp->activePopupWidget(), nullptr);
}

// ---------------------------------------------------------------------------
// setTextAlignMenuActionStatus — else branch (autoAdjustSize=false → enabled)
// ---------------------------------------------------------------------------

TEST(PageViewTextAlignMenuExtTest, SetTextAlignMenuStatus_AutoAdjustSizeFalse_EnablesActions)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    // Create a real text item.
    drawApp->setCurrentTool(text);
    createItemByMouse(view, false, QPoint(100, 100), QPoint(300, 200), false);
    view->slotOnSelectAll();

    auto *item = static_cast<CGraphicsTextItem *>(scene->selectedItems().first());
    ASSERT_EQ(item->type(), TextType);

    // setAutoAdjustSize(false) → else branch (lines 1619-1621): all enabled.
    item->setAutoAdjustSize(false);
    EXPECT_FALSE(item->isAutoAdjustSize());

    view->setTextAlignMenuActionStatus(item);

    EXPECT_TRUE(view->m_textLeftAlignAct->isEnabled());
    EXPECT_TRUE(view->m_textRightAlignAct->isEnabled());
    EXPECT_TRUE(view->m_textCenterAlignAct->isEnabled());

    scene->clearSelectGroup();
    delete item;
}

TEST(PageViewTextAlignMenuExtTest, SetTextAlignMenuStatus_AutoAdjustSizeTrue_DisablesActions)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    drawApp->setCurrentTool(text);
    createItemByMouse(view, false, QPoint(100, 100), QPoint(300, 200), false);
    view->slotOnSelectAll();

    auto *item = static_cast<CGraphicsTextItem *>(scene->selectedItems().first());
    ASSERT_EQ(item->type(), TextType);

    // setAutoAdjustSize(true) → if branch (lines 1614-1617): all disabled.
    item->setAutoAdjustSize(true);
    EXPECT_TRUE(item->isAutoAdjustSize());

    view->setTextAlignMenuActionStatus(item);

    EXPECT_FALSE(view->m_textLeftAlignAct->isEnabled());
    EXPECT_FALSE(view->m_textRightAlignAct->isEnabled());
    EXPECT_FALSE(view->m_textCenterAlignAct->isEnabled());

    scene->clearSelectGroup();
    delete item;
}
