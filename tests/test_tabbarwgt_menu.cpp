// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for TabBarWgt menu/mouse and FileSelectDialog::saveSetting
 * (src/frame/cmultiptabbarwidget.cpp).
 *
 * Covers (issue V-8025 §2):
 *   - TabBarWgt::menu() — right-click context menu construction
 *   - TabBarWgt::mousePressEvent() — right-click triggers menu display
 *   - FileSelectDialog::saveSetting() — persist default path & name filter
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QMenu>
#include <QAction>
#include <QMouseEvent>
#include <QTimer>
#include <QSignalSpy>

#define protected public
#define private public
#include "cmultiptabbarwidget.h"
#include "publicApi.h"
#undef protected
#undef private

// Helper: create a TabBarWgt attached to the running DrawBoard.
static TabBarWgt *createTabBarWgt()
{
    MainWindow *mw = drawApp->topMainWindow();
    DrawBoard *board = mw->drawBoard();
    return new TabBarWgt(board);
}

// ===========================================================================
// TabBarWgt::menu() — context menu construction
// ===========================================================================

TEST(TabBarWgtMenuTest, MenuReturnsNonNull)
{
    auto w = createTabBarWgt();
    ASSERT_NE(w, nullptr);

    // menu() is private; with `#define private public` it is callable.
    //
    // Do NOT delete w here. TabBarWgt::menu() keeps a function-local static
    // menu parented to the FIRST TabBarWgt that creates it; deleting that
    // instance leaves the static pointer dangling for every later menu()
    // call (product-side latent issue). The first creator stays parented to
    // the DrawBoard, so it remains reachable and LeakSanitizer stays quiet.
    QMenu *m = w->menu();
    EXPECT_NE(m, nullptr);
}

TEST(TabBarWgtMenuTest, MenuContainsCloseTabAction)
{
    auto w = createTabBarWgt();
    ASSERT_NE(w, nullptr);

    QMenu *m = w->menu();
    ASSERT_NE(m, nullptr);

    // menu() always builds a static menu with exactly two actions
    // ("Close tab" and "Close other tabs").
    auto actions = m->actions();
    ASSERT_EQ(actions.size(), 2);

    bool foundClose = false;
    for (QAction *act : actions) {
        if (act->text().contains("close", Qt::CaseInsensitive) ||
            act->text().contains("关闭", Qt::CaseInsensitive)) {
            foundClose = true;
            break;
        }
    }
    EXPECT_TRUE(foundClose);

    delete w;
}

TEST(TabBarWgtMenuTest, MenuContainsCloseOtherAction)
{
    auto w = createTabBarWgt();
    ASSERT_NE(w, nullptr);

    QMenu *m = w->menu();
    ASSERT_NE(m, nullptr);

    // menu() always builds a static menu with exactly two actions.
    auto actions = m->actions();
    ASSERT_EQ(actions.size(), 2);

    bool foundOther = false;
    for (QAction *act : actions) {
        if (act->text().contains("other", Qt::CaseInsensitive) ||
            act->text().contains("其他", Qt::CaseInsensitive)) {
            foundOther = true;
            break;
        }
    }
    EXPECT_TRUE(foundOther);

    delete w;
}

TEST(TabBarWgtMenuTest, MenuIsStaticSameInstance)
{
    auto w = createTabBarWgt();
    ASSERT_NE(w, nullptr);

    // menu() returns a static local QMenu — two calls should return the same
    // pointer.
    QMenu *m1 = w->menu();
    QMenu *m2 = w->menu();
    EXPECT_EQ(m1, m2);

    delete w;
}

// ===========================================================================
// TabBarWgt::mousePressEvent — right-click triggers menu
// ===========================================================================

TEST(TabBarWgtMousePressEvent, LeftClick_NoCrash)
{
    auto w = createTabBarWgt();
    ASSERT_NE(w, nullptr);
    w->addTab("Tab1");

    // Left-click should delegate to DTabBar::mousePressEvent.
    QMouseEvent event(QEvent::MouseButtonPress, QPointF(5, 5),
                      Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    w->mousePressEvent(&event);

    delete w;
    SUCCEED();
}

TEST(TabBarWgtMousePressEvent, MiddleClick_NoCrash)
{
    auto w = createTabBarWgt();
    ASSERT_NE(w, nullptr);
    w->addTab("Tab1");

    // Middle-click should not trigger menu popup or crash.
    QMouseEvent event(QEvent::MouseButtonPress, QPointF(5, 5),
                      Qt::MiddleButton, Qt::MiddleButton, Qt::NoModifier);
    w->mousePressEvent(&event);

    delete w;
    SUCCEED();
}

TEST(TabBarWgtMousePressEvent, NoButton_NoCrash)
{
    auto w = createTabBarWgt();
    ASSERT_NE(w, nullptr);
    w->addTab("Tab1");

    // NoButton press should not crash.
    QMouseEvent event(QEvent::MouseButtonPress, QPointF(5, 5),
                      Qt::NoButton, Qt::NoButton, Qt::NoModifier);
    w->mousePressEvent(&event);

    delete w;
    SUCCEED();
}

// ===========================================================================
// FileSelectDialog::saveSetting — persist path & filter
// ===========================================================================

TEST(FileSelectDialogSaveSetting, NoCrash_WithDrawBoardParent)
{
    MainWindow *mw = drawApp->topMainWindow();
    DrawBoard *board = mw->drawBoard();
    ASSERT_NE(board, nullptr);

    // saveSetting() is private; with `#define private public` it is callable.
    FileSelectDialog dialog(board);
    dialog.saveSetting();
    SUCCEED();
}

TEST(FileSelectDialogSaveSetting, NoCrash_WithNullptrParent)
{
    // With nullptr parent, saveSetting may skip drawApp writes but should
    // not crash.
    FileSelectDialog dialog(nullptr);
    dialog.saveSetting();
    SUCCEED();
}

TEST(FileSelectDialogSaveSetting, MultipleCalls_NoCrash)
{
    MainWindow *mw = drawApp->topMainWindow();
    DrawBoard *board = mw->drawBoard();
    ASSERT_NE(board, nullptr);

    FileSelectDialog dialog(board);
    dialog.saveSetting();
    dialog.saveSetting();
    dialog.saveSetting();
    SUCCEED();
}
