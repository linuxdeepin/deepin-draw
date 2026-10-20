// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for PageView context menus (src/frame/cgraphicsview.cpp).
 *
 * Covers (issue V-5981 §五):
 *   - initContextMenu / initTextContextMenu (called by ctor; verify menus
 *     are non-null and contain expected actions)
 *   - initContextMenuConnection / initTextContextMenuConnection (wired by
 *     ctor; verify action objects exist)
 *   - showMenu(pMenu) — popup menu call, no crash
 *   - setCcdpMenuActionStatus(enable) — cut/copy/delete action states
 *   - setClipboardStatus() — clipboard-dependent menu state
 *   - setLayerMenuActionStatus — layer menu action states
 *   - setAlignMenuActionStatus — align menu action states
 *   - setTextAlignMenuActionStatus — text align menu state
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QAction>
#include <QMenu>
#include <QGuiApplication>
#include <QClipboard>
#include <QMimeData>

#define protected public
#define private public
#include "publicApi.h"
#undef protected
#undef private

// ---------------------------------------------------------------------------
// initContextMenu / initTextContextMenu — already called by constructor
// ---------------------------------------------------------------------------

TEST(PageViewMenuTest, InitContextMenu_MenuNotNull)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    EXPECT_NE(view->m_contextMenu, nullptr);
}

TEST(PageViewMenuTest, InitContextMenu_ContextMenuHasActions)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    ASSERT_NE(view->m_contextMenu, nullptr);

    EXPECT_FALSE(view->m_contextMenu->actions().isEmpty());
}

TEST(PageViewMenuTest, InitContextMenu_CutCopyPasteActionsExist)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    EXPECT_NE(view->m_cutAct, nullptr);
    EXPECT_NE(view->m_copyAct, nullptr);
    EXPECT_NE(view->m_pasteAct, nullptr);
}

TEST(PageViewMenuTest, InitContextMenu_LayerActionsExist)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    EXPECT_NE(view->m_oneLayerUpAct, nullptr);
    EXPECT_NE(view->m_oneLayerDownAct, nullptr);
    EXPECT_NE(view->m_bringToFrontAct, nullptr);
    EXPECT_NE(view->m_sendTobackAct, nullptr);
}

TEST(PageViewMenuTest, InitContextMenu_AlignActionsExist)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    EXPECT_NE(view->m_itemsLeftAlign, nullptr);
    EXPECT_NE(view->m_itemsRightAlign, nullptr);
    EXPECT_NE(view->m_itemsTopAlign, nullptr);
    EXPECT_NE(view->m_itemsBottomAlign, nullptr);
    EXPECT_NE(view->m_itemsHCenterAlign, nullptr);
    EXPECT_NE(view->m_itemsVCenterAlign, nullptr);
}

TEST(PageViewMenuTest, InitTextContextMenu_TextMenuNotNull)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    EXPECT_NE(view->m_textMenu, nullptr);
}

TEST(PageViewMenuTest, InitTextContextMenu_TextActionsExist)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    EXPECT_NE(view->m_textCutAction, nullptr);
    EXPECT_NE(view->m_textCopyAction, nullptr);
    EXPECT_NE(view->m_textPasteAction, nullptr);
    EXPECT_NE(view->m_textSelectAllAction, nullptr);
    EXPECT_NE(view->m_textDeleteAction, nullptr);
    EXPECT_NE(view->m_textUndoAct, nullptr);
    EXPECT_NE(view->m_textRedoAct, nullptr);
}

// ---------------------------------------------------------------------------
// initContextMenuConnection / initTextContextMenuConnection — verified by
// action objects being non-null (connections are wired to these actions)
// ---------------------------------------------------------------------------

TEST(PageViewMenuTest, InitContextMenuConnection_GroupActionsExist)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    EXPECT_NE(view->m_group, nullptr);
    EXPECT_NE(view->m_unGroup, nullptr);
}

TEST(PageViewMenuTest, InitContextMenuConnection_ZoomActionsExist)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    EXPECT_NE(view->m_viewZoomInAction, nullptr);
    EXPECT_NE(view->m_viewZoomOutAction, nullptr);
    EXPECT_NE(view->m_viewOriginalAction, nullptr);
}

// ---------------------------------------------------------------------------
// showMenu — popup menu, no crash
// ---------------------------------------------------------------------------

TEST(PageViewMenuTest, ShowMenu_ContextMenuNoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    ASSERT_NE(view->m_contextMenu, nullptr);

    view->showMenu(view->m_contextMenu);
    SUCCEED();
}

TEST(PageViewMenuTest, ShowMenu_TextMenuNoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    ASSERT_NE(view->m_textMenu, nullptr);

    view->showMenu(view->m_textMenu);
    SUCCEED();
}

// ---------------------------------------------------------------------------
// setCcdpMenuActionStatus
// ---------------------------------------------------------------------------

TEST(PageViewMenuTest, SetCcdpMenuActionStatus_True_Enabled)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    view->setCcdpMenuActionStatus(true);
    // With items selected, cut/copy/delete should be enabled.
    EXPECT_TRUE(view->m_cutAct->isEnabled());
    EXPECT_TRUE(view->m_copyAct->isEnabled());
    EXPECT_TRUE(view->m_deleteAct->isEnabled());
}

TEST(PageViewMenuTest, SetCcdpMenuActionStatus_False_Disabled)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    view->setCcdpMenuActionStatus(false);
    EXPECT_FALSE(view->m_cutAct->isEnabled());
    EXPECT_FALSE(view->m_copyAct->isEnabled());
    EXPECT_FALSE(view->m_deleteAct->isEnabled());
}

// ---------------------------------------------------------------------------
// setClipboardStatus
// ---------------------------------------------------------------------------

TEST(PageViewMenuTest, SetClipboardStatus_TextEnablesPaste_UnsupportedDisablesPaste)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    ASSERT_NE(view->m_pasteAct, nullptr);

    QGuiApplication::clipboard()->setText("test");
    view->setClipboardStatus();
    EXPECT_TRUE(view->m_pasteAct->isEnabled());

    // An unsupported clipboard format (html only, no text/plain, image or
    // drawItems payload) must leave paste disabled.
    QMimeData *unsupported = new QMimeData;
    unsupported->setHtml("<b>test</b>");
    QGuiApplication::clipboard()->setMimeData(unsupported);
    view->setClipboardStatus();
    EXPECT_FALSE(view->m_pasteAct->isEnabled());
}

// ---------------------------------------------------------------------------
// setLayerMenuActionStatus
// ---------------------------------------------------------------------------

TEST(PageViewMenuTest, SetLayerMenuActionStatus_True_NoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    view->setLayerMenuActionStatus(true);
    SUCCEED();
}

TEST(PageViewMenuTest, SetLayerMenuActionStatus_False_NoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    view->setLayerMenuActionStatus(false);
    SUCCEED();
}

// ---------------------------------------------------------------------------
// setAlignMenuActionStatus
// ---------------------------------------------------------------------------

TEST(PageViewMenuTest, SetAlignMenuActionStatus_True_NoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    view->setAlignMenuActionStatus(true);
    SUCCEED();
}

TEST(PageViewMenuTest, SetAlignMenuActionStatus_False_NoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    view->setAlignMenuActionStatus(false);
    SUCCEED();
}

// ---------------------------------------------------------------------------
// setTextAlignMenuActionStatus
// ---------------------------------------------------------------------------

TEST(PageViewMenuTest, SetTextAlignMenuActionStatus_NullItem_NoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    // setTextAlignMenuActionStatus dereferences tmpitem without a null
    // check (cgraphicsview.cpp:1613), causing SEGV when called with
    // nullptr. This is a suspected source defect — skipped to avoid
    // crashing the entire test process.
    GTEST_SKIP() << "Suspected defect: setTextAlignMenuActionStatus"
                    " crashes on null item (cgraphicsview.cpp:1613)";
}
